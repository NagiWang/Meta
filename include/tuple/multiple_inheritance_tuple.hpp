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

struct empty_class {};

template <std::size_t Index, typename Type>
struct mtuple_value : empty_class {
	using this_type  = mtuple_value<Index, Type>;
	using value_type = Type;

	value_type m_value;

	template <typename ValueType>
	requires std::is_nothrow_constructible_v<value_type, ValueType>
	constexpr explicit mtuple_value(ValueType&& value) noexcept
	    : m_value(std::forward<ValueType>(value)) {}

	constexpr ~mtuple_value() noexcept = default;
};

template <typename, typename...>
struct mtuple_impl;

template <std::size_t... Index, typename... Types>
struct mtuple_impl<std::index_sequence<Index...>, Types...> : public mtuple_value<Index, Types>... {
	// clang-format off
	template <typename... Values>
	constexpr explicit mtuple_impl(Values&&... values) noexcept
	    :  mtuple_value<Index, Types>(std::forward<Values>(values))... {}
	// clang-format on
	constexpr ~mtuple_impl() noexcept = default;
};

template <std::size_t Index, typename... Types>
struct mtuple_element;

template <typename Head, typename... Tail>
struct mtuple_element<0, Head, Tail...> {
	using type = Head;
};

template <std::size_t Index, typename Head, typename... Tail>
requires(Index != 0) struct mtuple_element<Index, Head, Tail...> {
	using type = typename mtuple_element<Index - 1, Tail...>::type;
};

template <std::size_t Index, typename... Types>
using mtuple_element_t = typename mtuple_element<Index, Types...>::type;

// clang-format off
template <std::size_t Index, typename ...Types>
struct mtuple_helper {
	using type = mtuple_value<Index, typename mtuple_element<Index, Types...>::type>;
};
// clang-format on

template <std::size_t Index, typename... Types>
using mtuple_helper_t = typename mtuple_helper<Index, Types...>::type;

} // namespace details

template <typename... Types>
struct mtuple : public details::mtuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...> {
	using base_type = details::mtuple_impl<std::make_index_sequence<sizeof...(Types)>, Types...>;

	// clang-format off
	template <typename... Values>
	requires(sizeof...(Values) == sizeof...(Types))
	constexpr explicit mtuple(Values&&... values) noexcept
	    : base_type(std::forward<Values>(values)...) {}
	// clang-format on
	constexpr ~mtuple() noexcept = default;
};

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(mtuple<Types...>& tuple) noexcept {
	using impl_type = details::mtuple_helper_t<index, Types...>;
	return (static_cast<impl_type&>(tuple).m_value);
}

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(const mtuple<Types...>& tuple) noexcept {
	using impl_type = details::mtuple_helper_t<index, Types...>;
	return (static_cast<const impl_type&>(tuple).m_value);
}

template <std::size_t index, typename... Types>
constexpr decltype(auto) get(mtuple<Types...>&& tuple) noexcept {
	using impl_type  = details::mtuple_helper_t<index, Types...>;
	using value_type = details::mtuple_element_t<index, Types...>;
	return std::forward<value_type>(static_cast<impl_type&&>(tuple).m_value);
}

} // namespace meta

namespace std {

template <typename... Types>
struct tuple_size<meta::mtuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <std::size_t index, typename... Types>
struct tuple_element<index, meta::mtuple<Types...>> {
	using type = meta::details::mtuple_element_t<index, Types...>;
};

using meta::get;

} // namespace std

#endif //META_MULTIPLE_INHERITANCE_TUPLE_HPP
