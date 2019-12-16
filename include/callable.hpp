#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#define CALLABLE_ERROR                                                         \
  "`tmf::callable` cannot hold a callable this large! Increasing "        \
  "capacity might help; Or try decoupling state from functionality if "        \
  "possible."

namespace tmf { // represent
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
  using copier_function_pointer =
    void (*)(callable_base<ReturnT, ArgTs...>&,
             const callable_base<ReturnT, ArgTs...>&);
  using mover_function_pointer =
    void (*)(callable_base<ReturnT, ArgTs...>&,
             callable_base<ReturnT, ArgTs...>&&);
};

template<typename T>
struct generic_tag
{};

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

} // namespace detail

static constexpr auto default_callable_capacity = sizeof(std::uintptr_t) * 4;

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
  callable(const std::shared_ptr<ClassT>& object, MemPtrT member);

  // points to an object and points to it's call operator `ClassT::operator()`
  template<typename ClassT>
  callable(const std::shared_ptr<ClassT>& object);

   // points to an object and holds a pointer to non-static member function of
  // the held object
  template<typename ClassT, typename MemPtrT>
  callable(::std::shared_ptr<ClassT>&& object, MemPtrT member);

  // points to an object and points to it's call operator `ClassT::operator()`
  template<typename ClassT>
  callable(::std::shared_ptr<ClassT>&& object);

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
  typename callable_base<ReturnT, ArgTs...>::copier_function_pointer m_copier;
  typename callable_base<ReturnT, ArgTs...>::mover_function_pointer m_mover;

  bool m_empty;

  ::std::aligned_storage_t<Capacity, alignof(::std::max_align_t)> m_storage;
};

}

#include "callable.inl"