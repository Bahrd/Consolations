import std.core;
/* A account access (i.e. an obnoxiously innocuously lookin') example of
 * how we have gained parallelism at the expense of determinism...
 * http://web.cse.msu.edu/~cse914/Readings/theProblemWithThreads-Computer06.pdf
 * Hiram Johnson would have said: 
 *      "The first casualty when 
 *       [concurrency] comes is... 
 *       [determinism]!".
 */
// CMD: for /L %n in (1, 1, 10) do @echo Day #%n in a bank... & @LoopInLoupe.exe
int main()
{
    using namespace std::literals;
    std::mutex account;
    auto account_balance = 0, payments = 0, withdrawals = 0;
    // "Once upon a time..."
    // In a bank...
    auto payment = std::thread([&]()
    {
        while (payments + withdrawals < 0x100)
        {
            //std::lock_guard<std::mutex> lock(account);
            // All subsequent operations ♫to the end of time♫ 
            // or rather 'to the end of this scope'
            // are now atomic-like...
            auto local_balance = account_balance;
            std::this_thread::sleep_for(0b10us);
            local_balance += 1, account_balance = local_balance, ++payments;

            std::cout << '+'; // A '«progress» indicator'...
        }
    });
    // ... and in a middle of nowhere...
    auto withdrawal = std::thread([&]()
    {
        while (payments + withdrawals < 0x100)
        {
            //std::lock_guard<std::mutex> lock(account);
            // Note that the access to the variables in the scope is random...
            auto local_balance = account_balance;
            std::this_thread::sleep_for(0b11us);
            local_balance -= 1, account_balance = local_balance, ++withdrawals;

            std::cout << '-'; // A '«regress» indicator'...
        }
    });
    payment.join(); withdrawal.join();    
    //  ♫And what have you got at the end of the day?
    //   What have you got to take away?♫
    std::cout << std::format("\nShould be:\t${}.00 (= ${}.00 - ${}.00)", 
                             payments - withdrawals, payments, withdrawals)
              << std::format("\nActual balance:\t${}.00\n", account_balance);
    return 0;
}