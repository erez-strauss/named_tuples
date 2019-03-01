//
// Author: Erez Strauss <erez@erezstrauss.com>
// Date: 1/27/19.
//

#pragma once

#include <array>
#include <iostream>
#include <string_view>
#include <tuple>

namespace nvtuple_ns {
template<typename VT, typename NT>
class named_value;
}

// named_value - value type VT, name type NT - holds the value type in a tuple
// while knowing the named type as type property

namespace nvtuple_ns {

template<typename FN>  // decltype("abc"_)
class named_value_type {
   public:
    constexpr const static bool value{false};
    using type = void;
};

template<typename VT, typename NT>
class named_value {
   public:
    static constexpr inline const char* get_value_name() { return NT::_name; }
    using type = VT;
    using namedtype = typename NT::type;
    constexpr static inline bool is_a_named_value() { return true; }
    constexpr named_value() = default;

    constexpr named_value(const VT& v) : _data(v) {}
    constexpr named_value(VT&& v) : _data(std::move(v)) {}

    constexpr named_value(const named_value&) = default;
    constexpr named_value(named_value&&) noexcept = default;

    constexpr named_value& operator=(const named_value&) = default;
    constexpr named_value& operator=(named_value&&) = default;

    template<typename... Args>
    explicit named_value(Args... args)
        : _data(std::forward<Args...>(args)...) {}

    operator decltype(auto)() { return _data; }
    operator decltype(auto)() const { return _data; }
    // operator const VT&() const { return _data; }
    constexpr VT& get() { return _data; }
    constexpr const VT& get() const { return _data; }

    constexpr named_value& operator=(const VT& value) {
        _data = value;
        return *this;
    }
    constexpr named_value& operator=(VT&& value) {
        _data = std::move(value);
        return *this;
    }

   private:
    VT _data;
};

// named type - creates a type for a given string, field a tuple member.

template<char... C>
class named_type {
   public:
    static constexpr const inline char _name[sizeof...(C) + 1]{C..., '\0'};
    static constexpr const inline std::string_view _name_sv{_name,
                                                            sizeof...(C)};
    static constexpr inline bool is_a_named_type() { return true; }
    using type = named_type;

    constexpr auto str() const { return _name_sv; }
};

template<char... Ca, char... Cb>
constexpr decltype(auto) operator+(const named_type<Ca...>&,
                                   const named_type<Cb...>&) {
    return named_type<Ca..., Cb...>{};
}

// named tuple - hold named values, inherits from std::tuple
//  TS the named value types

template<typename... TS>
class named_tuple : public std::tuple<TS...> {
   public:
    using type = named_tuple<TS...>;
    using simple_tuple_t = typename std::tuple<typename TS::type...>;

    constexpr auto names() {
        const std::array<const char*, sizeof...(TS)> nms{
            (*this)[typename TS::namedtype{}].get_value_name()...};
        return nms;
    }
    template<typename>
    constexpr static int named_type_count(int) {
        return 0;
    }

    template<typename T, typename Head, typename... Tail>
    constexpr static int named_type_count(int current_index = 0) {
        return (std::is_same<T, typename Head::namedtype>::value ? 1 : 0) +
               named_type_count<T, Tail...>(current_index + 1);
    }

    template<typename T>
    constexpr static void verify_named_type_count() {
        static_assert(
            named_type_count<T, TS...>() == 1,
            "named type field, appears more than once in named tuple");
    }

    constexpr named_tuple(const named_tuple&) = default;
    constexpr named_tuple(named_tuple&&) noexcept = default;

    constexpr named_tuple& operator=(const named_tuple&) = default;
    constexpr named_tuple& operator=(named_tuple&) = default;
    constexpr named_tuple& operator=(named_tuple&&) noexcept = default;

    constexpr named_tuple(TS&&... ts) noexcept
        : std::tuple<TS...>(std::forward<TS>(ts)...) {
        (..., verify_named_type_count<typename TS::namedtype>());
    }

    template<typename... CT>
    named_tuple(named_tuple<CT...>& ct) noexcept {
        (..., (get<typename CT::namedtype>() = ct[typename CT::namedtype{}]));
    }

    template<typename... CT>
    named_tuple(const CT&... cvt) noexcept {
        (..., (get<typename CT::namedtype>() = cvt.get()));
    }

    template<typename>
    constexpr static int named_type_find(int) noexcept {
        return -1;
    }

    template<typename T, typename Head, typename... Tail>
    constexpr static int named_type_find(int current_index = 0) noexcept {
        return std::is_same<T, typename Head::namedtype>::value
                   ? current_index
                   : named_type_find<T, Tail...>(current_index + 1);
    }

    template<typename T>
    constexpr static int get_index() noexcept {
        static_assert(named_type_count<T, TS...>() == 1,
                      "named type can appear only once in a named tuple");
        return named_type_find<T, TS...>();
    }

    template<typename T>
    constexpr decltype(auto) get() noexcept {
        return std::get<(get_index<T>())>(*this);
    }

    template<typename T>
    constexpr decltype(auto) get() const noexcept {
        return std::get<(get_index<T>())>(*this);
    }

    template<typename T>
    constexpr auto& operator[](T t) noexcept {
        return get<decltype(t)>();
    }

