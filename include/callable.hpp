#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#define CALLABLE_ERROR                                                         \
  "`callable::callable` cannot hold a callable this large! Increasing "        \
  "capacity might help; Or try decoupling state from functionality if "        \
  "possible."

namespace callable {
using size_t = decltype(sizeof(0));

inline namespace detail {

template<typename T>
struct in_place_forward
{
  using type = const T&;
};

template<typename T>
struct in_place_forward<T&>
{
  using type = T&;
};

template<typename T>
struct in_place_forward<T&&>
{
  using type = T&&;
};

template<typename T>
using in_place_forward_t = typename in_place_forward<T>::type;

template<typename ReturnT, typename... ArgTs>
struct callable_base
{
  using deleter_function_pointer =
    void (*)(const callable_base<ReturnT, ArgTs...>*);
  using caller_function_pointer =
    ReturnT (*)(const callable_base<ReturnT, ArgTs...>*,
                in_place_forward_t<ArgTs>...);
};

template<typename ClassT, typename MemPtrT, typename ReturnT, typename... ArgTs>
struct member_function : callable_base<ReturnT, ArgTs...>
{
  template<typename FwdClassT,
           typename =
             ::std::enable_if_t<::std::is_member_function_pointer_v<MemPtrT> &&
                                !::std::is_const_v<ClassT>>>
  member_function(FwdClassT&& object, MemPtrT member)
    : m_object(::std::forward<FwdClassT>(object))
    , m_member(member)
  {}

  ClassT m_object;
  MemPtrT m_member;
};

template<typename ClassT, typename MemPtrT, typename ReturnT, typename... ArgTs>
struct member_function_smart_pointer : callable_base<ReturnT, ArgTs...>
{
  template<typename =
             ::std::enable_if_t<::std::is_member_function_pointer_v<MemPtrT> &&
                                !::std::is_const_v<ClassT>>>
  member_function_smart_pointer(const ::std::shared_ptr<ClassT>& object,
                                MemPtrT member)
    : m_object(object)
    , m_member(member)
  {}

  ::std::shared_ptr<ClassT> m_object;
  MemPtrT m_member;
};

template<typename ClassT, typename MemPtrT, typename ReturnT, typename... ArgTs>
struct member_function_raw_pointer : callable_base<ReturnT, ArgTs...>
{
  template<typename =
             ::std::enable_if_t<::std::is_member_function_pointer_v<MemPtrT> &&
                                !::std::is_const_v<ClassT>>>
  member_function_raw_pointer(ClassT* object, MemPtrT member)
    : m_object(object)
    , m_member(member)
  {}

  ClassT* m_object;
  MemPtrT m_member;
};

template<typename ReturnT, typename... ArgTs>
struct free_function : callable_base<ReturnT, ArgTs...>
{
  using function_pointer_type = ReturnT (*)(ArgTs...);
  free_function(function_pointer_type pointer)
    : m_function_ptr(pointer)
  {}

  function_pointer_type m_function_ptr;
};

#define SFINAE_CHECK                                                           \
  {                                                                            \
    return {};                                                                 \
  }

namespace sfinae {
// clang-format off
      template<typename T, typename ReturnT, typename... ArgTs>
      struct generic_call_operator
      {
        using type0 = ReturnT(T::*)(ArgTs...);
        using type1 = ReturnT(T::*)(ArgTs...) const;
        using type2 = ReturnT(T::*)(ArgTs...) volatile;
        using type3 = ReturnT(T::*)(ArgTs...) const volatile;
        using type4 = ReturnT(T::*)(ArgTs...) &;
        using type5 = ReturnT(T::*)(ArgTs...) const &;
        using type6 = ReturnT(T::*)(ArgTs...) volatile &;
        using type7 = ReturnT(T::*)(ArgTs...) const volatile &;
        using type8 = ReturnT(T::*)(ArgTs...) &&;
        using type9 = ReturnT(T::*)(ArgTs...) const &&;
        using type10 = ReturnT(T::*)(ArgTs...) volatile &&;
        using type11 = ReturnT(T::*)(ArgTs...) const volatile &&;
        static constexpr type0 check(type0) SFINAE_CHECK
        static constexpr type1 check(type1) SFINAE_CHECK
        static constexpr type2 check(type2) SFINAE_CHECK
        static constexpr type3 check(type3) SFINAE_CHECK
        static constexpr type4 check(type4) SFINAE_CHECK
        static constexpr type5 check(type5) SFINAE_CHECK
        static constexpr type6 check(type6) SFINAE_CHECK
        static constexpr type7 check(type7) SFINAE_CHECK
        static constexpr type8 check(type8) SFINAE_CHECK
        static constexpr type9 check(type9) SFINAE_CHECK
        static constexpr type10 check(type10) SFINAE_CHECK
        static constexpr type11 check(type11) SFINAE_CHECK
        using type = decltype(check(&T::operator()));
      };

      template<typename T>
      decltype(::std::is_pointer_v<T>&& ::std::is_function_v<::std::remove_pointer_t<T>>) is_function_pointer;
// clang-format on

// this initializes deduction_guide for lambdas or functors
template<typename T>
struct deduction_guide
{
  using type = typename deduction_guide<decltype(&T::operator())>::type;
};

// this guides deduction for free/static-member function pointers
template<typename ReturnT, typename... ArgTs>
struct deduction_guide<ReturnT (*)(ArgTs...)>
{
  using type = ReturnT(ArgTs...);
};

// this guides deduction for functors or mutable lambdas
template<typename ClassT, typename ReturnT, typename... ArgTs>
struct deduction_guide<ReturnT (ClassT::*)(ArgTs...)>
{
  using type = ReturnT(ArgTs...);
};

// this guides deduction for lambdas or const functors
template<typename ClassT, typename ReturnT, typename... ArgTs>
struct deduction_guide<ReturnT (ClassT::*)(ArgTs...) const>
{
  using type = ReturnT(ArgTs...);
};
} // namespace sfinae
} // namespace detail

constexpr auto default_callable_capacity = sizeof(std::uintptr_t) * 2;

struct empty_callable
{};

template<typename, size_t = default_callable_capacity>
struct callable;

template<typename ReturnT, typename... ArgTs, size_t Capacity>
struct callable<ReturnT(ArgTs...), Capacity>
{
  using function_type = ReturnT(ArgTs...);
  using this_type = callable<ReturnT(ArgTs...), Capacity>;

