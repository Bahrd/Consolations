#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// https://en.cppreference.com/w/cpp/thread/barrier
// chcp 65001 - to see notes in a terminal!
int main() 
{
    const auto itinerary = { "AZ", "NM", "CO", "UT", "NV", "CA"};
    std::barrier sync_point(std::ssize(itinerary), []() noexcept
                                                   {
                                                       std::cout << "▀▄▀▄\n";
                                                   });

    auto pass = [&sync_point](std::string state_name)
    {
        auto itinerary_report = "♫ Ramble on, " + state_name + "♫!\n";
        std::cout << itinerary_report;
        sync_point.arrive_and_wait();

        itinerary_report = "@" + state_name + "♫ Turn the page...♫\n";
        std::cout << itinerary_report;
        sync_point.arrive_and_wait();

        itinerary_report = "@" + state_name + "♫ Riders on the storm...♫\n";
        std::cout << itinerary_report;
        sync_point.arrive_and_wait();
    };

    std::cout << "[Aqua-]planning...\n";
    std::vector<std::jthread> threads;
    for (auto const& itinerary_item : itinerary) threads.emplace_back(pass, itinerary_item);

    return 0;
}