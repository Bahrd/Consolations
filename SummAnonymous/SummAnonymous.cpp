#include <vector>
#include <functional>
#include <numeric>
#include <execution>
#include <future>
#include <iostream>
int main()
{
    // An anonymous and asynchronous (and almost templatized...) 
    // recursive function... 
    // A 'pms' acronym stands for 'parallel massive summation'. Yup, really... 
    using T = std::vector<int>::const_iterator;
    auto v = std::vector<int>(0x100'000, 0b01);
    std::function<int(T, T)> pms = [&](T beg, T end)
    {
        auto len = end - beg;
        if (len < 0x1000) return std::reduce(std::execution::par, beg, end, 000);

        auto mid = beg + len / 0b10;
        auto launch_mode = std::launch::deferred | std::launch::async;
        auto async = std::async(launch_mode, pms, mid, end);
        return pms(beg, mid) + async.get();
    };
    // An asynchronous parallel anonymous example recursive invocation 
    // vs the standard library one...
    {
        auto [tick, summa, tack] = std::tuple(std::chrono::high_resolution_clock::now(), 
                                              std::reduce(std::execution::par, begin(v), end(v), 000),
                                              std::chrono::high_resolution_clock::now());
        std::cout << "Anonymously asynchronous summa = " << summa
                  << " after " << std::fixed << std::setprecision(1)
                  << std::chrono::duration<double, std::milli>(tick - tack) << "\n";
    }
    {
        auto tick = std::chrono::high_resolution_clock::now();
        auto summa = pms(begin(v), end(v));
        auto tack = std::chrono::high_resolution_clock::now();
        std::cout << std::format("Standard asynchronous summa = {} after {}\n", summa,
                                 std::chrono::duration<double, std::milli>(tack - tick));
    }
    return 0;
}