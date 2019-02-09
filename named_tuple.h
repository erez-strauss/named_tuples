//
// Author: Erez Strauss <erez@erezstrauss.com>
// Date: 1/27/19.
//

#pragma once

#include <iostream>
#include <tuple>

namespace nvtuple_ns {
template<typename VT, typename NT>
struct named_value;
}

template<typename VT, typename NT>
inline std::ostream& operator<<(std::ostream& os,
                                const nvtuple_ns::named_value<VT, NT>& nv);

namespace nvtuple_ns {  // named value tuple

template<class TT, size_t... I>
static inline std::ostream& tuple_print(std::ostream& os, const TT& _tup,
                                        std::index_sequence<I...>) {
    os << "(";
    (..., (os << (I == 0 ? "" : ", ") << (std::get<I>(_tup))));
    os << ")";
    return os;
}

}  // namespace nvtuple_ns

template<class... T>
static inline std::ostream& operator<<(std::ostream& os,
                                       const std::tuple<T...>& _tup) {
    return nvtuple_ns::tuple_print(os, _tup,
                                   std::make_index_sequence<sizeof...(T)>());
}

// named_value - value type VT, name type NT - holds the value type in a tuple
// while knowing the named type as type property

namespace nvtuple_ns {

template<typename VT, typename NT>
struct named_value {
    static inline const char* _typename{NT::_name};
    using type = VT;
    using namedtype = typename NT::type;

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
    operator const VT&() const { return _data; }
    constexpr VT& get() { return _data; }
    constexpr const VT& get() const { return _data; }

    constexpr VT& operator=(const VT& value) {
        _data = value;
        return _data;
    }
    constexpr VT& operator=(VT&& value) {
        _data = std::move(value);
        return _data;
    }

   private:
    VT _data;
};

// named type - creates a type for a given string, field a tuple member.

template<char... C>
struct named_type {
    static const inline char _name[sizeof...(C) + 1]{C..., '\0'};
    using type = named_type;

    template<size_t N>
    constexpr auto operator=(const char (&s)[N]) {
        return named_value<std::string, named_type>{s};
    }

    template<typename T>
    constexpr auto operator=(const T& v) const {
        return named_value<T, named_type>(v);
    }

    template<typename T>
    constexpr auto operator=(T& v) const {
        return named_value<T, named_type>(std::move(v));
    }
};

// named tuple - hold named values, inherits from std::tuple
//  TS the named value types

template<typename... TS>
class named_tuple : public std::tuple<TS...> {
   public:
    using type = named_tuple<TS...>;
    using simple_tuple_t = typename std::tuple<typename TS::type...>;

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

    //    constexpr named_tuple() noexcept : std::tuple<TS...>() {
    //      (..., verify_named_type_count<typename TS::namedtype>());
    //  }

    constexpr named_tuple(const named_tuple&) = default;
    constexpr named_tuple(named_tuple&&) noexcept = default;

    constexpr named_tuple& operator=(const named_tuple&) = default;
    constexpr named_tuple& operator=(named_tuple&) = default;
    constexpr named_tuple& operator=(named_tuple&&) noexcept = default;

    constexpr named_tuple(TS&&... ts) noexcept
        : std::tuple<TS...>(std::forward<TS>(ts)...) {
        (..., verify_named_type_count<typename TS::namedtype>());
    }

    // constexpr named_tuple(const TS&... ts) noexcept
    //    : std::tuple<TS...>(std::forward<TS>(ts)...) {}

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
    decltype(auto) get() noexcept {
        return std::get<(get_index<T>())>(*this);
    }

    template<typename T>
    auto& operator[](T t) noexcept {
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

}  // namespace nvtuple_ns

template<typename... TT, typename... ST>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg,
                 nvtuple_ns::named_tuple<ST...>& src) {
    (..., (trg[typename ST::namedtype{}] = src[typename ST::namedtype{}]));
    return trg;
}

template<typename... TT, typename... NV>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg, const NV&... src) {
    (..., (trg[typename NV::namedtype{}] = src.get()));
    return trg;
}

template<typename... TT, typename... NV>
auto& operator<<(nvtuple_ns::named_tuple<TT...>& trg, NV&&... mo) {
    (..., (trg[typename NV::namedtype{}] = std::move(mo)));
    return trg;
}

template<class CharT, CharT... CS>
inline constexpr auto operator"" _() {
    return nvtuple_ns::named_type<CS...>{};
}

template<typename VT, typename NT>
inline std::ostream& operator<<(std::ostream& os,
                                const nvtuple_ns::named_value<VT, NT>& nv) {
    os << nv._typename << ": " << nv.get();
    return os;
}

template<char... C>
std::ostream& operator<<(std::ostream& os,
                         const nvtuple_ns::named_type<C...>& nt) {
    os << "Namedtype: " << nt._name;
    return os;
}
