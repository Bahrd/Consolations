import std;

/* https://en.cppreference.com/w/cpp/utility/integer_sequence
   Pretty-Print a Tuple */
template<class Ch, class Tr, class Tuple, std::size_t... Is>
void type_tuple(std::basic_ostream<Ch, Tr>& os, const Tuple& t, std::index_sequence<Is...>)
{
    /* A remarkably ultra - hiper - super notable extra - smart use of
       'operators << ()' (pre-)defined for all standard C++ types 
        as the compilation-time recurrence stops...
        Note further that tuples can be embedded! It's a recursion after all! */
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

/* It is not a requirement that actually imposes a commutativity of addition...
   Moreover, for strings it is rather a concatenation... (which is not commutative!) */
template <typename... T>
concept commutative_addition = requires(T... t)
{
    (... + t) == (t + ...); // A dubious requirement... syntax says (almost) nothing about semantics...
    requires sizeof...(T) > 1;
};

template <typename... T>
requires commutative_addition<T...>
consteval first_arg_t<T...>::T add(T&& ...t)
{
    return (... + t);
}

/* The example below is not yet ready for a prime time:
   https://en.cppreference.com/w/cpp/language/parameter_pack - not for the fainthearted, 
                                                               but rather for the brave...*/
template<class... Args>
first_arg_t<Args...>::T g(Args... args)
{
    f(const_cast<const Args*>(&args)...);   /* const_cast<const Args*>(&args) is the pattern, it
                                               expands two packs (Args and args) simultaneously */
    f(h(args...) + args...); /* Nested pack expansion:
                                + inner pack expansion is "args...", it is expanded first
                                + outer pack expansion is h(E1, E2) + args..., it is expanded
                                + second (as h(E1, E2) + E1, h(E1, E2) + E2) */
}

int main()
{
    using namespace std;
    // Note (x, y) is not a tuple, it is a C's comma operator, that yields y!
    cout << noboolalpha << tuple{ 1, '1', "1", tuple{ 1.0, ("true", false)}} << endl
         << boolalpha   << tuple{ 0, "0", '0', tuple{ 0.0, (("true", false), true)}} << endl
         << bitset<0b111> {add(0b1, 0b10, 0'111, 0x10, 0x1)} << endl  // Pick trick...
         << format("{:b}", add(0b1, 0b10, 0'111, 0x10, 0x1)) << endl; // ... or threat
    return 0;
}