#include <format>
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
    // Atomic H₂
    {
        using namespace std::literals;
        auto account_balance = std::atomic(0);
        auto payments = 0, withdrawals = 0;

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
                    --account_balance; ++withdrawals;
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
    // Atomic H₂0
    {
        using namespace std::literals;
        auto account_balance = std::atomic(0);
        auto local_balance = 0, payments = 0, withdrawals = 0;

        // "Once upon a time..."
        // In a bank...
        auto payment = std::thread([&]()
            {
                while (payments + withdrawals < 0x100)
                {
                    local_balance = account_balance;                  
                    std::this_thread::sleep_for(0b10us);
                    //while (!account_balance.compare_exchange_strong(local_balance, local_balance + 1)); 
                                                            // Keeping out of trouble!
                    account_balance = local_balance + 0b1;  // Asking for trouble!
                    std::cout << (++payments, '+');         // A '«progress» indicator'...
                }
            });
        // ... and in a middle of nowhere...
        auto withdrawal = std::thread([&]()
            {
                while (payments + withdrawals < 0x100)
                {
                    local_balance = account_balance;                    
                    std::this_thread::sleep_for(0b11us);
                    //while (!account_balance.compare_exchange_strong(local_balance, local_balance - 1)); 
                                                            // Keeping nose clean!
                    account_balance = local_balance - 0b1;  // Looking for trouble!
                    std::cout << (++withdrawals, '-');      // A '«regress» indicator'...
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