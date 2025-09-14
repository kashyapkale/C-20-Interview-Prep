#include <iostream>
#include <future>
#include <numeric>
#include <vector>
#include <chrono>

// TODO: Implement this function. It should sum all numbers from 1 to 'max_num'.
// To simulate real work, make it sleep for 2 seconds before returning the result.
long long slow_summer(long long max_num, std::string output) {
    std::cout << "      [TASK] Starting slow sum...wit output :: "<< output << std::endl;
    long long sum = 0;
    for (long long i = 1; i <= max_num; ++i) {
        sum += i;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "      [TASK] Finished slow sum." << std::endl;
    return sum;
}

int main() {
    std::cout << "[MAIN] Launching the summer task asynchronously." << std::endl;

    // Launch the task. std::async returns a std::future immediately.
    std::future<long long> sum_future = std::async(std::launch::async, slow_summer, 100000, "test");

    std::cout << "[MAIN] The task is running in the background. Main thread is not blocked." << std::endl;
    std::cout << "[MAIN] Doing other work..." << std::endl;

    // The .get() call will block here until the future has a value.
    long long result = sum_future.get();

    std::cout << "[MAIN] The calculated sum is: " << result << std::endl;
}