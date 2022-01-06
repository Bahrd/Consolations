#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// https://en.cppreference.com/w/cpp/thread/barrier
int main() 
{
    const auto state_abrevs = { "AZ", "NV", "UT", "ID", "WY", "CO"};
    auto phase = "... Route planned!\n" "Hitting the road...\n";

    std::barrier sync_point(std::ssize(state_abrevs), [&phase]() noexcept
    {
        std::cout << phase;
        phase = "... back home!\n";
    });

    auto pass = [&](std::string state_name) 
    {
        std::string itinerary = "  " + state_name + " allowed\n";
        std::cout << itinerary; 
        sync_point.arrive_and_wait();

        itinerary = "  " + state_name + " visited\n";
        std::cout << itinerary;
        sync_point.arrive_and_wait();
    };

    std::cout << "Planning...\n";
    std::vector<std::jthread> threads;
    for (auto const& state_abrev : state_abrevs) 
        threads.emplace_back(pass, state_abrev);
}