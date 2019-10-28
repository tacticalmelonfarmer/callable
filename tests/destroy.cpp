#include <callable.hpp>
#include <iostream>

struct functor
{
  int operator()(int) { return 0; }
  ~functor()
  {
    std::cout << "functor[" << count << "] destructor called succesfully\n";
  }
  int count;
};

struct object
{
  int method(int) { return 0; }
  ~object()
  {
    std::cout << "object[" << count << "] destructor called succesfully\n";
  }
  int count;
};

int
main()
{
  using fn = tmf::callable<int(int)>;

  // functor[0], called twice due to copy
  fn a{ functor{ 0 } };

  // functor[1]
  functor b_init{ 1 };
  fn b{ b_init };

  // object[0], called twice due to copy
  fn c{ object{ 0 }, &object::method };

  // object[1]
  object d_init{ 1 };
  fn d{ d_init, &object::method };

  // object[2]
  fn e{ [o = object{ 2 }](int) { return 0; } };

  return 0;
}
