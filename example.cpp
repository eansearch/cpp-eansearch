#include <iostream>
#include <cstdlib>
#include "eansearch.hpp"
using namespace std;

int main() {
	auto token = getenv("EAN_SEARCH_API_TOKEN");
	auto * eansearch = new EANSearch(token);

	cout << "*** BarcodeLookup()" << endl;
	string ean = "5099750442227";
	ProductFull * p = eansearch->BarcodeLookup(ean);
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
	auto * b = eansearch->IsbnLookup(isbn);
	if (p) {
		cout << isbn << " is book title " << b->name << endl;
		delete b;
	} else {
		cout << isbn << " not found" << endl;
	}

	cout << "*** VerifyChecksum()" << endl;
	cout << ean << " is " << (eansearch->VerifyChecksum(ean) ? "" : "not ") << "valid" << endl;
	ean = "5099750442228"; // invalid
	cout << ean << " is " << (eansearch->VerifyChecksum(ean) ? "" : "not ") << "valid" << endl;

	cout << "*** ProductSearch() Bananaboat" << endl;
	auto * pl = eansearch->ProductSearch("Bananaboat");
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		delete pl;
	}

	cout << "*** SimilarProductSearch() iPhone Max whatever" << endl;
	pl = eansearch->SimilarProductSearch("iPhone Max whatever", 1);
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		delete pl;
	}

	cout << "*** CategorySearch() Bananaboat in Music" << endl;
	pl = eansearch->CategorySearch(45, "Bananaboat");
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		delete pl;
	}

	cout << "*** BarcodePrefixSearch() 4007249146" << endl;
	pl = eansearch->BarcodePrefixSearch("4007249146", 1);
	if (pl) {
		for (auto p : *pl) {
			cout << p->ean << " is " << p->name
				<< " in category " << p->categoryId << " [" << p->categoryName << "]"
				<< " issued in " << p->issuingCountry
				<< endl;
		}
		delete pl;
	}

	cout << "*** IssuingCountryLookup()" << endl;
	ean = "5099750442227";
	cout << ean << " was issued in " << eansearch->IssuingCountryLookup(ean) << endl;

	return 0;
}

