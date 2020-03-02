#pragma once

#include <callable.hpp>

using testing_type = tmf::callable<int(int, int&, int const&, int&&, int*)>;

inline int
parameter_test_function(int val, int& ref, int const& cref, int&& rval, int* ptr)
{
  int result = (val + ref + cref + rval + *ptr);
  ref = 0;
  *ptr = 0;
  return result;
}

struct functor
{
  int operator()(int val, int& ref, int const& cref, int&& rval, int* ptr)
  {
    return parameter_test_function(val, ref, cref, std::move(rval), ptr);
  }
};

struct object
{
  int method(int val, int& ref, int const& cref, int&& rval, int* ptr)
  {
    return parameter_test_function(val, ref, cref, std::move(rval), ptr);
  }
  static int static_method(int val, int& ref, int const& cref, int&& rval, int* ptr)
  {
    return parameter_test_function(val, ref, cref, std::move(rval), ptr);
  }
};

inline int
free_function(int val, int& ref, int const& cref, int&& rval, int* ptr)
{
  return parameter_test_function(val, ref, cref, std::move(rval), ptr);
}

struct non_trivial_destructing
{
private:
  int* check;

public:
  non_trivial_destructing(int* init_check)
    : check{ init_check }
  {}
  int operator()(int, int&, int const&, int&&, int*) { return 0; }
  ~non_trivial_destructing() { *check += 1; }
};