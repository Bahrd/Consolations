/*#include <numeric>
#include <functional>
#include <future>
#include <iostream>
#include <numbers>
#include <ranges>
#include <string>
*/

import std.core;
/*"Programming is one of the most difficult branches of applied mathematics;
   the poorer mathematicians had better remain pure mathematicians."
   Edsger W Dijkstra, "How do we tell truths that might hurt?", 1975
  */
// A specialization (the recursion stopper) of the...
template<typename T>
constexpr void varia_temp_type(T const& value)
{
    std::cout << value << std::endl;
}
// ... variadic template function template
// https://docs.microsoft.com/en-us/cpp/cpp/ellipses-and-variadic-templates?view=msvc-160
template<typename T, typename... Targs>
constexpr void varia_temp_type(T const& value, const Targs&... Fargs)
{
    std::cout << value << std::endl;
    varia_temp_type(Fargs...);
}

// https://www.youtube.com/watch?v=1hwRxW99lg0
constexpr auto _0()
{
    return +![] {};
}

constexpr auto _n(auto _)
{
    return  _ + !(![] {}); // _ + !_0();
}

int main()
{
    constexpr auto _0_ = _0(),
                   _1_ = _n(_0_),
                   _2_ = _n(_n(_0_));
    static_assert( _2_ + _1_ == _n(_2_));

    // https://en.cppreference.com/w/cpp/thread/future & https://en.cppreference.com/w/cpp/thread/promise
    // https://sodocumentation.net/cplusplus/topic/9840/futures-and-promises 
    auto a_fun_fun = std::async(std::launch::deferred,
                              varia_temp_type<int, std::string, char, double>,
                              0100,
                              "async variadic template function!",
                              '\100',
                              std::numbers::pi);
    std::cout << (varia_temp_type(010, "variadic template function!", '\10', std::numbers::e), a_fun_fun.wait(), 01)
              << std::endl;
    return 0;
}