  // copies/moves an object and holds a pointer to non-static member function of
  // the held object
  template<typename ClassT, typename MemPtrT>
  callable(ClassT&& object, MemPtrT member);

  // copies/moves an object and points to it's call operator
  // `ClassT::operator()`
  template<typename ClassT>
  callable(ClassT&& object);

  // points to an object and holds a pointer to non-static member function of
  // the held object
  template<typename ClassT, typename MemPtrT>
  callable(ClassT* object, MemPtrT member);

  // points to an object and points to it's call operator `ClassT::operator()`
  template<typename ClassT>
  callable(ClassT* object);

  // points to an object and holds a pointer to non-static member function of
  // the held object
  template<typename ClassT, typename MemPtrT>
  callable(const ::std::shared_ptr<ClassT>& object, MemPtrT member);

  // points to an object and points to it's call operator `ClassT::operator()`
  template<typename ClassT>
  callable(const ::std::shared_ptr<ClassT>& object);

  // points to a callable using a pointer to function
  callable(function_type* function_pointer);

  // default initialize to be an empty function<...>
  callable();

  // copy construct
  callable(const this_type& other);

  // move construct
  callable(this_type&& other) noexcept;

  // copy assignment
  this_type& operator=(const this_type& rhs);

  // move assignment
  this_type& operator=(this_type&& rhs) noexcept;

  // call the stored function
  template<typename... FwdArgTs>
  ReturnT operator()(FwdArgTs&&... arguments);

  template<typename... FwdArgTs>
  ReturnT operator()(FwdArgTs&&... arguments) const;

  ~callable();

private:
  // alias the storage address as a polymorphic pointer to our base class
  constexpr auto access();

  constexpr auto access() const;

  // check if empty or trivially destructible, if not then call the destructor
  // for the type-erased object
  void destroy();

  typename callable_base<ReturnT, ArgTs...>::deleter_function_pointer m_deleter;
  typename callable_base<ReturnT, ArgTs...>::caller_function_pointer m_caller;

  bool m_empty;

  ::std::aligned_storage_t<Capacity, alignof(::std::max_align_t)> m_storage;
};

template<typename u, typename T>
callable(u, T)
  ->callable<typename sfinae::deduction_guide<T>::type,
             default_callable_capacity>;

template<typename T>
callable(T)
  ->callable<typename sfinae::deduction_guide<T>::type,
             default_callable_capacity>;
}

#include "callable.implement.hpp"