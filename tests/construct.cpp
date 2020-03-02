#include "framework/types.hpp"
#include "framework/catch.hpp"

#include <memory>
#include <type_traits>

TEST_CASE("callables can be constructed from various sources", "[construct]")
{
  SECTION("callables can be copied or moved from other callables of the same type")
  {
    REQUIRE(std::is_nothrow_copy_constructible_v<testing_type>);
    REQUIRE(std::is_nothrow_move_constructible_v<testing_type>);
  }
  SECTION("from a functor (class with `operator()`), callables can")
  {
    SECTION("copy from") { REQUIRE(std::is_nothrow_constructible_v<testing_type, functor const&>); }
    SECTION("reference") { REQUIRE(std::is_nothrow_constructible_v<testing_type, functor&>); }
    SECTION("move from") { REQUIRE(std::is_nothrow_constructible_v<testing_type, functor&&>); }
    SECTION("point to (raw pointer)") { REQUIRE(std::is_nothrow_constructible_v<testing_type, functor*>); }
    SECTION("point to (shared pointer)")
    {
      REQUIRE(std::is_nothrow_constructible_v<testing_type, std::shared_ptr<functor>&>);
    }
  }
  SECTION("from an object (class with public methods), callables can")
  {
    SECTION("copy from")
    {
      REQUIRE(std::is_nothrow_constructible_v<testing_type, object const&, decltype(&object::method)>);
    }
    SECTION("reference") { REQUIRE(std::is_nothrow_constructible_v<testing_type, object&, decltype(&object::method)>); }
    SECTION("move from")
    {
      REQUIRE(std::is_nothrow_constructible_v<testing_type, object&&, decltype(&object::method)>);
    }
    SECTION("point to (raw pointer)")
    {
      REQUIRE(std::is_nothrow_constructible_v<testing_type, object*, decltype(&object::method)>);
    }
    SECTION("point to (shared pointer)")
    {
      REQUIRE(std::is_nothrow_constructible_v<testing_type, std::shared_ptr<object>&, decltype(&object::method)>);
    }
  }
  SECTION("callables can be constructed from free funtcions and static methods, as function pointer")
  {
    REQUIRE(std::is_nothrow_constructible_v<testing_type, decltype(&object::static_method)>);

    REQUIRE(std::is_nothrow_constructible_v<testing_type, decltype(&free_function)>);
  }
}