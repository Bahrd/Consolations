#include <numeric>
#include <functional>
#include <future>
#include <iostream>
#include <numbers>
#include <ranges>
#include <string>
/*Programming is one of the most difficult branches of applied mathematics;
  the poorer mathematicians had better remain pure mathematicians."
  Edsger W Dijkstra, "How do we tell truths that might hurt?", 1975
  */
// A specialization (the recursion stopper) of the...
template<typename T>
void vtprint(T const& value)
{
    std::cout << value << std::endl;
}
// ... variadic template function template
// https://docs.microsoft.com/en-us/cpp/cpp/ellipses-and-variadic-templates?view=msvc-160
template<typename T, typename... Targs>
void vtprint(T const& value, const Targs&... Fargs)
{
    std::cout << value << std::endl;
    vtprint(Fargs...);
}

int main()
{
    // https://en.cppreference.com/w/cpp/thread/future & https://en.cppreference.com/w/cpp/thread/promise
    // https://sodocumentation.net/cplusplus/topic/9840/futures-and-promises 
    auto fun_fun = std::async(std::launch::deferred,
                              vtprint<int, std::string, char, double>,
                              0100,
                              "async variadic template function call!",
                              '\100',
                              std::numbers::pi);
    auto dummy = 000;
    std::cout << (std::cin >> dummy, fun_fun.wait(), dummy) 
              << std::endl;
    return 0;
}