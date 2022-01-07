﻿#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// https://en.cppreference.com/w/cpp/thread/barrier
// chcp 65001 - to see notes in a terminal!
int main() 
{
    const auto itinerary = { "AZ", "NV", "UT", "ID", "WY", "CO" };
    auto phase = "♫ ... when the ⅄⅃LY breaks!\n" "♫ Wherever I may roam!\n";
    std::barrier sync_point(std::ssize(itinerary), [&phase]() noexcept
    {
        std::cout << phase;
        phase = "... back home!\n";
    });

    auto pass = [&](std::string state_name)
    {
        std::string itinerary_report = "♫ Ramble on, " + state_name + "!\n";
        std::cout << itinerary_report;
        sync_point.arrive_and_wait();

        itinerary_report = "♫ Turn the " + state_name + " page...\n";
        std::cout << itinerary_report;
        sync_point.arrive_and_wait();
    };

    std::cout << "Planning...\n";
    std::vector<std::jthread> threads;
    for (auto const& itinerary_item : itinerary)
        threads.emplace_back(pass, itinerary_item);
}