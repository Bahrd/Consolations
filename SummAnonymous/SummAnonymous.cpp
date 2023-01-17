#include <vector>
#include <format>
#include <functional>
#include <numeric>
#include <execution>
#include <future>
#include <iostream>
#include <iomanip>
// An anonymous and asynchronous (and almost templatized...) 
// recursive function... 
// ... vs the standard library veterans!
int main()
{
    using millisec_t = std::chrono::duration<double, std::milli>;

    auto v = std::vector<int>(0x10'000'00, 0b01);
    {
        // A bling-bling flavour of the standard library routine...
        auto [tick, summa, tack] = std::tuple(std::chrono::high_resolution_clock::now(),
                                              std::reduce(std::execution::par, begin(v), end(v)),
                                              std::chrono::high_resolution_clock::now());
        
        std::cout << "Standard parallelized summa = " << summa
                  << " in " << std::fixed << std::setprecision(0)
                  << millisec_t(tick - tack).count()
                  << "ms\n";
    }
    {
        // A 'pms' acronym stands for 'paralyzed massive summation'. Yup, really... 
        // https://en.cppreference.com/w/cpp/utility/functional/function
        using T = std::vector<int>::const_iterator;
        std::function<int(T, T)> pms = [&](T beg, T end)
        {
            auto len = end - beg;
            if (len < 0x100) return std::accumulate(beg, end, 000);

            auto [launch_mode, mid] = std::pair(std::launch::deferred | std::launch::async, beg + len / 0b10);
            auto async = std::async(launch_mode, pms, mid, end);
            return pms(beg, mid) + async.get();
        };

        auto tick = std::chrono::steady_clock::now();
        auto summa = pms(begin(v), end(v));
        auto tack = std::chrono::steady_clock::now();
        
        std::cout << std::format("Anonymously paralyzed summa = {} in {}ms\n", summa,
                                 millisec_t(tack - tick).count());
    }
    {
        // Just a standard library routine...
        // (Are the tuple's elements initialized in order of they appearance?)
        auto timer = std::tuple(std::chrono::system_clock::now(), 
                                std::reduce(std::execution::seq, begin(v), end(v)),
                                std::chrono::system_clock::now());
        
        std::cout << "Standard sequential summa = " << std::get<0b01>(timer)
                  << " in "
                  << millisec_t(std::get<0b00>(timer) - std::get<0b10>(timer)).count()
                  << "ms\n";
    }
    return 0;
}