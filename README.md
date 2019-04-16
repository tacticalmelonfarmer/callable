# Callable
Any object, pointer or member function `x` that can be called like a function with `operator()` can be represented by a `callable`.
Things that are callable comapatible:
 - function pointers: addresses of free functions or static member functions
 - functors: classes which define `operator()`
 - lambdas: implicit language-generated functors
 - member functions: a class object or pointer paired with a pointer-to-member-function

## Setup
### CMake
To use this library, simply clone the repository into your project, and in your *CMakeLists.txt* do:
```cmake
add_subdirectory("path/to/callable/folder")
```
and for any targets that need to use the library, do:
```cmake
target_link_libraries(my_target callable)
```

### Manual
add **include/** directory to your build config for targets that need to use this library

## Usage
To access the `tmf::callable` class, simply:
```c++
#include <callable.hpp>
using tmf::callable;
...
```
To use the class, initialize or assign with any compatible type[s].

for regular functions or static member functions:
```c++
int increment(int i) { return i + 1; }

struct math {
  static int decrement(int i) { return i - 1; }
};

 // initialize with free function pointer, and a specific function signature
callable<int(int)> specified(&increment);

 // initialize with static member function pointer, function signature is deduced
callable deduced(&math::decrement)
```
for functors and lambdas:
```c++
struct adder {
  int operator()(int a, int b) { return a + b; }
};

auto subtractor = [](int a, int b) { return a - b; };

// give an instance, without a pointer-to-member-function, to assume `operator()`
callable binary_operation{ adder{} };

// reassign to a different type, as long as call signatures are compatible
binary_operation = subtractor;
```
for member functions with access to a `this` pointer:
```c++
struct statistic {
  void accumulate(int i) { this->record += i; ++this->count; }
  int average() { return record / count; }
private:
  int count = 0;
  int record = 0;
};

// default initialize, just to show that you can assign from a braced-init-list
callable<void(int)> operation{};
statistic stats{};
op1 = { stats, &statistic::accumulate }; // :)

// of course you can do it all in one line
callable op2{ statistic{}, &statistic::average };
```
## Stack allocated
All `callable`s store their instances on the stack, and allow you to specify a fixed size as the second template argument. However, you can provide a **raw pointer** or a `shared_ptr` instead, to avoid copying or moving objects.

## Purpose
The main reason I made this library is to unify the calling interface between many entities that are supported by C++
and to make it simple, intuitive and predictable.