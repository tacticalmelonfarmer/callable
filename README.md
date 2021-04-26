# Callable
This is a tiny header-only library, everything is in namespace `tmf`.

The template class `callable` has a similar interface to `std::function`, the difference being it is on the stack with a user-defined static capacity.
When using a binding mechanism (~~std::bind~~, lambda, etc...), the entity must store the bound arguments somehow, hence the capacity, specified in bytes.

You can initialize a `callable` with:
 - function pointer
    - free function 
    - static member function
 - functor: class which defines `operator()`
    - functor constant reference
    - functor mutable reference
    - functor pointer
    - functor value
    - functor `std::shared_ptr`
 - lambda
    - lambda constant reference
    - lambda mutable reference
    - lambda pointer
    - lambda value
    - lambda `std::shared_ptr`
- { object, member } pair
    - { object constant reference, member function pointer } pair
    - { object mutable reference, member function pointer } pair
    - { object pointer, member function pointer } pair
    - { object value, member function pointer } pair
    - { object `std::shared_ptr`, member function pointer } pair

## Setup
### CMake it easy
To use this library, simply clone the repo somewhere into your project, and in your *CMakeLists.txt* do:
```cmake
add_subdirectory("path/to/callable")
```

and for any targets that need to use the library, do:
```cmake
target_link_libraries(my_target callable)
```

### No CMake? No problem.
here are ways you can do it without cmake:
+ add **include/** directory to your build system for targets that need to use this library

    or
+ just copy the headers and stick 'em where you need 'em.
