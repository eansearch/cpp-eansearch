#include <iostream>
#include <regex>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json/src.hpp>
#include <boost/json.hpp>
//#include <boost/url/src.hpp>
#include <boost/url/encode.hpp>
#include <boost/url/rfc/unreserved_chars.hpp>
#include "eansearch.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace json = boost::json;   // from <boost/json.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
namespace urls = boost::urls;	// from  <boost/url/encode.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace std;

static string urlencode(const string & str) {
	// TODO implement
	//return urls::encode(str, urls::unreserved_chars); // using Boost Urls, but linker error
	return regex_replace(str, std::regex("\\s"), "+"); // for now just replace whitespace
}

static Product * ProductFromJSON(const json::value & api_result) {
	ProductFull * p = new ProductFull();
	p->ean = api_result.at("ean").as_string();
	p->name = api_result.at("name").as_string();
	p->categoryId = stoi(api_result.at("categoryId").as_string().c_str());
	p->categoryName = api_result.at("categoryName").as_string();
	p->issuingCountry = api_result.at("issuingCountry").as_string();
	try {
		p->googleCategoryId = stoi(api_result.at("googleCategoryId").as_string().c_str());
	} catch (const exception & e) {
		// ignore, it's a simple Product
	}
	return p;
}

static ProductList * ParseProductList(const string & str) {
	error_code ec;
	auto api_result = json::parse(str, ec);
	json::array * arr = nullptr;
	if (!ec) {
		arr = api_result.at("productlist").if_array();
	}
	if (ec || !arr) {
		return nullptr;
	}
	ProductList * pl = new ProductList();
	for (auto o : *arr) {
		auto * p = ProductFromJSON(o);
		if (p) {
			pl->push_back(p);
		}
	}
	return pl;
}

EANSearch::EANSearch(const string & token) {
	this->token = token;
}

ProductFull * EANSearch::BarcodeLookup(const string & ean, int language) const
{
	string result;
	if (APICall("op=barcode-lookup&ean=" + ean + "&language=" + to_string(language), result)) {
		error_code ec;
		auto api_result = json::parse(result, ec);
		ProductFull * p = dynamic_cast<ProductFull *>(ProductFromJSON(api_result.at(0)));
		return p;
	} else {
		return nullptr;
	}
}

ProductFull * EANSearch::IsbnLookup(const string & isbn) const
{
	string result;
	if (APICall("op=barcode-lookup&isbn=" + isbn, result)) {
		error_code ec;
		auto api_result = json::parse(result, ec);
		ProductFull * p = dynamic_cast<ProductFull *>(ProductFromJSON(api_result.at(0)));
		return p;
	} else {
		return nullptr;
	}
}

bool EANSearch::VerifyChecksum(const string & ean) const
{
	string result;
	if (APICall("op=verify-checksum&ean=" + ean, result)) {
		error_code ec;
		auto api_result = json::parse(result, ec);
		return (api_result.at(0).at("valid").as_string() == "1");
	} else {
		return false;
	}
}

ProductList * EANSearch::ProductSearch(const string & name, int only_language, int page) const
{
	string result;
	if (APICall("op=product-search&name=" + urlencode(name) + "&language=" + to_string(only_language) + "&page=" + to_string(page), result)) {
		return ParseProductList(result);
	}
	return nullptr;
}

ProductList * EANSearch::SimilarProductSearch(const string & name, int only_language, int page) const
{
	string result;
	if (APICall("op=similar-product-search&name=" + urlencode(name) + "&language=" + to_string(only_language) + "&page=" + to_string(page), result)) {
		return ParseProductList(result);
	}
	return nullptr;
}

ProductList * EANSearch::CategorySearch(int category, const string & name, int only_language, int page) const
{
	string result;
	if (APICall("op=category-search&category=" + to_string(category) + "&name=" + urlencode(name)
				+ "&language=" + to_string(only_language) + "&page=" + to_string(page), result)) {
		return ParseProductList(result);
	}
	return nullptr;
}

ProductList * EANSearch::BarcodePrefixSearch(const string & prefix, int language, int page) const
{
	string result;
	if (APICall("op=barcode-prefix-search&prefix=" + prefix
				+ "&language=" + to_string(language) + "&page=" + to_string(page), result)) {
		return ParseProductList(result);
	}
	return nullptr;
}

string EANSearch::IssuingCountryLookup(const string & ean) const
{
	string result;
	if (APICall("op=issuing-country&ean=" + ean, result)) {
		error_code ec;
		auto api_result = json::parse(result, ec);
		return api_result.at(0).at("issuingCountry").as_string().c_str();
	} else {
		return "";
	}
}

string EANSearch::BarcodeImage(const string & ean, int width, int height) const
{
	string result;
	if (APICall("op=barcode-image&ean=" + ean + "&width=" + to_string(width) + "&height=" + to_string(height), result)) {
        error_code ec;
		auto api_result = json::parse(result, ec);
		return api_result.at(0).at("barcode").as_string().c_str();
	} else {
		return "";
	}
}


bool EANSearch::APICall(const string & params, string & output) const
{
	auto const host = "api.ean-search.org";
	auto const port = "443";
	auto const version = 11;
	string target = "/api?" + params + "&token=" + this->token + "&format=json";

	try {
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);
        tcp::resolver resolver(ioc);
        ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        if(! SSL_set_tlsext_host_name(stream.native_handle(), host)) {// set SNI
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::system::system_error{ec};
        }
        stream.set_verify_callback(ssl::host_name_verification(host));
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);
		output = res.body();
        beast::error_code ec;
        stream.shutdown(ec);
		if (ec == boost::asio::error::eof) {
            ec.assign(0, ec.category());
        }
        if (ec) {
            throw boost::system::system_error{ec};
		}
    }
    catch(std::exception const & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
	return true;
}
