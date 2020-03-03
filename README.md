# Callable
This is a tiny header-only library, everything is in namespace `tmf`.

The template class `callable` behaves like `std::function`, the difference being it is on the stack with an user-defined static capacity.
When using a binding mechanism (~~std::bind~~, lambda, etc...), the entity must store the bound arguments somehow, hence the capacity, specified in bytes.

You can initialize a `callable` with:
 - function pointer: address of free function or static member function
 - functor: class which defines `operator()`
    - pass by constant reference: 
 - functor pointer
 - functor `std::shared_ptr`
 - lambda
 - lambda pointer
 - lambda `std::shared_ptr`
 - object reference (rvalue or lvalue) + pointer-to-member-function
 - object pointer + pointer-to-member-function
 - object shared_ptr + pointer-to-member-function

## Setup
### CMake
To use this library, simply clone the repository into your project, and in your *CMakeLists.txt* do:
```cmake
add_subdirectory("path/to/callable/folder")
```
and for any targets that need to use the library, do:
```cmake
target_link_libraries(my_target libcallable)
```

### Not using CMake? No problem.
here are ways you can do it without cmake:
+ add **include/** directory to your build system for targets that need to use this library
+ just copy the headers and stick 'em where you need 'em.

## Usage
*defer to the tests for in depth usage*

for less typing you can:
```c++
#include <callable.hpp>
using tmf::callable;
```

basic instantiation of a `callable` providing function signature and defaulted capacity:
```c++
void free_fn()
{}

int main()
{
 callable<void()> basic1{ &free_fn };
 callable<void()> basic2{ free_fn };
}
```

basic instantiation of a `callable` deducing function signature and defaulted capacity:
```c++
void free_fn()
{}

int main()
{
 callable basic1{ &free_fn };
 callable basic2{ free_fn };
}
```

full instantiation of a `callable` providing function signature and providing capacity:
```c++
void free_fn()
{}

int main()
{
 callable<void(), 4> basic1{ &free_fn };
 callable<void(), 4> basic2{ free_fn };
}
```

## You can still use heap-backed entities
All `callable`s store their source on the stack, and allow you to specify a fixed size as the second template argument. However, you can provide a **raw pointer** or a `shared_ptr` instead, to avoid copying or moving objects.
