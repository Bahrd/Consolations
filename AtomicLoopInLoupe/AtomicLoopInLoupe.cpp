﻿#include <format>
#include <iostream>
#include <atomic>
#include <string>
#include <thread>
/* A account access (i.e. an obnoxiously innocuously lookin') example of
 * how we have gained parallelism at the expense of determinism...
 * http://web.cse.msu.edu/~cse914/Readings/theProblemWithThreads-Computer06.pdf
 * Hiram Johnson would have said:
 *      "The first casualty when
 *       [concurrency] comes is...
 *       [determinism]!".
 */

int main()
{
    // Atomic C
    {
        using namespace std::literals;
        auto account_balance = std::atomic(0);
        auto [payments, withdrawals] = std::pair(0, 0);

        // "Once upon a time..."
        // In a bank...
        auto payment = std::thread([&]()
        {
            while (payments + withdrawals < 0x100)
            {
                std::this_thread::sleep_for(0b10us);
                // Atomic operator++
                account_balance++, ++payments;

                std::cout << '+'; // A '«progress» indicator'...
            }
        });
        // ... and in a middle of nowhere...
        auto withdrawal = std::thread([&]()
        {
            while (payments + withdrawals < 0x100)
            {
                // Atomic --operator
                --account_balance, ++withdrawals;
                std::this_thread::sleep_for(0b11us);

                std::cout << '-'; // A '«regress» indicator'...
            }
        });
        payment.join(); withdrawal.join();

        //  ♫And what have you got at the end of the day?
        //   What have you got to take away?♫
        std::cout << std::format("\nShould be:\t${}.00 (= ${}.00 - ${}.00)",
                                 payments - withdrawals, payments, withdrawals)
                  << std::format("\nActual balance:\t${}.00\n", account_balance.load());
    }
    // Atomic C's allotropes
    // Not a buckminsterfullerene (a.k.a. a buckyball) in a full swing yet... 
    {
        using namespace std::literals;
        auto account_balance = std::atomic(0);
        auto [payments, withdrawals] = std::tuple(0, 0);

        // "Once upon a time..."
        // In a bank...
        auto asking_for_trouble = true;
        auto payment = std::thread([&]()
        {
            while (payments + withdrawals < 0x100)
            {
                auto local_balance = account_balance.load();                  
                std::this_thread::sleep_for(0b10us);
                if(asking_for_trouble)                      // Monte Carlo ain't a panaceum!
                    account_balance = local_balance + 0b1; 
                else                                        // Keeping ourselves out of trouble!
                    while (!account_balance.compare_exchange_strong(local_balance, local_balance + 1)); 
                                                        
                std::cout << (++payments, '+');             // A '«progress» indicator'...
            }
        });
        // ... and in a middle of nowhere...
        auto keeping_nose_clean = !asking_for_trouble;
        auto withdrawal = std::thread([&]()
        {
            while (payments + withdrawals < 0x100)
            {
                auto local_balance = account_balance.load();
                std::this_thread::sleep_for(0b11us);
                if(keeping_nose_clean)                      // ... and the hands too!
                    while (!account_balance.compare_exchange_strong(local_balance, local_balance - 1)); 
                else
                    account_balance = local_balance - 0b1;  // Looking for trouble!
                
                std::cout << (++withdrawals, '-');          // A '«regress» indicator'...
            }
        });
        payment.join(); withdrawal.join();
        //  ♫And what have you got at the end of the day?
        //   What have you got to take away?♫
        std::cout << std::format("\nShould be:\t${}.00 (= ${}.00 - ${}.00)",
                                 payments - withdrawals, payments, withdrawals)
                  << std::format("\nActual balance:\t${}.00\n", account_balance.load());
    }
    return 0;
}   // CMD: for /L %n in (1, 1, 10) do @echo Day #%n in a bank... & @AtomicLoopInLoupe.exe