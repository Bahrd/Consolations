#include <format>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

/* A account access (i.e. an obnoxiously innocuously lookin') example of
 * how we have gained parallelism at the expense of determinism...
 * http://web.cse.msu.edu/~cse914/Readings/theProblemWithThreads-Computer06.pdf
 * Hiram Johnson would have said: "The first casualty when [concurrency] comes is [determinism]".
 */
int main()
{
    using namespace std::literals;
    static std::mutex account;
    auto account_balance = 0, payments = 0, withdrawals = 0;

    // "Once upon a time..."
    // In a bank...
    auto payment = std::thread([&]()
    {
        while (payments + withdrawals < 100)
        {
            std::lock_guard<std::mutex> lock(account);
            // All subsequent operations  
            // ♫to the end of time♫ - or of this scope are now atomic-like...
            std::this_thread::sleep_for(0b100ms);
            ++payments, account_balance += 1;

            std::cout << '+'; // A '«progress» indicator'...
        }
    });
    // ... and in a middle of nowhere...
    auto withdrawal = std::thread([&]()
    {
        while (payments + withdrawals < 100)
        {
            std::lock_guard<std::mutex> lock(account);
            // Note that the access to the variables in the scope is random...
            std::this_thread::sleep_for(0b100ms);
            account_balance -= 1, ++withdrawals;

            std::cout << '-'; // A '«regress» indicator'...
        }
    });
    payment.join(); withdrawal.join();
    //  ♫And what have you got at the end of the day?
    //   What have you got to take away?♫
    std::cout << std::format("\nShould be:\t${}.00 (= ${}.00 - ${}.00)", 
                             payments - withdrawals, payments, withdrawals)
              << std::format("\nActual balance:\t${}.00", account_balance);
    return 0;
}