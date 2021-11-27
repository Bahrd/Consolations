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
void vtprint(T const &value)
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
// A parallel recursive function invocation example...
template <typename T>
int par_sum(T beg, T end)
{
    auto len = end - beg;
    if (len < 100) return std::accumulate(beg, end, 0);

    T mid = beg + len / 2;
    auto handle = std::async(std::launch::async, par_sum<T>, mid, end);
    int sum = par_sum(beg, mid);
    return sum + handle.get();
};

//auto wrt(int i, char c) { std::cout << i << c; };
int main()
{
    using namespace std::placeholders;
    //auto wrtln = std::bind(wrt, _1, 'X');
    //wrtln(1);

    {
        // See e.g. https://cxx20ranges.brcha.com/#/5/19
        using std::views::iota, 
              std::views::filter, std::views::transform, 
              std::views::take;

        auto lambdas = std::make_tuple([]<typename T>(T i)  { return 0 == i % 0b10; },
                                       []<typename T>(T i)  { return static_cast<double>(01) / i; },
                                       []<typename T>(T i, char e = '\t') { return std::cout << i << e, static_cast<T>(i); });

        for (auto _ : iota(-11)                    
                    | filter(std::get<0b0>(lambdas)) | transform(std::get<0b1>(lambdas)) | transform(std::get<0b10>(lambdas)) 
                    | take(11)
                    );

        std::cout << '\n';
        auto twice = []<typename T>(T i) -> T { return static_cast<T>(0b10) * i; };
        auto writeline = std::bind(std::get<2>(lambdas), _1, '\n');

        for (auto i : std::ranges::istream_view<double>(std::cin)
                    | transform(twice) | transform(writeline)
                    // Not ready for a prime time yet: http://ericniebler.github.io/range-v3/index.html
                    // | std::ranges::ostream_iterator<double>(std::cout, "\n")
                    );

        // https://en.cppreference.com/w/cpp/thread/future
        // https://en.cppreference.com/w/cpp/thread/promise
        // https://sodocumentation.net/cplusplus/topic/9840/futures-and-promises 
        auto fun_fun = std::async(std::launch::deferred, 
                                  vtprint<int, std::string, char, double>,
                                  0100,
                                  "async variadic template function call!",
                                  '\100',
                                  std::numbers::pi);
        auto dummy = 000;
        std::cout << (std::cin >> dummy, fun_fun.wait(), dummy) << std::endl;
    }
    {
        // using T = std::vector<int>::const_iterator;  // One way...
        std::vector<int> v(0x1000, 0b1);
        using T = decltype(begin(v));                   // ... or another.
        std::function<int(T, T)> pms = [&pms, &v](T a, T b)
        {
            auto len = b - a;
            if (len < 0x100) return std::accumulate(a, b, 0b0);

            auto m = a + len/0b10;
            auto handle = std::async(std::launch::async, pms, m, b);
            auto sum = pms(a, m);
            return sum + handle.get();
        };
        // A parallel recurrence (anonymous and not) example invocation...
        std::cout << std::format("Are both signed and anonymous sums made equal?\nIs {} == {}?\n",
                                  par_sum(begin(v), end(v)),
                                  pms(begin(v), end(v)));
    }
    return 0;
}