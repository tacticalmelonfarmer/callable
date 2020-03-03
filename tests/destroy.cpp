#include "framework/types.hpp"
#include "framework/catch.hpp"

TEST_CASE("when a callable is destroyed it will call through to the sources destructor (if available)", "[destroy]")
{
  SECTION("constant reference")
  {
    int check_value = 0;
    const non_trivial_destructing source{ &check_value };
    testing_type subject{ source };
    subject.~testing_type();
    // don't destroy a referenced entity
    REQUIRE(check_value == 0);
  }
  SECTION("mutable reference")
  {
    int check_value = 0;
    non_trivial_destructing source{ &check_value };
    testing_type subject{ source };
    subject.~testing_type();
    // don't destroy a referenced entity
    REQUIRE(check_value == 0);
  }
  SECTION("temporary reference (two destructor calls)")
  {
    int check_value = 0;
    testing_type subject{ non_trivial_destructing{ &check_value } };
    subject.~testing_type();
    // destructor called twice cause of move
    REQUIRE(check_value == 2);
  }
  SECTION("raw pointer")
  {
    int check_value = 0;
    non_trivial_destructing source{ &check_value };
    testing_type subject{ &source };
    subject.~testing_type();
    // don't destroy a pointed-to entity
    REQUIRE(check_value == 0);
  }
  SECTION("shared pointer reference")
  {
    int check_value = 0;
    auto source = std::make_shared<non_trivial_destructing>(&check_value);
    testing_type subject{ source };
    subject.~testing_type();
    // don't destroy a pointed-to entity
    REQUIRE(check_value == 0);
  }
  SECTION("shared pointer temporary reference")
  {
    int check_value = 0;
    testing_type subject{ std::make_shared<non_trivial_destructing>(&check_value) };
    subject.~testing_type();
    REQUIRE(check_value == 1);
  }
}