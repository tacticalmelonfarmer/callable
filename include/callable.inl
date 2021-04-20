#pragma once

#define SFINAE_CHECK                                                                                                   \
  {                                                                                                                    \
    return {};                                                                                                         \
  }

namespace tmf {

inline namespace detail {
namespace sfinae {

template<typename T>
struct generic_tag
{};

template<typename T, typename ReturnT, typename... ArgTs>
struct generic_member_function
{
  using type0 = ReturnT (T::*)(ArgTs...);
  using type1 = ReturnT (T::*)(ArgTs...) const;
  using type2 = ReturnT (T::*)(ArgTs...) volatile;
  using type3 = ReturnT (T::*)(ArgTs...) const volatile;
  using type4 = ReturnT (T::*)(ArgTs...) &;
  using type5 = ReturnT (T::*)(ArgTs...) const&;
  using type6 = ReturnT (T::*)(ArgTs...) volatile&;
  using type7 = ReturnT (T::*)(ArgTs...) const volatile&;
  using type8 = ReturnT (T::*)(ArgTs...) &&;
  using type9 = ReturnT (T::*)(ArgTs...) const&&;
  using type10 = ReturnT (T::*)(ArgTs...) volatile&&;
  using type11 = ReturnT (T::*)(ArgTs...) const volatile&&;
  // noexcept variants
  using type12 = ReturnT (T::*)(ArgTs...) noexcept;
  using type13 = ReturnT (T::*)(ArgTs...) const noexcept;
  using type14 = ReturnT (T::*)(ArgTs...) volatile noexcept;
  using type15 = ReturnT (T::*)(ArgTs...) const volatile noexcept;
  using type16 = ReturnT (T::*)(ArgTs...) & noexcept;
  using type17 = ReturnT (T::*)(ArgTs...) const& noexcept;
  using type18 = ReturnT (T::*)(ArgTs...) volatile& noexcept;
  using type19 = ReturnT (T::*)(ArgTs...) const volatile& noexcept;
  using type20 = ReturnT (T::*)(ArgTs...) && noexcept;
  using type21 = ReturnT (T::*)(ArgTs...) const&& noexcept;
  using type22 = ReturnT (T::*)(ArgTs...) volatile&& noexcept;
  using type23 = ReturnT (T::*)(ArgTs...) const volatile&& noexcept;
  static constexpr type0 check(type0) SFINAE_CHECK;
  static constexpr type1 check(type1) SFINAE_CHECK;
  static constexpr type2 check(type2) SFINAE_CHECK;
  static constexpr type3 check(type3) SFINAE_CHECK;
  static constexpr type4 check(type4) SFINAE_CHECK;
  static constexpr type5 check(type5) SFINAE_CHECK;
  static constexpr type6 check(type6) SFINAE_CHECK;
  static constexpr type7 check(type7) SFINAE_CHECK;
  static constexpr type8 check(type8) SFINAE_CHECK;
  static constexpr type9 check(type9) SFINAE_CHECK;
  static constexpr type10 check(type10) SFINAE_CHECK;
  static constexpr type11 check(type11) SFINAE_CHECK;
  static constexpr type12 check(type12) SFINAE_CHECK;
  // noexcept variants
  static constexpr type13 check(type13) SFINAE_CHECK;
  static constexpr type14 check(type14) SFINAE_CHECK;
  static constexpr type15 check(type15) SFINAE_CHECK;
  static constexpr type16 check(type16) SFINAE_CHECK;
  static constexpr type17 check(type17) SFINAE_CHECK;
  static constexpr type18 check(type18) SFINAE_CHECK;
  static constexpr type19 check(type19) SFINAE_CHECK;
  static constexpr type20 check(type20) SFINAE_CHECK;
  static constexpr type21 check(type21) SFINAE_CHECK;
  static constexpr type22 check(type22) SFINAE_CHECK;
  static constexpr type23 check(type23) SFINAE_CHECK;
};

template<typename T>
decltype(std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>) is_function_pointer;

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

// this guides deduction for generic lambdas
template<typename T>
struct deduction_guide<generic_tag<T>>
{
  using type = generic_tag<T>;
};

} // namespace sfinae
} // namespace detail

template<typename u, typename T>
callable(u, T)->callable<typename sfinae::deduction_guide<T>::type, default_callable_capacity>;

template<typename T>
callable(T)->callable<typename sfinae::deduction_guide<T>::type, default_callable_capacity>;

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT&& object, MemPtrT member) noexcept
  : m_empty(false)
{
  using class_type = std::remove_reference_t<ClassT>;
  using member_function_ptr_t = decltype(sfinae::generic_member_function<class_type, ReturnT, ArgTs...>::check(member));
  using concrete_type = member_function<ClassT, member_function_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(std::forward<ClassT>(object), member);
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT&& object) noexcept
  : m_empty(false)
{
  using class_type = std::remove_reference_t<ClassT>;
  using call_operator_ptr_t =
    decltype(sfinae::generic_member_function<class_type, ReturnT, ArgTs...>::check(&class_type::operator()));
  using concrete_type = member_function<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(std::forward<ClassT>(object), &class_type::operator());
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT* object, MemPtrT member) noexcept
  : m_empty(false)
{
  using class_type = std::remove_pointer_t<ClassT>;
  using member_function_ptr_t = decltype(sfinae::generic_member_function<class_type, ReturnT, ArgTs...>::check(member));
  using concrete_type = member_function_raw_pointer<ClassT, member_function_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(object, member);
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT* object) noexcept
  : m_empty(false)
{
  using class_type = std::remove_pointer_t<ClassT>;
  using call_operator_ptr_t =
    decltype(sfinae::generic_member_function<class_type, ReturnT, ArgTs...>::check(&class_type::operator()));
  using concrete_type = member_function_raw_pointer<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(object, &class_type::operator());
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(std::shared_ptr<ClassT>& object, MemPtrT member) noexcept
  : m_empty(false)
{
  using member_function_ptr_t = decltype(sfinae::generic_member_function<ClassT, ReturnT, ArgTs...>::check(member));
  using concrete_type = member_function_smart_pointer<ClassT, member_function_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(object, member);
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(std::shared_ptr<ClassT>& object) noexcept
  : m_empty(false)
{
  using call_operator_ptr_t =
    decltype(sfinae::generic_member_function<ClassT, ReturnT, ArgTs...>::check(&ClassT::operator()));
  using concrete_type = member_function_smart_pointer<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(object, &ClassT::operator());
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(std::shared_ptr<ClassT>&& object, MemPtrT member) noexcept
  : m_empty(false)
{
  using member_function_ptr_t = decltype(sfinae::generic_member_function<ClassT, ReturnT, ArgTs...>::check(member));
  using concrete_type = member_function_smart_pointer<ClassT, member_function_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(std::move(object), member);
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(std::shared_ptr<ClassT>&& object) noexcept
  : m_empty(false)
{
  using call_operator_ptr_t =
    decltype(sfinae::generic_member_function<ClassT, ReturnT, ArgTs...>::check(&ClassT::operator()));
  using concrete_type = member_function_smart_pointer<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(std::move(object), &ClassT::operator());
  m_deleter = [](auto base) {
    auto concrete = static_cast<const concrete_type*>(base);
    concrete->~concrete_type();
  };
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      auto source_object = concrete->m_object;
      auto source_member = concrete->m_member;
      return (source_object.get()->*source_member)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(function_type* function_pointer) noexcept
  : m_empty(false)
{
  using concrete_type = free_function<ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  new (access()) concrete_type(function_pointer);
  m_deleter = nullptr;
  m_caller = [](bool is_const, const callable_base<ReturnT, ArgTs...>* base, ArgTs... arguments) {
    if (is_const) {
      auto concrete = static_cast<const concrete_type*>(base);
      return (*concrete->m_function_ptr)(static_cast<ArgTs>(arguments)...);
    } else {
      auto concrete = const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
      return (*concrete->m_function_ptr)(static_cast<ArgTs>(arguments)...);
    }
  };
  m_copier = [](auto& base, const auto& other_base) {
    new (&base) concrete_type(static_cast<const concrete_type&>(other_base));
  };
  m_mover = [](auto& base, auto&& other_base) { new (&base) concrete_type(static_cast<concrete_type&&>(other_base)); };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable() noexcept
  : m_empty(true)
{
  m_deleter = nullptr;
  m_caller = nullptr;
  m_copier = nullptr;
  m_mover = nullptr;
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(const this_type& other) noexcept
{
  destroy();
  if (other.m_empty) {
  } else {
    (*other.m_copier)(*access(), *other.access());
    m_deleter = other.m_deleter;
    m_caller = other.m_caller;
    m_copier = other.m_copier;
    m_mover = other.m_mover;
    m_empty = false;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(this_type&& other) noexcept
{
  destroy();
  if (other.m_empty) {
  } else {
    (*other.m_mover)(*access(), std::move(*other.access()));
    m_deleter = other.m_deleter;
    m_caller = other.m_caller;
    m_copier = other.m_copier;
    m_mover = other.m_mover;
    m_empty = false;
    other.destroy();
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>&
callable<ReturnT(ArgTs...), Capacity>::operator=(this_type& rhs) noexcept
{
  destroy();
  if (rhs.m_empty) {
    return *this;
  } else {
    (*rhs.m_copier)(*access(), *rhs.access());
    m_deleter = rhs.m_deleter;
    m_caller = rhs.m_caller;
    m_copier = rhs.m_copier;
    m_mover = rhs.m_mover;
    m_empty = false;
    return *this;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>&
callable<ReturnT(ArgTs...), Capacity>::operator=(const this_type& rhs) noexcept
{
  destroy();
  if (rhs.m_empty) {
    return *this;
  } else {
    (*rhs.m_copier)(*access(), *rhs.access());
    m_deleter = rhs.m_deleter;
    m_caller = rhs.m_caller;
    m_copier = rhs.m_copier;
    m_mover = rhs.m_mover;
    m_empty = false;
    return *this;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>&
callable<ReturnT(ArgTs...), Capacity>::operator=(this_type&& rhs) noexcept
{
  destroy();
  if (rhs.m_empty) {
    return *this;
  } else {
    (*rhs.m_mover)(*access(), std::move(*rhs.access()));
    m_deleter = rhs.m_deleter;
    m_caller = rhs.m_caller;
    m_copier = rhs.m_copier;
    m_mover = rhs.m_mover;
    m_empty = false;
    rhs.destroy();
    return *this;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
ReturnT
callable<ReturnT(ArgTs...), Capacity>::operator()(ArgTs... arguments)
{
  if (empty()) {
    throw callable_exception{ "attempted to call an empty callable." };
  }
  return (*m_caller)(false, access(), static_cast<ArgTs>(arguments)...);
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
ReturnT
callable<ReturnT(ArgTs...), Capacity>::operator()(ArgTs... arguments) const
{
  if (empty()) {
    throw callable_exception{ "attempted to call an empty callable." };
  }
  return (*m_caller)(true, access(), static_cast<ArgTs>(arguments)...);
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
bool
callable<ReturnT(ArgTs...), Capacity>::empty() const
{
  return m_empty;
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::~callable()
{
  destroy();
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable_base<ReturnT, ArgTs...>*
callable<ReturnT(ArgTs...), Capacity>::access()
{
  return std::launder(reinterpret_cast<callable_base<ReturnT, ArgTs...>*>(&m_storage));
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
const callable_base<ReturnT, ArgTs...>*
callable<ReturnT(ArgTs...), Capacity>::access() const
{
  return std::launder(reinterpret_cast<const callable_base<ReturnT, ArgTs...>*>(&m_storage));
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
void
callable<ReturnT(ArgTs...), Capacity>::destroy()
{
  {
    if (m_empty == false && m_deleter != nullptr) {
      (*m_deleter)(access());
      m_empty = true;
    }
  }
}
}