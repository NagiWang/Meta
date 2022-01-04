#ifndef META_MULTIPLE_INHERITANCE_TUPLE_HPP
#define META_MULTIPLE_INHERITANCE_TUPLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <concepts>
#include <type_traits>

namespace meta {

template <typename...> struct mtuple;

namespace details {

template <std::size_t Index, typename Type>
struct mtuple_value {
    using value_type = Type;

    value_type m_value;

    template <typename ElemType>
    requires std::is_nothrow_constructible_v<value_type, ElemType>
    constexpr explicit mtuple_value(ElemType&& value) noexcept
        : m_value(std::forward<ElemType>(value)) {}

    constexpr ~mtuple_value() noexcept = default;
};

template <typename, typename...>
struct mtuple_impl;

template <std::size_t... Indices, typename... Types>
struct mtuple_impl<std::index_sequence<Indices...>, Types...> : public mtuple_value<Indices, Types>... {
    // clang-format off
    template <typename... Values>
    constexpr explicit mtuple_impl(Values&&... values) noexcept
        :  mtuple_value<Indices, Types>(std::forward<Values>(values))... {}
    // clang-format on
    constexpr ~mtuple_impl() noexcept = default;
};

template <std::size_t Index, typename... Types>
struct mtuple_element;

template <typename Head, typename... Tail>
struct mtuple_element<0, Head, Tail...> {
    using type = Head;
};
// clang-format off
template <std::size_t Index, typename Head, typename... Tail>
requires (Index != 0)
struct mtuple_element<Index, Head, Tail...> {
    using type = typename mtuple_element<Index - 1, Tail...>::type;
};

template <std::size_t Index, typename... Types>
requires (Index < sizeof...(Types))
using mtuple_element_t = typename mtuple_element<Index, Types...>::type;

template <std::size_t Index, typename ...Types>
requires (Index < sizeof...(Types))
struct mtuple_helper {
	using type = mtuple_value<Index, mtuple_element_t<Index, Types...>>;
};
// clang-format on

template <std::size_t Index, typename... Types>
using mtuple_helper_t = typename mtuple_helper<Index, Types...>::type;

} // namespace details

template <typename... Types>
struct mtuple : private details::mtuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...> {
    using base_type = details::mtuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...>;

    // clang-format off
    template <typename... Values>
    requires (sizeof...(Values) == sizeof...(Types))
    constexpr explicit mtuple(Values&&... values) noexcept
        : base_type(std::forward<Values>(values)...) {}
    // clang-format on
    constexpr ~mtuple() noexcept = default;

    template <std::size_t Index, typename... ElemTypes>
    friend constexpr decltype(auto) get(mtuple<ElemTypes...>& tuple) noexcept;

    template <std::size_t Index, typename... ElemTypes>
    friend constexpr decltype(auto) get(const mtuple<ElemTypes...>& tuple) noexcept;

    template <std::size_t Index, typename... ElemTypes>
    friend constexpr decltype(auto) get(mtuple<ElemTypes...>&& tuple) noexcept;
};

template <std::size_t Index, typename... ElemTypes>
constexpr decltype(auto) get(mtuple<ElemTypes...>& tuple) noexcept {
    using base_type  = details::mtuple_helper_t<Index, ElemTypes...>;
    using value_type = details::mtuple_element_t<Index, ElemTypes...>;
    return (static_cast<base_type&>(tuple).m_value);
}

template <std::size_t Index, typename... ElemTypes>
constexpr decltype(auto) get(const mtuple<ElemTypes...>& tuple) noexcept {
    using base_type  = details::mtuple_helper_t<Index, ElemTypes...>;
    using value_type = details::mtuple_element_t<Index, ElemTypes...>;
    return (static_cast<const base_type&>(tuple).m_value);
}

template <std::size_t Index, typename... ElemTypes>
constexpr decltype(auto) get(mtuple<ElemTypes...>&& tuple) noexcept {
    using base_type  = details::mtuple_helper_t<Index, ElemTypes...>;
    using value_type = details::mtuple_element_t<Index, ElemTypes...>;
    return std::forward<value_type>(static_cast<base_type&&>(tuple).m_value);
}

} // namespace meta

namespace std {

template <typename... Types>
struct tuple_size<meta::mtuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <std::size_t Index, typename... Types>
struct tuple_element<Index, meta::mtuple<Types...>> {
    using type = meta::details::mtuple_element_t<Index, Types...>;
};

using meta::get;

} // namespace std

#endif //META_MULTIPLE_INHERITANCE_TUPLE_HPP
