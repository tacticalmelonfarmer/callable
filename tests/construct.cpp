#include <callable.hpp>
#include <utility>

struct functor
{
  int operator()(int) { return 0; }
};

struct object
{
  int method(int) { return 0; }
  static int static_method(int) { return 0; }
};

int
free_function(int)
{
  return 0;
}

/*
  Here are valid ways to initialize a callable
*/

int
main()
{
  // a callable is decribed by an 'unqualified' function signature and a fixed
  // capacity specified in bytes
  // - function signature (not the return type) must not be a pointer or
  // reference type and must not be [const, volatile, &, &&] qualified
  // - capacity[optional] is the size of allocated (stack based) storage;
  // default value is `sizeof(std::uintptr_t) * 4`,
  //   which is a common size for anything other than lambdas/functors that
  //   capture or contain large objects
  using fn = tmf::callable<int(int)>;

  // to initialize with a functor, provide a class instance
  // - you can provide an rvalue to have your instance possibly moved into the
  // callable
  fn a{ functor{} };
  // - you can provide an lvalue to have your instance copied into the callable
  functor b_init{};
  fn b{ b_init };

  // to initialize with an arbitrary member function provide a class instance
  // and a pointer to member function
  // - you can provide an rvalue to have your instance possibly moved into the
  // callable
  fn c{ object{}, &object::method };
  // - you can provide an lvalue to have your instance copied into the callable
  object d_init{};
  fn d{ d_init, &object::method };

  // to initialize with a free function, provide the address of a
  // free function or static member function
  fn e{ &free_function };
  fn f{ &object::static_method };

  // to initialize with a lambda, simply provide the lambda
  fn g{ [](int) { return 0; } };

  // if the lambda can be downcast to a function pointer, this can reduce the
  // size of the stored callable
  using function_pointer = int (*)(int);
  fn h{ static_cast<function_pointer>([](int) { return 0; }) };

  // lambdas can capture, but keep in mind how this might effect the size of the
  // callable
  fn i{ [&g](int) { return g(0); } };

  // initialize with a raw pointer to avoid copies and moves
  functor j_init;
  fn j{ &j_init };

  object k_init;
  fn k{ &k_init, &object::method };

  // shared_ptr is a safer way to avoid copies and moves
  fn l{ std::make_shared<functor>() };

  fn m{ std::make_shared<object>(), &object::method };

  return 0;
}