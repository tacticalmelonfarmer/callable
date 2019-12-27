#include <callable.hpp>

#include <iostream>

struct functor
{
		int operator()(int& out, int value) { out += 1; return value; }
};

struct object
{
		int method(int& out, int value) { out += 1; return value; }
		static int static_method(int& out, int value) { out += 1; return value; }
};

int
free_function(int& out, int value)
{
		out += 1;
		return value;
}

/*
		Here are valid ways to initialize a callable
*/

int
main()
{
		using fn = tmf::callable<int(int&, int)>;
		int failures = 0;
		int lvalue = 0;

		// to initialize with a functor, provide a class instance
		// - you can provide an rvalue to have your instance possibly moved into the callable
		fn a{ functor{} };
		a(lvalue, 1) == 1 ? 0 : (std::cout << "call test: step 1 failed!\n", ++failures);

		// - you can provide an lvalue to have your instance copied into the callable
		functor b_init{};
		fn b{ b_init };
		b(lvalue, 2) == 2 ? 0 : (std::cout << "call test: step 2 failed!\n", ++failures);

		// to initialize with an arbitrary member function provide a class instance
		// and a pointer to member function
		// - you can provide an rvalue to have your instance possibly moved into the callable
		fn c{ object{}, &object::method };
		c(lvalue, 3) == 3 ? 0 : (std::cout << "call test: step 3 failed!\n", ++failures);

		// - you can provide an lvalue to have your instance copied into the callable
		object d_init{};
		fn d{ d_init, &object::method };
		d(lvalue, 4) == 4 ? 0 : (std::cout << "call test: step 4 failed!\n", ++failures);

		// to initialize with a free function, provide the address of a
		// free function or static member function
		fn e{ &free_function };
		e(lvalue, 5) == 5 ? 0 : (std::cout << "call test: step 5 failed!\n", ++failures);

		fn f{ &object::static_method };
		f(lvalue, 6) == 6 ? 0 : (std::cout << "call test: step 6 failed!\n", ++failures);

		// to initialize with a lambda, simply provide the lambda
		fn g{ [](int& out, int i) { out += 1; return i; } };
		g(lvalue, 7) == 7 ? 0 : (std::cout << "call test: step 7 failed!\n", ++failures);

		// if the lambda can be downcast to a function pointer, this can reduce the size of the stored callable
		fn h{ static_cast<int(*)(int&, int)>([](int& out, int i) { out += 1; return i; }) };
		h(lvalue, 8) == 8 ? 0 : (std::cout << "call test: step 8 failed!\n", ++failures);

		// lambdas can capture, but keep in mind how this might effect the size of the callable
		fn i{ [&g](int& out, int value) { return g(out, value); } };
		i(lvalue, 9) == 9 ? 0 : (std::cout << "call test: step 9 failed!\n", ++failures);

		// initialize with a raw pointer to avoid copies and moves
		functor j_init;
		fn j{ &j_init };
		j(lvalue, 10) == 10 ? 0 : (std::cout << "call test: step 10 failed!\n", ++failures);

		// shared_ptr is a safer way to avoid copies and moves
		fn k{ std::make_shared<functor>() };
		k(lvalue, 11) == 11 ? 0 : (std::cout << "call test: step 11 failed!\n", ++failures);

		// initialize with a raw pointer to avoid copies and moves
		object l_init;
		fn l{ &l_init, &object::method };
		l(lvalue, 12) == 12 ? 0 : (std::cout << "call test: step 12 failed!\n", ++failures);

		// shared_ptr is a safer way to avoid copies and moves
		fn m{ std::make_shared<object>(), &object::method };
		m(lvalue, 13) == 13 ? 0 : (std::cout << "call test: step 13 failed!\n", ++failures);

		return failures;
}