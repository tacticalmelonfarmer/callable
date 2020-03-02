#include "framework/types.hpp"
#include "framework/catch.hpp"

#include <type_traits>

TEST_CASE("callables can be assigned with various sources", "[assign]")
{
  REQUIRE(std::is_nothrow_copy_assignable_v<testing_type>);
  REQUIRE(std::is_nothrow_move_assignable_v<testing_type>);
}