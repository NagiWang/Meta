#ifndef META_SHARED_PTR_IPP
#define META_SHARED_PTR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "shared_ptr.hpp"

#include <mutex>
#include <type_traits>
#include <memory>

namespace meta {

template <typename T>
struct shared_ptr<T>::shared_ptr_impl {
private:
	std::mutex m_mutex{};
	std::size_t m_count = 1;
	T* m_data           = nullptr;

public:
	constexpr shared_ptr_impl() noexcept = default;
	constexpr explicit shared_ptr_impl(T* data) noexcept : m_mutex{}, m_count{1}, m_data{data} {}
	void increaseRef() noexcept {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_count == 0) return;
		else
			++m_count;
	}
	void decreaseRef() noexcept {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_count == 0) {
			delete m_data;
			m_data = nullptr;
		}
		else
			--m_count;
	}
	std::size_t countRef() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_count;
	}
	constexpr ~shared_ptr_impl() noexcept = default;
};

template <typename T>
constexpr shared_ptr<T>::shared_ptr(T* ptr) noexcept : impl{new shared_ptr_impl(ptr)} {}

template <typename T>
constexpr shared_ptr<T>::shared_ptr(const shared_ptr& other) noexcept : impl{other.impl} { impl->increaseRef(); }

template <typename T>
constexpr shared_ptr<T>::shared_ptr(shared_ptr&& other) noexcept : impl{other.impl} {}

template <typename T>
constexpr shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr& other) noexcept {
	if (&other == this) return *this;
	if (impl != nullptr) {
		impl->decreaseRef();
		impl = other.impl;
		impl->increaseRef();
	}
	return *this;
}

template <typename T>
constexpr shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr&& other) noexcept {
	if (impl != nullptr) impl->decreaseRef();
	impl = other.impl;
	return *this;
}
template <typename T>
constexpr shared_ptr<T>::~shared_ptr() noexcept {
	if (impl != nullptr) impl->decreaseRef();
	else if (impl->countRef() == 0) {
		delete impl;
		impl = nullptr;
	}
}
template <typename T>
constexpr T& shared_ptr<T>::operator*() {
	if (impl != nullptr) return *(impl->m_data);
	else throw std::exception("Empty shared_ptr cannot use operator* method!\n");
}

template <typename T>
constexpr T* shared_ptr<T>::operator->() {
	if (impl != nullptr) return impl->m_data;
	else throw std::exception("Empty shared_ptr cannot use operator* method!\n");
}

template <typename T>
constexpr std::size_t shared_ptr<T>::count() const {
	if (impl != nullptr) return impl->countRef();
	else return 0;
}

} // namespace meta

#endif //META_SHARED_PTR_IPP
