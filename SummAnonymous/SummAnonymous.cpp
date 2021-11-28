#include <vector>
#include <functional>
#include <numeric>
#include <future>
#include <iostream>
int main()
{
    // An anonymous and asynchronous (and almost templatized...) 
    // recursive function... 
    // A 'pms' acronym stands for 'parallel massive summation'. Yup, really... 
    std::vector<int> v(0x1000, 0b01);
    using T = std::vector<int>::const_iterator;
    std::function<int(T, T)> pms = [&](T beg, T end)
    {
        auto len = end - beg;
        if (len < 0x100) return std::accumulate(beg, end, 000);

        auto mid = beg + len / 0b10;
        auto launch_mode = std::launch::deferred | std::launch::async;
        auto async = std::async(launch_mode, pms, mid, end);
        return pms(beg, mid) + async.get();
    };
    // An asynchronous parallel anonymous example recursive invocation...
    std::cout << "\nAnonymously asynchronous summa = "
        << pms(begin(v), end(v)) << '\n';
    return 0;
}