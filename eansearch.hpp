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

class EANSearch
{
public:
	EANSearch(const string & token);

	ProductFull * BarcodeLookup(const string & ean, int language = 1) const;
	ProductFull * IsbnLookup(const string & isbn) const;
	bool VerifyChecksum(const string & ean) const;
	ProductList * ProductSearch(const string & name, int only_language = 99, int page = 0) const;
	ProductList * SimilarProductSearch(const string & name, int only_language = 99, int page = 1) const;
	ProductList * CategorySearch(int category, const string & name, int only_language = 99, int page = 0) const;
	ProductList * BarcodePrefixSearch(const string & prefix, int language = 1, int page = 0) const;
	string IssuingCountryLookup(const string & ean) const;

private:
	bool APICall(const string & params, string & result) const;

	string token;
};

