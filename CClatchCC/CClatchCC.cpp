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
    std::string const name{};
    std::string status{"Assigned"};
    std::thread action{};
}
dream_team[] = {{"Fabian Cancellara"}, 
                {"Filippo Ganna"}, 
                {"Tony Martin"}};
int main() 
{
    std::latch team{std::size(dream_team)}, 
               last_on_finish{0b1};
    auto [t, U] = std::tuple(std::default_random_engine(std::random_device{}()), 
                             std::uniform_int_distribution<int>(0b1, 0b11));
    auto work = [&](tt_profile& tt) 
    {
        tt.status = tt.name + " started";      
        team.count_down();
        
        std::this_thread::sleep_for(U(t) * 0b1s);

        last_on_finish.wait();
        tt.status = tt.name + " finished";
    };
    std::cout << "TT start... begins ";
    
    for (auto& rider : dream_team) rider.action = std::thread{work, std::ref(rider)};
    team.wait(), std::cout << "& TT start ends!\n";
    
    for (auto const& rider : dream_team) std::cout << "  " << rider.status << '\n';
    std::cout << "Riders approaching the finish line... "; 
    last_on_finish.count_down(), std::cout << " all of'em!:\n";

    for (auto& rider : dream_team) rider.action.join(), std::cout << "  " << rider.status << '\n';  
    return 0;
}
