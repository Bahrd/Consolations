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

int main()
{
    auto tup_tuple = std::make_tuple(1, '1', "1", 1.0, true, 
                     std::make_tuple(0, '0', "0", 0.0, false));
    
    return std::cout << tup_tuple, 0;
}