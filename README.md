# cpp-eansearch — EAN, GTIN, UPC and ISBN lookup

A C++ class for EAN, GTIN, UPC and ISBN lookup and validation using the API at ean-search.org.

Core implementation:
- Interface: [eansearch.hpp](eansearch.hpp) — class and data structures
- Implementation: [eansearch.cpp](eansearch.cpp)
- Example usage: see [example.cpp](example.cpp)
- Build on Linux: [Makefile](Makefile)
- License: MIT

Primary classes and types
- [`EANSearch`](eansearch.hpp) — main API class
- [`Product`](eansearch.hpp) — basic product info
- [`ProductFull`](eansearch.hpp) — product with added Google product category (inherits [`Product`](eansearch.hpp))
- [`ProductList`](eansearch.hpp) — typedef for product lists

Main public methods on [`EANSearch`](eansearch.hpp)
- [`EANSearch::BarcodeLookup`](eansearch.hpp)
    search by a single EAN, GTIN, UPC or ISBN-13 code
- [`EANSearch::IsbnLookup`](eansearch.hpp)
    search by ISBN code (ISBN-10)
- [`EANSearch::VerifyChecksum`](eansearch.hpp)
    verify whether the checksum on any EAN, GTIN, UPC or ISBN-13 code is valid
- [`EANSearch::ProductSearch`](eansearch.hpp)
    search the product database by product name (exact search)
- [`EANSearch::SimilarProductSearch`](eansearch.hpp)
    search the product database for similar product names
- [`EANSearch::CategorySearch`](eansearch.hpp)
    search the product database by product name, restricted by product category
- [`EANSearch::BarcodePrefixSearch`](eansearch.hpp)
    search by the first digits of a barcode (prefix)
- [`EANSearch::IssuingCountryLookup`](eansearch.hpp)
    check the issuing country of any EAN, GTIN, UPC or ISBN-13 code
- [`EANSearch::BarcodeImage`](eansearch.hpp)
    generate a PNG image of the barcode (base64 encoded)

## Compiling

To compile, you need Boost and OpenSSL installed.

On Debian and Ubuntu:
   ```sh
    sudo apt install libssl-dev boost-dev
   ```

On Windows with vcpgk
   ```sh
    vcpkg install openssl boost --triplet x32-windows
   ```

Build and run the example:
   ```sh
   make
   ./example
   ```

## Using the example

Export your API token as an environment variable:
   ```sh
   export EAN_SEARCH_API_TOKEN=your_token_here
   ```
