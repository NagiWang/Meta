#ifndef META_SHARED_PTR_HPP
#define META_SHARED_PTR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

namespace meta {

template <typename T>
class shared_ptr {
	struct shared_ptr_impl;
	shared_ptr_impl* impl = nullptr;

public:
	constexpr explicit shared_ptr() noexcept = default;
	constexpr explicit shared_ptr(T* ptr) noexcept;
	constexpr shared_ptr(const shared_ptr& other) noexcept;
	constexpr shared_ptr(shared_ptr&& other) noexcept;
	constexpr shared_ptr& operator=(const shared_ptr& other) noexcept;
	constexpr shared_ptr& operator=(shared_ptr&& other) noexcept;
	constexpr ~shared_ptr() noexcept;
	constexpr std::size_t count() const;
	constexpr T& operator*();
	constexpr T* operator->();
};

} // namespace meta

#include "shared_ptr.ipp"

#endif //META_SHARED_PTR_HPP
