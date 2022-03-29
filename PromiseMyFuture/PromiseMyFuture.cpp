import std.core;
import std.threading;
/*
 "[...] computer programming is an art, because it applies accumulated knowledge 
  to the world, because it requires skill and ingenuity, and especially because 
  it produces objects of beauty." 
  Donald E Knuth, "The Art of Computer Programming", 1968
  https://quotablemath.blogspot.com/2021/08/out-of-context-programming-is.html
*/

using namespace std::literals;

// https://en.cppreference.com/w/cpp/utility/tuple/tuple
// helper function to print a tuple of any size
template<class Tuple, std::size_t N>
struct tuple_joiner {
    static void join(Tuple& t)
    {
        tuple_joiner<Tuple, N - 1>::join(t);
        std::get<N - 1>(t).join();
    }
};
template<class Tuple>
struct tuple_joiner<Tuple, 1> {
    static void join(Tuple& t)
    {
        std::get<0>(t).join();
    }
};
template<class... Args>
void join_em_all(std::tuple<Args...>& t)
{
    tuple_joiner<decltype(t), sizeof...(Args)>::join(t);
}

int main()
{
    // A blocking ATOM-ic operation... 
    // Parmenides: "Only Being exists and [...] Not-Being is not, and can never be"
    {
        auto eleatic_principle = std::atomic_bool(false);
        auto scholar = [&](auto name)
        {
            std::cout << "Waiting...\n";
            eleatic_principle.wait(false);
            std::cout << std::format("{}'s got: {}\n", name, eleatic_principle.load());
        };
        auto [philosopher, freethinker] = std::pair(std::thread(scholar, "Plato"), std::thread(scholar, "Clifford"));

        auto seeker_after_truth = std::thread([&]()
        {
            std::cout << "The truth will soon be revealed!\n";
            // Consider an ontological (or epistemological?) problem for just a second second...
            std::this_thread::sleep_for(1s);
            // ... to find a definitive answer:
            eleatic_principle = true;
            // ... and to share the good news!
            eleatic_principle.notify_all();
        });
        for (auto t : { &philosopher, &freethinker, &seeker_after_truth }) t->join();
    }
    //Producer and consumers
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
            std::cin >> (std::cout << "Any number, please: ", i);
            promise.set_value(std::format("Hello World No. {}!", i));
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
        for (auto&& p : { &producer, &consumer_A, &consumer_B}) p->join();
    }
    // A promise-based binary semaphore...
    // cf. also https://en.cppreference.com/w/cpp/thread/future
    {
        auto promise = std::promise<void>();
        auto producer = std::jthread([&]
        {
            std::cout << "Our shift starts...\n";
            std::this_thread::sleep_for(1s);
            std::cout << "Our shift ends...\n";
            // There is a kind of a mutual exclusion area here...
            promise.set_value();
        });
        auto consumer = std::jthread([&]
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
        // A bureaucratic artifact II is not necessary (there is no need for join-in-synchronization)
        // producer.join(); consumer.join();
    }
    //A pair of bona fide semaphores (in action)!
    {
        std::binary_semaphore green_card(0),
                              invoice(0);
        // A worker is going to work hard(ly?)...
        auto worker = std::jthread([&]()
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
        auto principal = std::jthread([&]()
        {
            // A principal sends a "Ready! Set! Go!" message
            std::cout << "[Principal] Do your job!\n";
            green_card.release();

            // ... and hangs on waiting for the job to be done!
            invoice.acquire();
            std::cout << "[Principal] Here's your cheque!\n"; // ... no double-check, huh?!
        });
        // A bureaucratic artifact III is not necessary here either
        //worker.join(); principal.join();
    }
    // A writer-readers problem
    {
        // https://en.cppreference.com/w/cpp/thread/condition_variable 
        // "Even if the shared variable is atomic, it must be modified under the mutex 
        // in order to correctly publish the modification to the waiting thread."
        using ulm = std::unique_lock<std::mutex>;

        std::mutex m;
        std::condition_variable cv;
        auto days = 0b1;

        auto reader = [&](std::string const& name)
        {
            // Wait until a writer writes a book...
            ulm lock(m);
            cv.wait(lock, [&] { return days > name.size(); });
            // after the 'wait', we own the lock ♫to the end of [scope]♫
            std::cout << std::format("{} read the book after {} days ...\n", name, days);
        };
        //auto readers = std::vector<std::thread>();
        //for (auto name : { "Id", "Ego", "Superego" })
        //    readers.push_back(std::move(std::thread(reader, name)));
        // Or...
        auto readers = std::make_tuple(std::thread(reader, "Id"), 
                                       std::thread(reader, "Ego"), 
                                       std::thread(reader, "Superego"));
        // A writer's thread
        {
            std::cout << "A book to be days by one should be ready to be read by (not so) many...\n";
            std::cout << (cv.notify_all(), "... not yet!\n");
            {
                ulm lock(m);
                std::cout << (days += 0b10, cv.notify_all(), "... but now!\n");
            }
            std::this_thread::sleep_for(0b1s);
            {
                ulm lock(m);
                std::cout << (days = 010, cv.notify_all(), "... and now again!\n");
            }
            std::this_thread::sleep_for(0b1s);
            {
                ulm lock(m);
                std::cout << (days += 0x10, cv.notify_all(), "... and again!\n");
            }
            std::this_thread::sleep_for(0b1s);
        }
        // Dynamic...
        // for (auto& rt: readers) rt.join();
        // Static...
        join_em_all(readers);
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