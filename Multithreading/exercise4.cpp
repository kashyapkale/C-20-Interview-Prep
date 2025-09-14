#include <iostream>
#include <thread>
#include <vector>
#include <latch>

// A latch that will wait for 3 racers + 1 main thread signal.
std::latch race_latch(4);

void racer(int id) {
    std::cout << "  [Racer " << id << "] Preparing..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(id * 100)); // Staggered prep time
    std::cout << "  [Racer " << id << "] Ready." << std::endl;
    
    // TODO: Decrement the latch and wait for it to reach zero.
    race_latch.count_down();
    race_latch.wait();
    
    std::cout << "  -> [Racer " << id << "] GO!" << std::endl;
}

int main() {
    std::cout << "[MAIN] On your marks..." << std::endl;
    std::vector<std::thread> racers;
    for (int i = 1; i < 4; ++i) {
        racers.emplace_back(racer, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[MAIN] Get set..." << std::endl;

    // TODO: Decrement the latch from the main thread to release the racers.
    race_latch.count_down();

    for (auto& r : racers) {
        r.join();
    }
}