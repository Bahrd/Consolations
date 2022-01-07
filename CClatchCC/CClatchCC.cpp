// https://en.cppreference.com/w/cpp/thread/latch
// chcp 65001 - to see notes in a terminal!
#include <chrono>
#include <functional>
#include <iostream>
#include <latch>
#include <random>
#include <string>
#include <thread>

using namespace std::literals;
struct tt_profile
{
    const std::string name;
    std::string status{ "Assigned" };
    std::thread action{};
} 
dream_team[] = { {"Fabian Cancellara"}, {"Filippo Ganna"}, {"Tony Martin"} };

int main() 
{
    auto [t, U] = std::tuple(std::default_random_engine(std::random_device{}()), std::uniform_int_distribution<int>(1, 3));
    std::latch race{ std::size(dream_team) }, last_on_finish{ 1 };

    auto work = [&](tt_profile& tt) 
    {
        tt.status = tt.name + " started";      
        race.count_down();
        
        std::this_thread::sleep_for(U(t) * 1s);

        last_on_finish.wait();
        tt.status = tt.name + " finished";
    };
    std::cout << "TT start... begins ";
    for (auto& rider : dream_team) rider.action = std::thread{ work, std::ref(rider) };
    race.wait();
    std::cout << "TT start ends...\n";
    for (auto const& rider : dream_team) std::cout << "  " << rider.status << '\n';

    std::cout << "Riders crossing the finish line... ";
    last_on_finish.count_down();
    for (auto& rider : dream_team) rider.action.join();
    std::cout << " all:\n";
    for (auto const& rider : dream_team) std::cout << "  " << rider.status << '\n';
}