#include <callable.hpp>

struct functor
{
  int operator()(int value) { return value; }
};

struct object
{
  int method(int value) { return value; }
  static int static_method(int value) { return value; }
};

int
free_function(int value)
{
  return value;
}

/*
  Here are valid ways to initialize a callable
*/

int
main()
{
  using fn = callable::callable<int(int)>;
  int failures = 0;

  // to initialize with a functor, provide a class instance
  // - you can provide an rvalue to have your instance possibly moved into the callable
  fn a{ functor{} };
  a(1) == 1 ? 0 : ++failures;

  // - you can provide an lvalue to have your instance copied into the callable
  functor b_init{};
  fn b{ b_init };
  b(2) == 2 ? 0 : ++failures;

  // to initialize with an arbitrary member function provide a class instance
  // and a pointer to member function
  // - you can provide an rvalue to have your instance possibly moved into the callable
  fn c{ object{}, &object::method };
  c(3) == 3 ? 0 : ++failures;

  // - you can provide an lvalue to have your instance copied into the callable
  object d_init{};
  fn d{ d_init, &object::method };
  d(4) == 4 ? 0 : ++failures;

  // to initialize with a free function, provide the address of a
  // free function or static member function
  fn e{ &free_function };
  e(5) == 5 ? 0 : ++failures;

  fn f{ &object::static_method };
  f(6) == 6 ? 0 : ++failures;

  // to initialize with a lambda, simply provide the lambda
  fn g{ [](int i) { return i; } };
  g(7) == 7 ? 0 : ++failures;

  // if the lambda can be downcast to a function pointer, this can reduce the size of the stored callable
  fn h{ static_cast<int(*)(int)>([](int i) { return i; }) };
  h(8) == 8 ? 0 : ++failures;

  // lambdas can capture, but keep in mind how this might effect the size of the callable
  fn i{ [&g](int value) { return g(value); } };
  i(9) == 9 ? 0 : ++failures;

  // initialize with a raw pointer to avoid copies and moves
  functor j_init;
  fn j{ &j_init };
  j(10) == 10 ? 0 : ++failures;

  // shared_ptr is a safer way to avoid copies and moves
  fn k{ std::make_shared<functor>() };
  k(11) == 11 ? 0 : ++failures;

  // initialize with a raw pointer to avoid copies and moves
  object l_init;
  fn l{ &l_init, &object::method };
  l(12) == 12 ? 0 : ++failures;

  // shared_ptr is a safer way to avoid copies and moves
  fn m{ std::make_shared<object>(), &object::method };
  m(13) == 13 ? 0 : ++failures;

  return failures;
  }