    template<typename T>
    constexpr auto& operator[](T t) const noexcept {
        return get<decltype(t)>();
    }

    template<typename F>
    auto& foreach (F&& f) noexcept {
        (..., f(get<typename TS::namedtype>()));
        return *this;
    }

    // Return the std::tuple with values without named_types information

    inline std::tuple<typename TS::type...>& simple_tuple() {
        return reinterpret_cast<std::tuple<typename TS::type...>&>(*this);
    }
};

template<typename... TS>
using tuple = named_tuple<TS...>;

template<size_t N, char... C,
         typename RT = typename std::conditional<
             nvtuple_ns::named_value_type<nvtuple_ns::named_type<C...>>::value,
             typename nvtuple_ns::named_value_type<
                 nvtuple_ns::named_type<C...>>::type,
             std::string>::type>
constexpr decltype(auto) operator,(const nvtuple_ns::named_type<C...> fn,
                                   const char (&s)[N]) {
    return nvtuple_ns::named_value<RT, const decltype(fn)>{s};
}

template<typename T, char... C,
         typename RT = typename std::conditional<
             nvtuple_ns::named_value_type<nvtuple_ns::named_type<C...>>::value,
             typename nvtuple_ns::named_value_type<
                 nvtuple_ns::named_type<C...>>::type,
             T>::type>
constexpr decltype(auto) operator,(const nvtuple_ns::named_type<C...> fn,
                                   const T& v) {
    return nvtuple_ns::named_value<RT, const decltype(fn)>{v};
}

}  // namespace nvtuple_ns

template<typename... TT, typename... ST>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg,
                 const nvtuple_ns::named_tuple<ST...>& src) {
    (..., (trg[typename ST::namedtype{}] = src[typename ST::namedtype{}]));
    return trg;
}

template<typename... TT, typename... ST>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg,
                 nvtuple_ns::named_tuple<ST...>&& src) {
    (..., (trg[typename ST::namedtype{}] =
               std::move(src[typename ST::namedtype{}])));
    return trg;
}

template<typename... TT, typename... NV>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg, const NV&... src) {
    (..., (trg[typename NV::namedtype{}] = src.get()));
    return trg;
}

template<typename... TT, typename NV>
typename std::enable_if<NV::is_a_named_value(),
                        nvtuple_ns::named_tuple<TT...>&>::type
operator<<(nvtuple_ns::named_tuple<TT...>& trg, NV&& mo) {
    trg[typename NV::namedtype{}] = std::move(mo);
    return trg;
}

template<class CharT, CharT... CS>
inline constexpr auto operator"" _() {
    return nvtuple_ns::named_type<CS...>{};
}

template<char... C>
std::ostream& operator<<(std::ostream& os,
                         const nvtuple_ns::named_type<C...>& nt) {
    os << "named_type: " << nt.str();
    return os;
}

template<class... T>
static inline std::ostream& operator<<(std::ostream& os,
                                       const std::tuple<T...>& tup);

template<typename NT, typename VT>
inline typename std::enable_if<std::is_same<VT, std::string>::value,
                               std::ostream&>::type
operator<<(std::ostream& os,
           const typename nvtuple_ns::named_value<VT, NT>& nv) {
    os << nv.get_value_name() << ": \"" << nv.get() << '"';
    return os;
}

template<typename NT, typename VT>
inline typename std::enable_if<!std::is_same<VT, std::string>::value,
                               std::ostream&>::type
operator<<(std::ostream& os,
           const typename nvtuple_ns::named_value<VT, NT>& nv) {
    os << nv.get_value_name() << ": " << nv.get();
    return os;
}

namespace nvtuple_ns {
template<class TT, size_t... I>
static inline std::ostream& tuple_print(std::ostream& os, const TT& tup,
                                        std::index_sequence<I...>);
}

template<class... T>
static inline std::ostream& operator<<(std::ostream& os,
                                       const std::tuple<T...>& tup);

template<class... T>
static inline std::ostream& operator<<(std::ostream& os,
                                       const std::tuple<T...>& tup) {
    return nvtuple_ns::tuple_print(os, tup,
                                   std::make_index_sequence<sizeof...(T)>());
}

namespace nvtuple_ns {

template<class TT, size_t... I>
static inline std::ostream& tuple_print(std::ostream& os, const TT& tup,
                                        std::index_sequence<I...>) {
    os << "(";
    (..., ((os << (I == 0 ? "" : ", ")) << (std::get<I>(tup))));
    os << ")";
    return os;
}

}  // namespace nvtuple_ns

// typename std::enable_if<NV::value, typename NV::nvalue_type>::type

template<
    typename NV,
    typename RT = typename std::enable_if<
        NV::is_a_named_type(),
        typename nvtuple_ns::named_value<
            typename nvtuple_ns::named_value_type<NV>::type, const NV>>::type>
RT operator~(NV&&) {
    return {};
}

#define NVT_FIELD_TYPE(F, T)                                           \
    namespace nvtuple_ns {                                             \
    template<>                                                         \
    struct named_value_type<decltype(F)> {                             \
        constexpr const inline static bool value{true};                \
        using named_type = const decltype(F);                          \
        using type = T;                                                \
        using nvalue_type = nvtuple_ns::named_value<type, named_type>; \
    };                                                                 \
    }
