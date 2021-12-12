#include <catch.hpp>

#include "../src/common.h"


TEST_CASE( "GenFakeIsbn", "[1]" ) {
    REQUIRE( GenFakeIsbn().length() == 14 );
    REQUIRE( GenFakeIsbn().length() == 14 );
    REQUIRE( GenFakeIsbn().length() == 14 );
}

TEST_CASE( "ValidateIsbn", "[2]" ) {
    std::string test1 = "9788374802932";
    std::string test2 = "2343244444444";
    REQUIRE( ValidateIsbn(test1) == 0 );
    REQUIRE( ValidateIsbn(test2) == -1 );
}

TEST_CASE( "gen openssl string", "[3]" ) {
    REQUIRE( !GenerateRandomString().empty());
    log::debug(GenerateRandomString());
}

