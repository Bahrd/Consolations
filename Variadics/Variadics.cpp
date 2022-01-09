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
    (... + t);
    requires sizeof...(T) > 1;
};

template <typename... T> requires additive<T...>
first_arg_t<T...>::T add(T&& ...t)
{
    return (... + t);
}

int main()
{    
    std::cout << std::boolalpha   << std::tuple{ 1, '1', "1", std::tuple{ 1.0, true  }} << std::endl
              << std::noboolalpha << std::tuple{ 0, "0", '0', std::tuple{ 0.0, false }} << std::endl
              << std::hex << std::showbase << add(1, 2, 3, 4, 5);
    return 0;
}