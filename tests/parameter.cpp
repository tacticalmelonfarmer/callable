#include <callable.hpp>

#include <iostream>

struct functor
{
		void operator()() { std::cout << "\tfunctor::operator()" << std::endl; }
};

struct object
{
		void method() { std::cout << "\tobject{}.method" << std::endl; }
		static void static_method() { std::cout << "\tobject::static_method" << std::endl; }
};

void
free_function()
{
		std::cout << "\tfree_function" << std::endl;
}

/// functions taking a `callable` arguments, by reference

void use(tmf::callable<void()>& cb)
{
		std::cout << "by mutable-ref:" << std::endl;
		cb();
}

void use(const tmf::callable<void()>& cb)
{
		std::cout << "by constant-ref:" << std::endl;
		cb();
}

void use(tmf::callable<void()>&& cb)
{
		std::cout << "by temporary-ref:" << std::endl;
		cb();
}

/// functions taking a `callable` arguments, by reference

void copy(tmf::callable<void()> cb)
{
		std::cout << "by value:" << std::endl;
		cb();
}

int main()
{
		tmf::callable<void()> functor_lv{ functor{} }, object_lv{ object{}, &object::method }, free_lv{ free_function };
		const tmf::callable<void()> functor_clv{ functor{} }, object_clv{ object{}, &object::method }, free_clv{ free_function };
		
		use(functor_lv);
		use(functor_clv);
		use(functor{});
		copy(functor_lv);

		use(object_lv);
		use(object_clv);
		use({ object{}, &object::method });
		copy(object_lv);

		use(free_lv);
		use(free_clv);
		use(free_function);
		copy(free_lv);
}