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
template <typename _T, typename...>
struct first_arg_t { using T = _T; };

template <typename... T>
concept commutative_addition = requires(T... t)
{
    (... + t) <=> (t + ...);
    requires sizeof...(T) > 1;
};

template <typename... T> requires commutative_addition <T...>
constexpr first_arg_t<T...>::T add(T&& ...t)
{
    return (... + t);
}

int main()
{    
    std::cout << std::noboolalpha << std::tuple{ 1, '1', "1", std::tuple{ 1.0, true  }} << std::endl
              << std::boolalpha   << std::tuple{ 0, "0", '0', std::tuple{ 0.0, false }} << std::endl
              << std::bitset<0b111>{add(0b1, 0b10, 0111, 0x10, 0x1)};
    return 0;
}