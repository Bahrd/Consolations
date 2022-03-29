import std.core;
import std.threading;
/*
 * "The art of programming is the art of organizing complexity,
 * of mastering multitude and avoiding its bastard chaos as effectively as possible."
 * Edsger W Dijkstra, "Notes On Structured Programming", 1970
 * https://quotablemath.blogspot.com/2021/08/out-of-context-programming-is.html
 */
using namespace std::literals;
int main()
{
    {
        // A binary semaphore...
        auto promise = std::promise<void>();
        auto first_shift = std::jthread([&]
        {
            std::cout << "[Our shift] starts...\n";
            std::this_thread::sleep_for(1s);
            std::cout << "[Our shift] ends...\n";
            // There is a kind of a mutual exclusion area here...
            promise.set_value();
        });
        auto second_shift = std::jthread([&]
        {
            try
            {
                auto future = promise.get_future();
                // Here a consumer is trying to acquire a semaphore.
                // A 'future.get()' also works...
                std::cout << (future.wait(), "[New shift] has just got a seal of approval!\n");
            }
            catch (std::exception& e)
            {
                std::cerr << "[New shift] an error occured: " << e.what() << std::endl;
            }
        });
    }
    {
        //A pair of the bona fide semaphores (in action)!
        std::binary_semaphore green_card(0),
                              invoice(0);

        // A worker is going to work hard(ly?)...
        auto worker = std::jthread([&green_card, &invoice]()
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

        // A principal's principle is to watch...
        auto principal = std::jthread([&green_card, &invoice]()
        {
            // A principal sends a "Ready! Set! Go!" message
            std::cout << "[Principal] Do your job!\n";
            green_card.release();

            // ... and hangs on waiting for the job to be done!
            invoice.acquire();
            std::cout << "[Principal] Here's your cheque!\n"; // ... no double-check, huh?!
        });
    }
    return 0;
}