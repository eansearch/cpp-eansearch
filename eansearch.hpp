/*
 * A C++ class for EAN and ISBN name lookup and validation using the API on ean-search.org
 * https://www.ean-search.org/ean-database-api.html
 * 
 * (c) 2025 Relaxed Communications GmBH, <info@relaxedcommunications.com>
 * 
 * License: MIT https://opensource.org/license/mit
*/

#ifndef EANSEARCH_HPP
#define EANSEARCH_HPP

#include <string>
#include <list>
using namespace std;

/////////////////////////////////////////////////////////////////////
// Interface
/////////////////////////////////////////////////////////////////////

/**
 * @brief Basic product information.
 *
 * Contains common product fields such as the barcode,
 * product name, category id/name and issuing country.
 */
class Product {
public:
    virtual ~Product() { };

    /// Barcode (EAN/GTIN/UPC)
    string ean;
    /// Product name
    string name;
    /// Category identifier (numeric)
    int categoryId;
    /// Category name
    string categoryName;
    /// Issuing country
    string issuingCountry;
};

/**
 * @brief Extended product information.
 *
 * Inherits Product and adds the Google product category id.
 */
class ProductFull : public Product {
public:
    /// Google product category id
    int googleCategoryId;
};

/**
 * @brief Alias for a list of Product pointers.
 *
 * Returned lists must be freed by the caller.
 */
typedef std::list<Product*> ProductList;

/**
 * @brief Language codes used by the API
 */
enum Language {
    English = 1,
    Danish = 2,
    German = 3,
    Spanish = 4,
    Finish = 5,
    French = 6,
    Italian = 8,
    Dutch = 10,
    Norwegian = 11,
    Polish = 12,
    Portuguese = 13,
    Swedish = 15,
    Any = 99
};

/**
 * @brief Main class to interact with the API.
 *
 * Construct with a valid API token. Methods perform synchronous HTTP(S)
 * requests and return either result objects or null/empty values on error.
 */
class EANSearch
{
public:
    /**
     * @brief Construct a new EANSearch object.
     * @param token API token string used for all requests.
     */
    EANSearch(const string & token);

    /**
     * @brief Lookup a single barcode (EAN/GTIN/UPC/ISBN-13).
     * @param ean Barcode string to lookup.
     * @param language Preferred language for the product name (optional).
     * @return Pointer to ProductFull on success, nullptr on failure or not found.
     */
    ProductFull * BarcodeLookup(const string & ean, int language = English) const;

    /**
     * @brief Lookup an ISBN (ISBN-10).
     * @param isbn ISBN-10 string.
     * @return Pointer to ProductFull on success, nullptr on failure or not found.
     */
    ProductFull * IsbnLookup(const string & isbn) const;

    /**
     * @brief Verify the checksum of an EAN/GTIN/UPC/ISBN-13 code.
     * @param ean Barcode to verify.
     * @return true if the checksum is valid, false otherwise or on error.
     */
    bool VerifyChecksum(const string & ean) const;

    /**
     * @brief Exact product search by name.
     * @param name Product name to search for.
     * @param only_language Only return results int this language (Language enum), default Any.
     * @param page Page index for paged results (0-based).
     * @return Pointer to a ProductList on success, nullptr on error or not found.
     *
     * Caller takes ownership of the returned ProductList and must delete it.
     */
    ProductList * ProductSearch(const string & name, int only_language = Any, int page = 0) const;

    /**
     * @brief Search for similar product names.
     * @param name Search terms.
     * @param only_language Only return results int this language (Language enum), default Any.
     * @param page Page index for paged results (1-based).
     * @return Pointer to ProductList on success, nullptr on error.
     *
     * Caller takes ownership of the returned ProductList and must delete it.
     */
    ProductList * SimilarProductSearch(const string & name, int only_language = Any, int page = 1) const;

    /**
     * @brief Search for products within a specific category.
     * @param category Category id to restrict the search to.
     * @param name Product name search term.
     * @param only_language Only return results int this language (Language enum), default Any.
     * @param page Page index for paged results (0-based).
     * @return Pointer to ProductList on success, nullptr on error.
     *
     * Caller takes ownership of the returned ProductList and must delete it.
     */
    ProductList * CategorySearch(int category, const string & name, int only_language = Any, int page = 0) const;

