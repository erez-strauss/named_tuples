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

template<typename VT, typename NT>
struct NamedValue {
    static inline const char* _typename { NT::_name };
    using type = VT;
    using namedtype = typename NT::type;

    VT _data;
    operator VT&() { return _data; }

    VT& operator=(const VT& value) { _data = value; return _data; }
    VT& operator=(VT&& value) { _data = std::move(value); return _data; }
};

template<char ... C>
struct NamedType {
    static const inline char _name[sizeof...(C)+1] { C ..., '\0' };
    using type = NamedType;

    template<size_t N>
    auto operator=(const char (& s)[N]) {
        return NamedValue<std::string, NamedType> { s };
    }

    template<typename T>
    auto operator=(const T& v) { return NamedValue<T, NamedType> { v } ; }

};

template<typename ... TS>
class NamedTuple : public std::tuple<TS...> {
public:
    using type = NamedTuple<TS...>;
    using simple_tuple_t = typename std::tuple<typename TS::type ...>;

    NamedTuple(TS&& ... ts) noexcept : std::tuple<TS...>(std::forward<TS>(ts)...) {
        //static_assert(count<T, Types...>() == 1, "T must appear exactly once in ...Types");
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

};

template<class CharT, CharT... CS>
inline constexpr auto operator "" _ ()
{
    return NamedType< CS ... > {};
}

#if 0 // to be implemented
// Return a std::tuple with values without named_types information
template<typename ... Ts>
inline auto simple_tuple(std::tuple<NamedValue<Ts...>>& t) {
    return std::tuple<Ts...>  (std::get<0>(t));
}
#endif

//} // namespace named_tuple


template<typename VT, typename NT>
inline std::ostream& operator<<(std::ostream& os, const NamedValue<VT, NT>& nv) {
    os << nv._typename << ": " << nv._data;
    return os;
}

template<char ... C>
std::ostream& operator << (std::ostream& os, const NamedType<C...>& nt) {
    os << "Namedtype: " << nt._name ;
    return os;
}


#endif //NAMED_TUPLE_H
