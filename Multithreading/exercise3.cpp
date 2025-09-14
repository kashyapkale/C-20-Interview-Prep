#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>

// A semaphore that allows only 2 threads to access the resource at a time.
std::counting_semaphore resource_semaphore(2);

void access_resource(int thread_id) {
    std::cout << "  [Thread " << thread_id << "] Waiting to access resource..." << std::endl;
    
    // TODO: Acquire a permit from the semaphore. This will block if 2 are already in use.
    resource_semaphore.acquire();
    
    std::cout << "  -> [Thread " << thread_id << "] ACCESS GRANTED. Working..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "  <- [Thread " << thread_id << "] FINISHED. Releasing resource." << std::endl;

    // TODO: Release the permit back to the semaphore.
    resource_semaphore.release();
}

int main() {
    std::cout << "[MAIN] Launching 5 threads to compete for 2 resources." << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(access_resource, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}