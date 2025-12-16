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

class Product {
public:
	virtual ~Product() { };

	string ean;
	string name;
	int categoryId;
	string categoryName;
	string issuingCountry;
};

class ProductFull : public Product {
public:
	int googleCategoryId;
};

typedef list<Product*> ProductList;

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

class EANSearch
{
public:
	EANSearch(const string & token);

	ProductFull * BarcodeLookup(const string & ean, int language = English) const;
	ProductFull * IsbnLookup(const string & isbn) const;
	bool VerifyChecksum(const string & ean) const;
	ProductList * ProductSearch(const string & name, int only_language = Any, int page = 0) const;
	ProductList * SimilarProductSearch(const string & name, int only_language = Any, int page = 1) const;
	ProductList * CategorySearch(int category, const string & name, int only_language = Any, int page = 0) const;
	ProductList * BarcodePrefixSearch(const string & prefix, int language = English, int page = 0) const;
	string IssuingCountryLookup(const string & ean) const;
	string BarcodeImage(const string & ean, int width = 102, int height = 50) const;

private:
	bool APICall(const string & params, string & result) const;
    static string urlencode(const string & str);
    static ProductList * ParseProductList(const string & str);

	string token;
};

#endif // EANSEARCH_HPP
