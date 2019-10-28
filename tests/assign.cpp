#include <iostream>
#include <cassert>
#include <callable.hpp>

struct functor
{
  int operator()(int v) { return v; }
};

struct object
{
  int method(int v) { return v; }
  static int static_method(int v) { return v; }
};

int
free_function(int v)
{
  return v;
}

/*
  Here are valid ways to assign to a callable
*/

int
main()
{
  using fn = tmf::callable<int(int)>;

  // default initialize an empty callable 'a'
  fn a{};

  // assign 'a' with a class with `int operator()(int)` defined
  // provide an rvalue to have your object possibly moved into the callable
		a = functor{};
  // you can provide an lvalue to have your instance copied into the callable
  functor functor_init{};
  a = functor_init;

  // to initialize with an arbitrary member function provide a class instance
  // and a pointer to member function
  // you can provide an rvalue to have your instance possibly moved into the
  // callable
  a = { object{}, &object::method };
  // you can provide an lvalue to have your instance copied into the callable
  object object_init{};
  a = { object_init, &object::method };

  // to initialize with a free function, provide the address of a
  // free function or static member function
  a = &free_function;
  a = &object::static_method;

  // to initialize with a lambda, simply provide the lambda
  a = [](int) { return 0; };

  // if the lambda can be downcast to a function pointer, this can reduce the storage requirements
  a = static_cast<int (*)(int)>([](int) { return 0; });

  // lambdas can capture, but keep in mind how this might effect the storage requirements
  int b = 42;
  a = [&b](int) { return b; };

  // assign a raw pointer to avoid copies and moves
  functor j_init;
  a = { &j_init };

  object l_init;
  a = { &l_init, &object::method };

  // shared_ptr is a safer way to avoid copies and moves
  a = { std::make_shared<functor>() };
  
  a = { std::make_shared<object>(), &object::method };

  return 0;
}