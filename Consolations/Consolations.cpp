#include <numeric>
#include <future>
#include <iostream>
#include <numbers>
#include <ranges>
#include <string>

// A specialization (the recursion stopper) of the...
template<typename T>
void vtprint(T const &value = T())
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

        auto inv = []<typename T>(T i) { return static_cast<double>(1.0 / i); };
        auto even = []<typename T>(T i) { return 0 == i % 0b10; };
        auto print = []<typename T>(T i) { return std::cout << i << '\t', static_cast<T>(i); };

        for (auto i : iota(-11) | filter(even) | transform(inv) | transform(print) | take(11));
        std::cout << '\n';
        // https://en.cppreference.com/w/cpp/thread/future
        // https://en.cppreference.com/w/cpp/thread/promise
        // https://sodocumentation.net/cplusplus/topic/9840/futures-and-promises 
        auto fun_ptr = vtprint<int, std::string, char, double>;
        auto a0 = std::async(std::launch::deferred, fun_ptr,
            0100,
            "async variadic template function call!",
            '\100',
            std::numbers::pi);
        auto dummy = 0;
        std::cout << (std::cin >> dummy, a0.wait(), dummy) << std::endl;
    }
    {
        std::vector<int> v(0x1000, 0b1);
        using T = decltype(begin(v));
        std::function<int(T, T)> pms = [&pms, &v](T a, T b)
        {
            auto len = b - a;
            if (len < 0x100)
            {
                return std::accumulate(a, b, 0);
            }
            else
            {
                auto m = a + len/02;
                auto handle = std::async(std::launch::async, pms, m, b);
                auto sum = pms(a, m);
                return sum + handle.get();
            }
        };
        // A parallel recurrence (anonymous and not) example invocation...
        std::cout << std::format("Are both signed and anonymous sums made equal?\nIs {} == {}?\n",
                                 par_sum(begin(v), end(v)),
                                 pms(begin(v), end(v)));
    }
    return 0;
}