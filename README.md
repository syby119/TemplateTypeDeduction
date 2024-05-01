## TemplateTypeDeduction

#### General Idea
The general idea is to first match parameter type, and then deduce the template parameter type.

```
template <typename T>
void func(ParamType t) { }

// 1. think of what is the correct type for the following declaration
ParamType t = ActualExpression;

// 2. ParamType -> T
```

The forwarding reference is a special case to be treated, it has its own rules.

#### Parse the type of a name
The most important principle to understanding the type of a name is 'Go right if you can, go left if you must'. If an array is const, it can be interpreted as an array to const elements.

#### Distinguish lvalue, xvalue and prvalue
Using the decltype expression with the following class template
```C++
#include <iostream>
#include <type_traits>

template <typename T> struct is_lvalue: public std::false_type {};
template <typename T> struct is_lvalue<T&> : public std::true_type {};
template <typename T> struct is_lvalue<T&&> : public std::false_type {};

template <typename T> struct is_xvalue: public std::false_type {};
template <typename T> struct is_xvalue<T&> : public std::false_type {};
template <typename T> struct is_xvalue<T&&> : public std::true_type {};

template <typename T> struct is_prvalue: public std::true_type {};
template <typename T> struct is_prvalue<T&> : public std::false_type {};
template <typename T> struct is_prvalue<T&&> : public std::false_type {};

#define ValueCategory(expr) do {                                                      \
    std::cout << "Value Category of `"#expr"`" << std::endl;                          \
    std::cout << std::boolalpha;                                                      \
    std::cout << "  + lvalue:  " << is_lvalue<decltype((expr))>::value << std::endl;  \
    std::cout << "  + xvalue:  " << is_xvalue<decltype((expr))>::value << std::endl;  \
    std::cout << "  + prvalue: " << is_prvalue<decltype((expr))>::value << std::endl; \
} while(0);

int main() {
    ValueCategory(main);
    ValueCategory(std::move(10));
    ValueCategory(10);
}
```