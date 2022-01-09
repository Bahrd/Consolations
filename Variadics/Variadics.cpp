import std.core;

// https://en.cppreference.com/w/cpp/utility/integer_sequence
// Pretty-Print a Tuple 
template<class Ch, class Tr, class Tuple, std::size_t... Is>
void type_tuple(std::basic_ostream<Ch, Tr>& os, const Tuple& t, std::index_sequence<Is...>)
{
    // A remarkably ultra-hiper-super notable extra-smart use of 
    // 'operators << ()' (pre-)defined for all standard C++ types as recurrence stops...
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<class Ch, class Tr, class... Args>
auto& operator <<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t)
{
    auto seq = std::index_sequence_for<Args...>{};    
    return os << "(", type_tuple(os, t, seq), os << ")";
}

// https://youtu.be/_FoXWnrGuNU?t=640 
template <typename TT, typename...>
struct first_arg_t { using T = TT; };

template <typename... T>
concept additive = requires(T... t)
{
    (... + t) + (t + ...);
    requires sizeof...(T) > 1;
};

template <typename... T> requires additive<T...>
constexpr first_arg_t<T...>::T add_twice(T&& const ...t)
{
    return (... + t) + (t + ...);
}

int main()
{    
    std::cout << std::boolalpha   << std::tuple{ 1, '1', "1", std::tuple{ 1.0, true  }} << std::endl
              << std::noboolalpha << std::tuple{ 0, "0", '0', std::tuple{ 0.0, false }} << std::endl
              << std::bitset<8>{add_twice(0b1, 0b10, 0b11, 0x10, 010)};
    return 0;
}