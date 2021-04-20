#include "framework/types.hpp"
#include "framework/catch.hpp"

TEST_CASE("calling a callable", "[call]")
{
  SECTION("with a functor as source")
  {
    SECTION("constant reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      const functor const_ref_source{};
      testing_type subject{ const_ref_source };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("mutable reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      functor ref_source{};
      testing_type subject{ ref_source };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("temporary reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      testing_type subject{ functor{} };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("raw pointer")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      functor ptr_source{};
      testing_type subject{ &ptr_source };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("shared pointer reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      auto ptr_source = std::make_shared<functor>();
      testing_type subject{ ptr_source };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("shared pointer temporary reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      testing_type subject{ std::make_shared<functor>() };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
  }
  SECTION("with an object as source")
  {
    SECTION("constant reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      const object const_ref_source{};
      testing_type subject{ const_ref_source, &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("mutable reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      object ref_source{};
      testing_type subject{ ref_source, &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("temporary reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      testing_type subject{ object{}, &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("raw pointer")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      object ptr_source{};
      testing_type subject{ &ptr_source, &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("shared pointer reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      auto ptr_source = std::make_shared<object>();
      testing_type subject{ ptr_source, &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("shared pointer temporary reference")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      testing_type subject{ std::make_shared<object>(), &object::method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
  }
  SECTION("with a function pointer as source")
  {
    SECTION("free function")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      functor ptr_source{};
      testing_type subject{ &free_function };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
    SECTION("static member function")
    {
      int ref_data{ 2 };
      const int const_ref_data{ 3 };
      int ptr_data{ 5 };
      functor ptr_source{};
      testing_type subject{ &object::static_method };
      REQUIRE(subject(1, ref_data, const_ref_data, int{ 4 }, &ptr_data) == 15);
      REQUIRE(ref_data == 0);
      REQUIRE(ptr_data == 0);
    }
  }
}