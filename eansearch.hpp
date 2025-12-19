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
 * @brief Helper to delete a ProductList object and all its contents.
 * @param pl The list to delete
 */
void DeleteProductList(ProductList * pl);

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

#endif // EANSEARCH_HPP
