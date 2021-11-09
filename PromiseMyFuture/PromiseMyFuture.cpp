#include <atomic>
#include <condition_variable>
#include <chrono>
#include <format>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <semaphore>
#include <thread>
/*
 "[...] computer programming is an art, because it applies accumulated knowledge 
  to the world, because it requires skill and ingenuity, and especially because 
  it produces objects of beauty." 
  Donald E Knuth, "The Art of Computer Programming", 1968
  https://quotablemath.blogspot.com/2021/08/out-of-context-programming-is.html
*/
using namespace std::literals;
int main()
{
    {
        auto eleatic_principle = std::atomic_bool(false);
        auto philosopher = std::thread([&]()
        {
            std::cout << "Waiting...\n";
            eleatic_principle.wait(false);
            std::cout << std::format("Plato's got: {}\n", eleatic_principle.load());
        });
        auto freethinker = std::thread([&]()
        {
            std::cout << "Waiting...\n";
            eleatic_principle.wait(false);
            std::cout << std::format("Clifford's got: {}\n", eleatic_principle.load());
        });

        auto seeker_after_truth = std::thread([&]()
        {
            // Consider an ontological (or epistemological?) problem for just a second...
            std::cout << "The truth will soon be revealed!\n";
            std::this_thread::sleep_for(1s);
            // ... to find a definitive answer:
            eleatic_principle = true;
            // ... and to share the good news!
            eleatic_principle.notify_all();
        });
        philosopher.join(); freethinker.join();
        seeker_after_truth.join();
    }
    {
        // A Producer's "PUSH" that also defines the return value type
        auto promise = std::promise<std::string>();
        auto producer = std::thread([&]
            {
                // Here the magic happens (and it should not be too trivial
                // if one cries concurency for assistance...)
                // ...
                // 'set_value' is a signal that the promise is fulfilled 
                // and the result is ready to be consumed.
                // In the stack parlance it is just a return value.
                int i;
                std::cin  >> i;
                promise.set_value("Hello World!");
            });

        // A Consumers' "POP" is here!
        // A single consumer (whoever he|she|they is|are first) 
        // snatches the result using 'future.get()'.
        // By virtue of ♫The winner takes it all!♫ rule, 
        // the other(s) had to leave (exceptionally) empty handed... 
        auto consumer_A = std::thread([&]
            {
                try
                {
                    // This consumer is a bit impatient...
                    std::cout << "I only have about three seconds to spare...\n";
                    auto future = promise.get_future();
                    auto result = future.wait_for(3s) == std::future_status::timeout
                                  ? "Too late, too late..."
                                  : future.get();

                    std::cout << result << "\n--\nConsumer (an impatient one)!\n";
                }
                catch (std::exception& e)
                {
                    std::cerr << "Impatient consumer error...: " << e.what() << std::endl;
                }
            });
        auto consumer_B = std::thread([&]
            {
                try
                {
                    // This one has plenty of time on his hands...
                    std::cout << "'Langsam, aber sicher' is my motto...\n";
                    auto future = promise.get_future();
                    std::cout << future.get() << "\n--\nConsumer (a phlegmatic one)!\n";
                }
                catch (std::exception& e)
                {
                    std::cerr << "Phlegmatic consumer error...: " << e.what() << std::endl;
                }
            });
        // A bureaucratic artifact (from Honoré de Balzac to Vercingetorix (a.k.a. Asterix))
        producer.join(); 
        consumer_A.join(); consumer_B.join();
    }
    {
        // A binary semaphore...
        auto promise = std::promise<void>();
        auto producer = std::thread([&]
            {
                std::cout << "Our shift starts...\n";
                std::this_thread::sleep_for(1s);
                std::cout << "Our shift ends...\n";
                // There is a kind of a mutual exclusion area here...
                promise.set_value();
            });
        auto consumer = std::thread([&]
            {
                try
                {
                    auto future = promise.get_future();
                    // Here a consumer is trying to acquire a semaphore.
                    // A 'future.get()' also works...
                    std::cout << (future.wait(), "A new shift has just got a seal of approval!\n"); 
                }
                catch (std::exception& e)
                {
                    std::cerr << "Consumer C error...: " << e.what() << std::endl;
                }
            });
        // A bureaucratic artifact II
        producer.join(); consumer.join();
    }
    {
        //A pair of bona fide semaphores (in action)!
        std::binary_semaphore green_card(0),
                              invoice(0);

        // A worker is going to work hard(ly?)...
        auto worker = std::thread([&]()
            {
                // ... and thus (badly) needs a permission from his principal...
                green_card.acquire();
                std::cout << "[Worker] Got a green light!\n";

                // ... then waits for a while (as if an intensive effort has been made)...
                std::this_thread::sleep_for(1s);
                // ... and then immediately calls it a day!
                std::cout << "[Worker] Job's done!\n";
                invoice.release();
            });

        // A principle is going to watch...
        auto principal = std::thread([&]()
            {
                // A principal sends a "Ready! Set! Go!" message
                std::cout << "[Principal] Do your job!\n";
                green_card.release();

                // ... and hangs on waiting for the job to be done!
                invoice.acquire();
                std::cout << "[Principal] Here's your cheque!\n"; // ... no double-check, huh?!
            });

        // A bureaucratic artifact III
        worker.join(); principal.join();
    }
    {
        // A account access (i.e. an obnoxiously innocuously lookin') example 
        auto account_balance = 0, payments = 0, withdrawals = 0;
        std::mutex teller;

        while (payments + withdrawals < 0x100)
        {
            // Here, in the bank...
            auto payment = std::thread([&]()
                {
                    std::lock_guard<std::mutex> lock(teller);
                    // All subsequent operations (to the end of the scope) 
                    // are now atomic-like...
                    std::this_thread::sleep_for(10ms);
                    account_balance += 10;
                    ++payments;
                });
            // ... and in the middle of nowhere...
            auto withdrawal = std::thread([&]()
                {
                    std::lock_guard<std::mutex> lock(teller);
                    // Note that the access to the variables in the 
                    // scope is random...
                    std::this_thread::sleep_for(10ms);
                    account_balance -= 10;
                    ++withdrawals;
                });
            payment.join(); withdrawal.join();
        }
        std::cout << std::format("Should be:\t${}.00\n", (payments - withdrawals) * 10)
                  << std::format("Actual balance:\t${}.00\n", account_balance);
    }
    {
        // A writer-readers problem
        std::mutex m;
        std::condition_variable cv;
        auto written = false;

        auto reader_thread = [&](std::string const &name)
        {
            // Wait until a writer writes a book...
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [&] { return written; });
            // after the 'wait', we own the lock ♫to the end of [scope]♫
            std::cout << std::format("{}'s just read the book...", name);
        };

        std::thread reader_I(reader_thread, "Id"),
                    reader_II(reader_thread, "Ego"),
                    reader_III(reader_thread, "Superego");
        {
            std::lock_guard<std::mutex> lock(m);
            std::cout << "A book to be written by one should be ready to be read by many...\n";
            written = true;
            std::this_thread::sleep_for(2s);
            std::cout << "... right now!\n";
        }
        cv.notify_all();
        reader_I.join(); reader_II.join(); reader_III.join();
    }
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
            
            auto mid = beg + len/0b10;
            auto launch_mode = std::launch::deferred | std::launch::async;
            auto async = std::async(launch_mode, pms, mid, end);
            return pms(beg, mid) + async.get();
        };
        // An asynchronous parallel anonymous example recursive invocation...
        std::cout << "\nAnonymously asynchronous summa = " 
                  << pms(begin(v), end(v)) << '\n';
    }
    return 0;
}
/**
What makes a freethinker is not his beliefs but the way in which he holds them.
If he holds them because his elders told him they were true when he was young,
or if he holds them because if he did not he would be unhappy,
his thought is not free; but if he holds them because,
after careful thought he finds a balance of evidence in their favour,
then his thought is free, however odd his conclusions may seem.
— 
Bertrand Russell, The Value of Free Thought.
How to Become a Truth-Seeker and Break the Chains of Mental Slavery
 */