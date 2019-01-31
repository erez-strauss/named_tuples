//
// Author: Erez Strauss <erez@erezstrauss.com>
// Date: 1/27/19.
//

#pragma once

#ifndef NAMED_TUPLE_H
#define NAMED_TUPLE_H


#include <iostream>
#include <tuple>

namespace {

template<class... T>
static inline std::ostream &operator<<(std::ostream &os, const std::tuple<T...> &_tup);

template<class TupType, size_t... I>
static inline std::ostream &tuple_print(std::ostream &os, const TupType &_tup, std::index_sequence<I...>)
{
    os << "(";
    (...,(os << (I == 0 ? "" : ", ") << (std::get<I>(_tup))));
    os << ")";
    return os;
}

template<class... T>
static inline std::ostream &operator<<(std::ostream &os, const std::tuple<T...> &_tup)
{
    return tuple_print(os, _tup, std::make_index_sequence<sizeof...(T)>());
}

}

// -----------------------------------------

//namespace named_tuple {

// named_value - value type VT, name type NT - holds the value type in a tuple while knowing the named type as type property

template<typename VT, typename NT>
struct named_value {
    static inline const char* _typename { NT::_name };
    using type = VT;
    using namedtype = typename NT::type;

  named_value(const VT& v) : _data (v) {}
  named_value(VT&& v) : _data (std::move(v)) {}
  named_value() = default;
  named_value(const named_value&) = default;
  named_value(named_value&&) = default;
  named_value& operator=(const named_value&) = default;
  named_value& operator=(named_value&&) = default;

  template<typename ... Args> explicit named_value(Args ... args) : _data (std::forward<Args...>(args)...) {}
  
    operator VT&() { return _data; }
    VT& get() { return _data; }
    const VT& get() const { return _data; }

    VT& operator=(const VT& value) { _data = value; return _data; }
    VT& operator=(VT&& value) { _data = std::move(value); return _data; }
private:
  VT _data;
};

// named type - creates a type for a given string, field a tuple member.

template<char ... C>
struct named_type {
    static const inline char _name[sizeof...(C)+1] { C ..., '\0' };
    using type = named_type;

    template<size_t N>
    auto operator=(const char (& s)[N]) {
        return named_value<std::string, named_type> { s };
    }

    template<typename T>
    auto operator=(const T& v) const { return named_value<T, named_type> ( v ) ; }

    template<typename T>
    auto operator=(T& v) const { return named_value<T, named_type> ( std::move(v) ) ; }

};

// named tuple - hold named values, inherits from std::tuple
//  TS the named value types

template<typename ... TS>
class named_tuple : public std::tuple<TS...> {
public:

    using type = named_tuple<TS...>;
    using simple_tuple_t = typename std::tuple<typename TS::type ...>;

    template <typename>
    constexpr static int named_type_count(int)
    {
        return 0;
    }

    template <typename T, typename Head, typename... Tail>
    constexpr static int named_type_count(int current_index = 0)
    {
      return (std::is_same<T, typename Head::namedtype>::value ? 1 : 0)
	+ named_type_count<T, Tail...>(current_index + 1);
    }
  
    template <typename T>
    constexpr static void verify_named_type_count() {
        static_assert(named_type_count<T, TS...>() == 1, "named type field, appears more than once in named tuple");
    }

    named_tuple() noexcept : std::tuple<TS...>() {
      (...,verify_named_type_count<typename TS::namedtype>());
    }

    named_tuple(TS&& ... ts) noexcept : std::tuple<TS...>(std::forward<TS>(ts)...) {
      (...,verify_named_type_count<typename TS::namedtype>());
    }

    template <typename>
    constexpr static int named_type_find(int)
    {
        return -1;
    }

    template <typename T, typename Head, typename... Tail>
    constexpr static int named_type_find(int current_index = 0)
    {
        return std::is_same<T, typename Head::namedtype>::value
               ? current_index
               : named_type_find<T, Tail...>(current_index + 1);
    }
  
    template <typename T>
    constexpr static int get_index() 
    {
      static_assert(named_type_count<T, TS...>() == 1, "named type can appear only once in a named tuple");
        return named_type_find<T, TS...>();
    }

    template <typename T>
    auto& get()
    {
        return std::get<(get_index<T>())> (*this);
    }

    template <typename T>
    auto& operator[](T t) {
        return get<decltype(t)>();
    }

  template<typename F>
  auto& foreach(F&& f) {
    (...,f(get<typename TS::namedtype>()));
    return *this;
  }

  // Return the std::tuple with values without named_types information

  inline std::tuple<typename TS::type ... >& simple_tuple() {
    return reinterpret_cast< std::tuple< typename TS::type ... >& >  (*this);
  }

};

template<typename ... TT, typename ... ST>
auto& operator << (named_tuple<TT...>&trg, named_tuple<ST...>&src) {
  (...,(trg[typename ST::namedtype{}] = src[typename ST::namedtype{}]));
  return trg;
}

template<typename ... TT, typename ... NV>
auto& operator << (named_tuple<TT...>&trg, const NV& ... src) {
  (..., (trg[typename NV::namedtype{}] = src.get()));
  return trg;
}

template<class CharT, CharT... CS>
inline constexpr auto operator "" _ ()
{
    return named_type< CS ... > {};
}

//} // namespace named


template<typename VT, typename NT>
inline std::ostream& operator<<(std::ostream& os, const named_value<VT, NT>& nv) {
  os << nv._typename << ": " << nv.get();
    return os;
}

template<char ... C>
std::ostream& operator << (std::ostream& os, const named_type<C...>& nt) {
    os << "Namedtype: " << nt._name ;
    return os;
}


#endif //NAMED_TUPLE_H
