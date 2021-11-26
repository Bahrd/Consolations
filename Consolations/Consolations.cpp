#include <numeric>
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

int main()
{
    {
        using std::views::take, std::views::transform, std::views::filter;
        using std::views::iota;

        auto lambdas = std::make_tuple([]<typename T>(T i)  { return 0 == i % 0b10; },
                                       []<typename T>(T i)  { return static_cast<double>(01) / i; },
                                       []<typename T>(T i)  { return std::cout << i << '\t', static_cast<T>(i); } );
        
        for (auto i : iota(-11)                       | 
                      filter(std::get<0>(lambdas))    |
                      transform(std::get<1>(lambdas)) |
                      transform(std::get<2>(lambdas)) |
                      take(11));
        std::cout << '\n';

        // https://en.cppreference.com/w/cpp/thread/future
        // https://en.cppreference.com/w/cpp/thread/promise
        // https://sodocumentation.net/cplusplus/topic/9840/futures-and-promises 
        auto fun_ptr = vtprint<int, std::string, char, double>;
        auto a_fun = std::async(std::launch::deferred, fun_ptr,
                                0100,
                                "async variadic template function call!",
                                '\100',
                                std::numbers::pi);
        auto dummy = 0;
        std::cout << (std::cin >> dummy, a_fun.wait(), dummy) << std::endl;
    }
    {
        // using T = std::vector<int>::const_iterator;  // One way...
        std::vector<int> v(0x1000, 0b1);
        using T = decltype(begin(v));                   // ... or another.
        std::function<int(T, T)> pms = [&pms, &v](T a, T b)
        {
            auto len = b - a;
            if (len < 0x100) return std::accumulate(a, b, 00);

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