    /**
     * @brief Search products by barcode prefix.
     * @param prefix Barcode prefix digits.
     * @param language Preferred language for the product name (optional).
     * @param page Page index for paged results (0-based).
     * @return Pointer to ProductList on success, nullptr on error.
     *
     * Caller takes ownership of the returned ProductList and must delete it.
     */
    ProductList * BarcodePrefixSearch(const string & prefix, int language = English, int page = 0) const;

    /**
     * @brief Lookup the issuing country for a given barcode.
     * @param ean Barcode.
     * @return Issuing country as string on success, empty string on error.
     */
    string IssuingCountryLookup(const string & ean) const;

    /**
     * @brief Generate a PNG image of the barcode.
     * @param ean Barcode.
     * @param width Image width in pixels (default 102).
     * @param height Image height in pixels (default 50).
     * @return Base64-encoded PNG data on success, empty string on error.
     */
    string BarcodeImage(const string & ean, int width = 102, int height = 50) const;

private:
    bool APICall(const string & params, string & result) const;
    static string urlencode(const string & str);
    static ProductList * ParseProductList(const string & str);

    /// API token provided at construction time
    string token;
};

/////////////////////////////////////////////////////////////////////
// Implementation (existing - declarations remain unchanged)
/////////////////////////////////////////////////////////////////////

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json/src.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace json = boost::json;   // from <boost/json.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

static Product * ProductFromJSON(const json::value & api_result) {
    Product * p = nullptr;
    auto json_product = api_result.if_object();
    if (json_product) {
        if (json_product->if_contains("googleCategoryId")) {
            p = new ProductFull();
            auto * pf = static_cast<ProductFull *>(p);
            pf->googleCategoryId = stoi(json_product->at("googleCategoryId").as_string().c_str());
        } else {
            p = new Product();
        }
        p->ean = json_product->at("ean").as_string();
        p->name = json_product->at("name").as_string();
        p->categoryId = stoi(json_product->at("categoryId").as_string().c_str());
        p->categoryName = json_product->at("categoryName").as_string();
        p->issuingCountry = json_product->at("issuingCountry").as_string();
    }
    return p;
}

EANSearch::EANSearch(const string & token) {
    this->token = token;
}

ProductFull * EANSearch::BarcodeLookup(const string & ean, int language) const
{
    string result;
    if (APICall("op=barcode-lookup&ean=" + ean + "&language=" + to_string(language), result)) {
        auto api_result = json::parse(result);
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
        auto api_result = json::parse(result);
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
        auto api_result = json::parse(result);
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
        auto api_result = json::parse(result);
        return api_result.at(0).at("barcode").as_string().c_str();
    } else {
        return "";
    }
}

/**
 * @brief Perform a synchronous HTTPS GET request to the API.
 * @param params Query parameters (without token/format).
 * @param result Output parameter that receives the raw JSON response body.
 * @return true on success, false on network/SSL/parse error.
 */
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
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) { // set SNI
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::system::system_error{ec};
        }
        stream.set_verify_callback(ssl::host_name_verification(host));
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, "cpp-eansearch/1.0");
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

/**
 * @brief Percent-encode a string for use in URL query components (RFC 3986).
 * @param str Input string to encode.
 * @return Encoded string where non-unreserved characters are percent-encoded.
 */
string EANSearch::urlencode(const string & str) {
    // RFC 3986 percent-encoding for query components: unreserved characters remain unencoded
    // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
    static const char * hex = "0123456789ABCDEF";
    string out;
    out.reserve(str.size() * 3);

    for (unsigned char c : str) {
        if ( (c >= 'A' && c <= 'Z') ||
             (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') ||
             c == '-' || c == '_' || c == '.' || c == '~' ) {
            out.push_back(static_cast<char>(c));
        } else {
            out.push_back('%');
            out.push_back(hex[(c >> 4) & 0xF]);
            out.push_back(hex[c & 0xF]);
        }
    }
    return out;
}

ProductList * EANSearch::ParseProductList(const string & str) {
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

#endif // EANSEARCH_HPP
