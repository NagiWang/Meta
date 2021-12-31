#ifndef META_RECURSIVE_INHERITANCE_TUPLE_HPP
#define META_RECURSIVE_INHERITANCE_TUPLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <concepts>
#include <type_traits>

namespace meta {

template <typename... Types> struct rtuple;

namespace details {

template <typename... Types> struct rtuple_impl;

template <> struct rtuple_impl<> {
	constexpr rtuple_impl() noexcept  = default;
	constexpr ~rtuple_impl() noexcept = default;
};

template <typename Head, typename... Tail>
struct rtuple_impl<Head, Tail...> : public rtuple_impl<Tail...> {
	using base_type  = rtuple_impl<Tail...>;
	using value_type = Head;

	value_type m_value;

	template <typename FirstValue, typename... OtherValues>
	requires std::is_nothrow_constructible_v<value_type, FirstValue>
	constexpr explicit rtuple_impl(FirstValue&& value, OtherValues&&... others) noexcept
	    : base_type(std::forward<OtherValues>(others)...)
	    , m_value(std::forward<FirstValue>(value)) {}

	constexpr ~rtuple_impl() noexcept = default;
};

template <std::size_t, typename...>
struct rtuple_helper;

template <typename... Types>
struct rtuple_helper<0, Types...> {
	using type = rtuple_impl<Types...>;
};

// clang-format off
template <std::size_t Index, typename Head, typename... Tail>
requires(Index != 0)
struct rtuple_helper<Index, Head, Tail...> {
	using type = typename rtuple_helper<Index - 1, Tail...>::type;
};
// clang-format on

template <std::size_t Index, typename... Types>
using rtuple_helper_t = typename rtuple_helper<Index, Types...>::type;

template <std::size_t Index, typename... Types>
struct rtuple_element;

template <typename Head, typename... Tail>
struct rtuple_element<0, Head, Tail...> {
	using type = Head;
};

template <std::size_t Index, typename Head, typename... Tail>
requires(Index != 0) struct rtuple_element<Index, Head, Tail...> {
	using type = typename rtuple_element<Index - 1, Tail...>::type;
};

template <std::size_t Index, typename... Types>
using rtuple_element_t = typename rtuple_element<Index, Types...>::type;

} // namespace details

template <typename... Types> struct rtuple : public details::rtuple_impl<Types...> {
	using base_type = details::rtuple_impl<Types...>;

	// clang-format off
	template <typename... Values>
	requires(sizeof...(Values) == sizeof...(Types))
	constexpr explicit rtuple(Values&&... values) noexcept
	    : base_type(std::forward<Values>(values)...) {}
	// clang-format on
	constexpr ~rtuple() noexcept = default;
};

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(rtuple<Types...>& tuple) noexcept {
	using impl_type = details::rtuple_helper_t<index, Types...>;
	return (static_cast<impl_type&>(tuple).m_value);
}

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(const rtuple<Types...>& tuple) noexcept {
	using impl_type = details::rtuple_helper_t<index, Types...>;
	return (static_cast<const impl_type&>(tuple).m_value);
}

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(rtuple<Types...>&& tuple) noexcept {
	using impl_type  = details::rtuple_helper_t<index, Types...>;
	using value_type = details::rtuple_element_t<index, Types...>;
	return std::forward<value_type>(static_cast<impl_type&&>(tuple).m_value);
}

} // namespace meta

namespace std {

template <typename... Types>
struct tuple_size<meta::rtuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <std::size_t index, typename... Types>
struct tuple_element<index, meta::rtuple<Types...>> {
	using type = meta::details::rtuple_element_t<index, Types...>;
};

using meta::get;

} // namespace std

#endif //META_RECURSIVE_INHERITANCE_TUPLE_HPP
