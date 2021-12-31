# Tuple 异质容器

![background](../img/bb.png)

在 **Modern C++** 中，很多项目都有用到异质容器 `tuple`。有的项目用的直接就是 **C++** 标准库里的 `std::tuple`，有的项目可能会根据自己的需求重新实现一个。现在 `tuple` 实现方式一般分为两种，一种是 *递归继承 (Recursive Inheritance)*，另一种是 *多继承 (Multiple Inheritance)*，一般来说后者的性能可能会更好一些。本文会分别探讨这两种实现方式，最后会讲一下如何实现 **C++ 17** 中的 [结构化绑定声明](https://zh.cppreference.com/w/cpp/language/structured_binding) 如何实现，如何获取 `tuple` 的元素也将在结构化绑定这块讲。由于不太想写 [SFINAE](https://zh.cppreference.com/w/cpp/language/sfinae) 所以我采用了 **C++ 20** 的 [概念与约束 (Constraints and Concepts)](https://zh.cppreference.com/w/cpp/language/constraints) 来实现，所有代码都需要以 **C++ 20** 编译。

---

## 递归继承

递归式继承的 `tuple` 实现起来相对比较简单，像微软的 [Microsoft STL](https://github.com/microsoft/STL) 就是用该方式实现的，它的实现可以参考 [Here](https://github.com/microsoft/STL/blob/main/stl/inc/tuple)。递归式实现的 `tuple` 我将其命名为 `rtuple`。为了隐藏实现细节，用 `rtuple_impl` 来实现，`rtuple` 仅用于封装。对于一个 `rtuple<Type1, Type2, Type3>` 其继承关系大体如下所示

```cpp
struct rtuple<Type1, Type2, Type3>      : public rtuple_impl<Type1, Type2, Type3>;
struct rtuple_impl<Type1, Type2, Type3> : public rtuple_impl<Type2, Type3>;
struct rtuple_impl<Type2, Type3>        : public rtuple_impl<Type3>;
struct rtuple_impl<Type3>               : public rtuple_impl<>;
```

可以看到 `rtuple<int, char, double>` 继承自实现 `rtuple_impl<int, char, double>`，而 `rtuple_impl<int, char, double>` 又递归地继承自 `rtuple_impl<char, double>`、`rtuple_impl<double>`、`rtuple_impl<>`。数据便是保存在 `rtuple_impl` 之中，比如 `rtuple_impl<int, char, double>` 保存其第一个类型 `int` 的数据，`rtuple_impl<char, double>` 也是保存其第一个类型 `char` 的数据；而 `rtuple_impl<>` 不保存任何数据，仅用作于 [空基类优化](https://zh.cppreference.com/w/cpp/language/ebo)。

`rtuple` 的实现重点在于实现 `rtuple_impl`，首先需要声明 `rtuple_impl`

```cpp
template <typename... Types> struct rtuple_impl;
```

然后定义其递归基 `rtuple_impl<>`

```cpp
template <> struct rtuple_impl<> {
    constexpr rtuple_impl() noexcept  = default;
    constexpr ~rtuple_impl() noexcept = default;
};
```

看到这可能有人就要问了，为啥不用 `virtual destructor` ?
因为没这个必要！我们的 `rtuple_impl` 不是用作多态基类用途，加上了只 `virtual` 会使对象增加一个虚表 `vtable` 占用更多的内存。我们最终使用的是它的子类 `rtuple<...>` 而不是基类 `rtuple_impl<...>`，所以也不存在基类析构了而子类未析构的问题。

---

接下来便是定义 `rtuple_impl<...>` 的递归继承关系以及类成员，这一块比之前的略微复杂。

递归关系非常好定义，比如有一个 `rtuple_impl<Types...>`，其类型序列为 `Types...` 可以分为第一个类型 `Head` 和余后的类型序列 `Tail...`。我们让`rtuple_impl<Types...>` 继承自 `rtuple_impl<Tail...>` 即可，

```cpp
template <typename Head, typename... Tail>
struct rtuple_impl<Head, Tail...> : public rtuple_impl<Tail...> { ... }
```

比如 `rtuple_impl<Type1, Type2, Type3>` 便可展开为

```cpp
struct rtuple_impl<Type1, Type2, Type3> : public rtuple_impl<Type2, Type3> {...};
struct rtuple_impl<Type2, Type3>        : public rtuple_impl<Type3> {...};
struct rtuple_impl<Type3>               : public rtuple_impl<> {...};
```

由于 `rtuple_impl<>` 我们已经实现了，所以递归到这就会停下来。

---

再然后是定义 `rtuple_impl<...>` 的类成员，其中主要包含:

- 两个类型别名
  - `base_type` : 即基类类型 `rtuple_impl<Tail...>`
  - `value_type` : 即该类的首类型 `Head`，也是其数据成员的类型
- 数据成员 : `m_value`，类型为 `Head`
- 构造函数
- 默认析构函数

具体代码如下

```cpp
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
```

其中比较复杂的就只有构造函数了，它接受一个万能引用序列 `value, others...`，并将序列第一个值 `value` 用来构造类成员 `m_value`，其余的值序列 `other...` 通过 `std::forward` 转发给基类构造函数。
`requires` 是 **C++ 20** 的新关键词，具体用法请移步 [概念与约束 (Constraints and Concepts)](https://zh.cppreference.com/w/cpp/language/constraints)。`requires std::is_nothrow_constructible_v<value_type, FirstValue>` 这里是为了确保可以通过类型 `FirstValue` 来构造类型 `value_type`，即用 `value` 来构造 `m_value`。`value` 的类型不一定非要与 `m_value` 的一致，比如用 `double` 类型的值也可用来构造 `int` 类型的变量，尽管数值溢出可能会导致一些奇怪的错误。此处也可以用 [SFINAE](https://zh.cppreference.com/w/cpp/language/sfinae) 来实现，具体替代方法如下

```cpp
template <typename Head, typename... Tail>
struct rtuple_impl<Head, Tail...> : public rtuple_impl<Tail...> {
    using base_type  = rtuple_impl<Tail...>;
    using value_type = Head;

    value_type m_value;

    template <typename FirstValue, typename... OtherValues,
              std::enable_if_t<std::is_nothrow_constructible_v<value_type, FirstValue>, int> = 0>
    constexpr explicit rtuple_impl(FirstValue&& value, OtherValues&&... others) noexcept
        : base_type(std::forward<OtherValues>(others)...)
        , m_value(std::forward<FirstValue>(value)) {}

    constexpr ~rtuple_impl() noexcept = default;
};
```

显然还是用 `requires` 更简单明了。
