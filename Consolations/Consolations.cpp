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

int main()
{
    using namespace std::placeholders;
    using std::views::iota, 
          std::views::filter, std::views::transform, 
          std::views::take;

    // Where functions are the first class-citizens...: 
    // https://en.wikipedia.org/wiki/First-class_function    
    // A tuple of anonymous items... 
    auto lambdas = std::tuple([]<typename T>(T i) -> bool             { return 0 == i % 0b10; },
                              []<typename T>(T i) -> double           { return static_cast<double>(01) / i; },
                              []<typename T>(T i, char e = '\t') -> T { return std::cout << i << e, static_cast<T>(i); });
    for (auto _ : iota(-11)                    
                | filter(std::get<0b0>(lambdas)) | transform(std::get<0b1>(lambdas)) | transform(std::get<0b10>(lambdas)) 
                | take(11)
                );
    // Named tuple/pair...    std::pair(..., ...);
    auto [twice, writeline] = std::tuple([]<typename T>(T i) -> T     { return static_cast<T>(0b10) * i; },
                              std::bind(std::get<2>(lambdas), _1, '\n'));
    for (auto _ : std::ranges::istream_view<double>(std::cin)
                | transform(twice) | transform(writeline)
                );
    // Not ready for a prime time yet: 
    // 'std::ranges::ostream_iterator<double>(std::cout, "\n")'
    // See e.g.: http://ericniebler.github.io/range-v3/index.html and https://cxx20ranges.brcha.com/#/5/19
    return 0;
}