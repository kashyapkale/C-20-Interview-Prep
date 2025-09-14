#include <iostream>
#include <thread>
#include <chrono>

void counter_task(std::stop_token token) {
    int count = 0;
    while (true) {
        // TODO: Check if a stop has been requested on the token.
        // If it has, print a shutdown message and break the loop.
        if (token.stop_requested()) {
            std::cout << "      Stop requested. Shutting down counter." << std::endl;
            break;
        }

        std::cout << "      Counter is at: " << ++count << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::cout << "[MAIN] Starting counter task." << std::endl;
    
    // Create the jthread. It starts running immediately.
    std::jthread worker(counter_task);

    std::cout << "[MAIN] Doing other work for 3 seconds." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    worker.request_stop();
    
    std::cout << "[MAIN] Doing more other work for 3 seconds." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[MAIN] Main function is ending. jthread will now be destroyed." << std::endl;
    // The destructor of 'worker' is called here.
    // It will automatically request the stop and join the thread.
}