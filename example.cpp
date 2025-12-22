#include <iostream>
#include <cstdlib>
#include "eansearch.hpp"
using namespace std;

int main() {
	auto token = getenv("EAN_SEARCH_API_TOKEN");
    if (token == nullptr) {
        cout << "Please check your API token" << endl;
        return 1;
    }

	auto * api = new EANSearch(token);

	cout << "*** BarcodeLookup()" << endl;
	string ean = "5099750442227";
	ProductFull * p = api->BarcodeLookup(ean);
	if (p) {
		cout << ean << " is " << p->name
			<< " in category " << p->categoryId << " [" << p->categoryName << "]"
			<< " Google category " << p->googleCategoryId << " issued in " << p->issuingCountry
			<< endl;
		delete p;
	} else {
		cout << ean << " not found" << endl;
	}

	cout << "*** IsbnLookup()" << endl;
	string isbn = "1119578884";
	auto * b = api->IsbnLookup(isbn);
	if (p) {
		cout << isbn << " is book title " << b->name << endl;
		delete b;
	} else {
		cout << isbn << " not found" << endl;
	}

	cout << "*** VerifyChecksum()" << endl;
	cout << ean << " is " << (api->VerifyChecksum(ean) ? "" : "not ") << "valid" << endl;
	ean = "5099750442228"; // invalid
	cout << ean << " is " << (api->VerifyChecksum(ean) ? "" : "not ") << "valid" << endl;

	cout << "*** ProductSearch() Bananaboat" << endl;
	auto * pl = api->ProductSearch("Bananaboat");
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		DeleteProductList(pl);
	}

	cout << "*** SimilarProductSearch() iPhone Max whatever" << endl;
	pl = api->SimilarProductSearch("iPhone Max whatever", 1);
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		DeleteProductList(pl);
	}

	cout << "*** CategorySearch() Bananaboat in Music" << endl;
	pl = api->CategorySearch(45, "Bananaboat");
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		DeleteProductList(pl);
	}

	cout << "*** BarcodePrefixSearch() 4007249146" << endl;
	pl = api->BarcodePrefixSearch("4007249146", 1);
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		DeleteProductList(pl);
	}

	cout << "*** IssuingCountryLookup()" << endl;
	ean = "5099750442227";
	cout << ean << " was issued in " << api->IssuingCountryLookup(ean) << endl;

	cout << "*** BarcodeImage() base64 encoded" << endl;
	ean = "5099750442227";
	cout << ean << " image: <img src=\"data:image/gif;base64," << api->BarcodeImage(ean, 102, 50) << "\">" << endl;

	return 0;
}

