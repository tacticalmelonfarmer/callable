namespace callable {

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT&& object, MemPtrT member)
  : m_empty(false)
{
  using concrete_type = member_function<ClassT, MemPtrT, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access()) concrete_type(::std::forward<ClassT>(object), member);
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object.*
            (object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT&& object)
  : m_empty(false)
{
  using class_type = ::std::remove_reference_t<ClassT>;
  using call_operator_ptr_t =
    typename sfinae::generic_call_operator<class_type, ReturnT, ArgTs...>::type;
  using concrete_type =
    member_function<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access())
    concrete_type(::std::forward<ClassT>(object), &class_type::operator());
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object.*
            (object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT* object,
                                                          MemPtrT member)
  : m_empty(false)
{
  using concrete_type =
    member_function_raw_pointer<ClassT, MemPtrT, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access()) concrete_type(::std::forward<ClassT>(object), member);
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, in_place_forward_t<ArgTs>... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object->*(object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(ClassT* object)
  : m_empty(false)
{
  using class_type = ::std::remove_reference_t<ClassT>;
  using call_operator_ptr_t =
    typename sfinae::generic_call_operator<class_type, ReturnT, ArgTs...>::type;
  using concrete_type =
    member_function_raw_pointer<ClassT, call_operator_ptr_t, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access())
    concrete_type(::std::forward<ClassT>(object), &class_type::operator());
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object->*(object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT, typename MemPtrT>
callable<ReturnT(ArgTs...), Capacity>::callable(
  const ::std::shared_ptr<ClassT>& object,
                                      MemPtrT member)
  : m_empty(false)
{
  using concrete_type =
    member_function_smart_pointer<ClassT, MemPtrT, ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access()) concrete_type(::std::forward<ClassT>(object), member);
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object.get()->*(object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename ClassT>
callable<ReturnT(ArgTs...), Capacity>::callable(
  const ::std::shared_ptr<ClassT>& object)
  : m_empty(false)
{
  using call_operator_ptr_t =
    typename sfinae::generic_call_operator<ClassT, ReturnT, ArgTs...>::type;
  using concrete_type = member_function_smart_pointer<ClassT,
                                                      call_operator_ptr_t,
                                                      ReturnT,
                                                      ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access())
    concrete_type(::std::forward<ClassT>(object), &ClassT::operator());
  m_deleter = [](auto base) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    object->~concrete_type();
  };
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (object->m_object.get()->*(object->m_member))(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(
  function_type* function_pointer)
  : m_empty(false)
{
  using concrete_type = free_function<ReturnT, ArgTs...>;
  static_assert(sizeof(concrete_type) <= Capacity, CALLABLE_ERROR);
  ::new (access()) concrete_type(function_pointer);
  m_deleter = nullptr;
  m_caller = [](auto base, auto... arguments) {
    auto object =
      const_cast<concrete_type*>(static_cast<const concrete_type*>(base));
    return (*object->m_function_ptr)(arguments...);
  };
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable()
  : m_empty(true)
{}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(
  const this_type& other)
{
  destroy();
  if (other.m_empty) {
  } else {
    m_storage = other.m_storage;
    m_deleter = other.m_deleter;
    m_caller = other.m_caller;
    m_empty = false;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::callable(
  this_type&& other) noexcept
{
  destroy();
  if (other.m_empty) {
  } else {
    m_storage = ::std::move(other.m_storage);
    m_deleter = other.m_deleter;
    m_caller = other.m_caller;
    m_empty = false;
    other.destroy();
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>&
callable<ReturnT(ArgTs...), Capacity>::operator=(const this_type& rhs)
{
  destroy();
  if (rhs.m_empty) {
    return *this;
  } else {
    m_storage = rhs.m_storage;
    m_deleter = rhs.m_deleter;
    m_caller = rhs.m_caller;
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
    m_storage = ::std::move(rhs.m_storage);
    m_deleter = rhs.m_deleter;
    m_caller = rhs.m_caller;
    m_empty = false;
    rhs.destroy();
    return *this;
  }
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename... FwdArgTs>
ReturnT
callable<ReturnT(ArgTs...), Capacity>::operator()(FwdArgTs&&... arguments)
{
  if (m_empty)
    throw empty_callable{};
  else
    return (*m_caller)(access(), ::std::forward<FwdArgTs>(arguments)...);
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
template<typename... FwdArgTs>
ReturnT
callable<ReturnT(ArgTs...), Capacity>::operator()(FwdArgTs&&... arguments) const
{
  if (m_empty)
    throw empty_callable{};
  else
    return (*m_caller)(access(), ::std::forward<FwdArgTs>(arguments)...);
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
callable<ReturnT(ArgTs...), Capacity>::~callable()
{
  destroy();
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
constexpr auto
callable<ReturnT(ArgTs...), Capacity>::access()
{
  return ::std::launder(
    reinterpret_cast<callable_base<ReturnT, ArgTs...>*>(&m_storage));
}

template<typename ReturnT, typename... ArgTs, size_t Capacity>
constexpr auto
callable<ReturnT(ArgTs...), Capacity>::access() const
{
  return ::std::launder(
    reinterpret_cast<const callable_base<ReturnT, ArgTs...>*>(&m_storage));
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