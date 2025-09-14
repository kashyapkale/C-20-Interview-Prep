-----

### \#\# Exercise 1: `std::jthread` and Cooperative Cancellation

**Goal**: Understand how `std::jthread` automatically joins and how to use its `std::stop_token` for a clean shutdown.

**Your Task**: Complete the `counter_task` function. It should print the current count every second but stop gracefully when a stop is requested.

```cpp
#include <iostream>
#include <thread>
#include <jthread> // Header for jthread
#include <chrono>

void counter_task(std::stop_token token) {
    int count = 0;
    while (true) {
        // TODO: Check if a stop has been requested on the token.
        // If it has, print a shutdown message and break the loop.
    }
}

int main() {
    std::cout << "[MAIN] Starting counter task." << std::endl;
    
    // Create the jthread. It starts running immediately.
    std::jthread worker(counter_task);

    std::cout << "[MAIN] Doing other work for 3 seconds." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[MAIN] Main function is ending. jthread will now be destroyed." << std::endl;
    // The destructor of 'worker' is called here.
    // It will automatically request the stop and join the thread.
}
```

**✅ Expected Output:**

```text
[MAIN] Doing other work for 3 seconds.
      Counter is at: 1
      Counter is at: 2
      Counter is at: 3
[MAIN] Doing more other work for 3 seconds.
      Stop requested. Shutting down counter.
[MAIN] Main function is ending. jthread will now be destroyed.
```

-----

### \#\# Exercise 2: `std::async` and `std::future`

**Goal**: Learn how to run a function asynchronously and get its return value later without manually managing a `std::promise`.

**Your Task**: Write a function that slowly computes a sum and use `std::async` to run it in the background.

```cpp
#include <iostream>
#include <future>
#include <numeric>
#include <vector>
#include <chrono>

// To simulate real work, make it sleep for 2 seconds before returning the result.
long long slow_summer(long long max_num) {
    std::cout << "      [TASK] Starting slow sum..." << std::endl;
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

    //TODO :: Launch the task. std::async returns a std::future immediately.

    std::cout << "[MAIN] The task is running in the background. Main thread is not blocked." << std::endl;
    std::cout << "[MAIN] Doing other work..." << std::endl;

    // TODO :: The .get() call will block here until the future has a value.

    std::cout << "[MAIN] The calculated sum is: " << result << std::endl;
}
```

**✅ Expected Output:**

```text
[MAIN] Launching the summer task asynchronously.
      [TASK] Starting slow sum...
[MAIN] The task is running in the background. Main thread is not blocked.
[MAIN] Doing other work...
      [TASK] Finished slow sum.
[MAIN] The calculated sum is: {any number that returns from fun}
```

-----

### \#\# Exercise 3: `std::counting_semaphore`

**Goal**: Use a semaphore to limit the number of threads concurrently accessing a "resource."

**Your Task**: Complete the `access_resource` function. It must acquire the semaphore before "using" the resource and release it after.

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>

// TODO :: A semaphore that allows only 2 threads to access the resource at a time.

void access_resource(int thread_id) {
    std::cout << "  [Thread " << thread_id << "] Waiting to access resource..." << std::endl;
    
    // TODO: Acquire a permit from the semaphore. This will block if 2 are already in use.
    
    std::cout << "  -> [Thread " << thread_id << "] ACCESS GRANTED. Working..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "  <- [Thread " << thread_id << "] FINISHED. Releasing resource." << std::endl;

    // TODO: Release the permit back to the semaphore.
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
```

**✅ Expected Output** (Order of threads may vary, but only two should be "ACCESS GRANTED" at a time):

```text
[MAIN] Launching 5 threads to compete for 2 resources.
  [Thread 0] Waiting to access resource...
  -> [Thread 0] ACCESS GRANTED. Working...
  [Thread 1] Waiting to access resource...
  -> [Thread 1] ACCESS GRANTED. Working...
  [Thread 2] Waiting to access resource...
  [Thread 3] Waiting to access resource...
  [Thread 4] Waiting to access resource...
  <- [Thread 0] FINISHED. Releasing resource.
  -> [Thread 2] ACCESS GRANTED. Working...
  <- [Thread 1] FINISHED. Releasing resource.
  -> [Thread 3] ACCESS GRANTED. Working...
  <- [Thread 2] FINISHED. Releasing resource.
  -> [Thread 4] ACCESS GRANTED. Working...
  <- [Thread 3] FINISHED. Releasing resource.
  <- [Thread 4] FINISHED. Releasing resource.
```

-----

### \#\# Exercise 4: `std::latch`

**Goal**: Synchronize a group of threads so they all start a task at the exact same time.

**Your Task**: Have each thread perform "prep work" and then wait on a latch. The main thread will release the latch, starting the "race."

```cpp
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
    
    std::cout << "  -> [Racer " << id << "] GO!" << std::endl;
}

int main() {
    std::cout << "[MAIN] On your marks..." << std::endl;
    std::vector<std::thread> racers;
    for (int i = 0; i < 3; ++i) {
        racers.emplace_back(racer, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[MAIN] Get set..." << std::endl;

    // TODO: Decrement the latch from the main thread to release the racers.

    for (auto& r : racers) {
        r.join();
    }
}
```

**✅ Expected Output** (The "GO\!" messages should appear at almost the exact same time):

```text
[MAIN] On your marks...
  [Racer 1] Preparing...
  [Racer 1] Ready.
  [Racer 2] Preparing...
  [Racer 3] Preparing...
  [Racer 2] Ready.
  [Racer 3] Ready.
[MAIN] Get set...
  -> [Racer 1] GO!
  -> [Racer 2] GO!
  -> [Racer 3] GO!
```

-----

### \#\# Final Challenge: The Combined Exercise

This exercise is designed to be a comprehensive challenge that integrates the C++20 concurrency features into a single, practical problem.

-----

### \#\# The Exercise: Concurrent Data Processing Pipeline

You need to build a system that processes telemetry data. The system has a limited number of "processing licenses," so only a few data packets can be processed at the same time. The pipeline must be efficient, thread-safe, and shut down cleanly.

**The Pipeline Stages:**

1.  **Submission:** The main thread submits raw data packets to a queue.
2.  **Coordination:** All worker threads must wait for a "start" signal before they begin processing any data.
3.  **Processing:** A pool of worker threads grabs data from the queue. To process a packet, a worker must first acquire a "processing license."
4.  **Aggregation:** The main thread must be able to get the results of the processing back to calculate a final sum.

-----

### \#\# 📝 Your Task

Complete the `TelemetryProcessor` class below. You will need to use the C++ concurrency tools we've discussed to make it work correctly.

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <jthread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <semaphore>
#include <latch>
#include <functional>
#include <numeric>

// GIVEN CODE
struct RawData {
    int id;
    double value;
};
struct ProcessedData {
    int source_id;
    double processed_value;
};
ProcessedData process_data(const RawData& data) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return {data.id, data.value * 1.5};
}
using Task = std::pair<RawData, std::promise<ProcessedData>>;

class TelemetryProcessor {
private:
    const int num_threads_;
    const int max_licenses_;

    // TODO 1: Declare all necessary member variables for:
    // - Storing worker threads (hint: std::vector of jthreads)
    // - A task queue (hint: std::queue of Task)
    // - A mutex for the queue
    // - A condition variable for the queue
    // - A semaphore for managing licenses
    // - A latch for starting the workers simultaneously
    /* YOUR CODE HERE */

public:
    TelemetryProcessor(int num_threads, int max_licenses)
        : num_threads_(num_threads),
          max_licenses_(max_licenses)
          // TODO 2: Initialize your synchronization member variables in the initializer list.
          // The semaphore should be initialized with `max_licenses`.
          // The latch should be initialized to wait for one signal.
    {
        // TODO 3: Create and start the worker threads, storing them in your member variable.
        // Each thread should run the `worker_loop` function.
        /* YOUR CODE HERE */
    }

    void worker_loop(std::stop_token st) {
        // TODO 4: Wait for the start signal using your latch.
        /* YOUR CODE HERE */
        
        while (!st.stop_requested()) {
            // TODO 5: Acquire a license from your semaphore.
            /* YOUR CODE HERE */

            std::unique_lock<std::mutex> lock(queue_mutex_); // Note: queue_mutex_ will need to be your declared mutex
            // TODO 6: Wait on your condition variable until the queue has a task OR a stop is requested.
            /* YOUR CODE HERE */
            
            if (st.stop_requested()) {
                license_semaphore_.release(); // Note: license_semaphore_ will need to be your declared semaphore
                break;
            }

            Task current_task = std::move(task_queue_.front()); // Note: task_queue_ will be your declared queue
            task_queue_.pop();
            lock.unlock();

            ProcessedData result = process_data(current_task.first);
            
            // TODO 7: Fulfill the promise.
            /* YOUR CODE HERE */

            // TODO 8: Release the license back to your semaphore.
            /* YOUR CODE HERE */
        }
    }

    std::future<ProcessedData> submit(RawData data) {
        // TODO 9: Create a promise, get its future, push the task to your queue, and notify one waiting thread.
        /* YOUR CODE HERE */
    }

    void start_processing() {
        // TODO 10: Signal the latch to start all workers.
        /* YOUR CODE HERE */
    }

    ~TelemetryProcessor() {
        // TODO 11: Request stop on all threads and notify all waiting threads.
        /* YOUR CODE HERE */
    }
};

// Main function remains the same for testing.
int main() {
    const int NUM_JOBS = 10;
    const int NUM_THREADS = 4;
    const int NUM_LICENSES = 2;

    TelemetryProcessor processor(NUM_THREADS, NUM_LICENSES);

    std::vector<std::future<ProcessedData>> futures;
    for (int i = 0; i < NUM_JOBS; ++i) {
        futures.push_back(processor.submit({i, static_cast<double>(i)}));
    }
    
    processor.start_processing();

    double total_sum = 0;
    for (auto& f : futures) {
        total_sum += f.get().processed_value;
    }
    
    std::cout << "Final sum: " << total_sum << std::endl;
    return 0;
}
```

-----

### \#\# ✅ Expected Output

Your output will vary slightly due to thread scheduling, but it should look very similar to this. The key things to look for are:

  * All workers wait for the "start" signal.
  * The processing messages are interleaved, but there are never more than `NUM_LICENSES` (2) processing at one time.
  * The main thread collects results as they become available.
  * All workers shut down cleanly at the end.

<!-- end list -->

```text
[MAIN] Initializing Telemetry Processor with 4 threads and 2 licenses.
[MAIN] Submitted 10 jobs.
[MAIN] Signaling workers to start.
Worker 139934164809280 started processing.
Worker 139934173201984 started processing.
Worker 139934148025920 started processing.
Worker 139934156418624 started processing.
    Worker 139934164809280 processed data ID 0
    Worker 139934173201984 processed data ID 1
[MAIN] Got result for ID 0, value 0
    Worker 139934164809280 processed data ID 2
[MAIN] Got result for ID 1, value 1.5
    Worker 139934173201984 processed data ID 3
[MAIN] Got result for ID 2, value 3
    Worker 139934164809280 processed data ID 4
[MAIN] Got result for ID 3, value 4.5
    Worker 139934173201984 processed data ID 5
[MAIN] Got result for ID 4, value 6
    Worker 139934164809280 processed data ID 6
[MAIN] Got result for ID 5, value 7.5
    Worker 139934173201984 processed data ID 7
[MAIN] Got result for ID 6, value 9
    Worker 139934164809280 processed data ID 8
[MAIN] Got result for ID 7, value 10.5
    Worker 139934173201984 processed data ID 9
[MAIN] Got result for ID 8, value 12
[MAIN] Got result for ID 9, value 13.5
[MAIN] All jobs complete.
[MAIN] Final sum: 67.5
[MAIN] Shutting down.
Worker 139934164809280 shutting down.
Worker 139934148025920 shutting down.
Worker 139934173201984 shutting down.
Worker 139934156418624 shutting down.
```

-----

### \#\# 💡 Concepts Tested

  * **Thread Management (`std::jthread`)**: Creating a pool of threads that are automatically joined upon destruction.
  * **Cooperative Cancellation (`std::stop_token`)**: Gracefully shutting down the worker threads when requested.
  * **Task Queue (`std::queue`, `std::mutex`, `std::condition_variable`)**: The classic thread-safe producer-consumer queue pattern.
  * **Asynchronous Results (`std::promise`, `std::future`)**: Decoupling the submission of a task from the retrieval of its result.
  * **Resource Limiting (`std::counting_semaphore`)**: Ensuring no more than a fixed number of threads can perform the "work" simultaneously.
  * **Group Synchronization (`std::latch`)**: Making sure all threads wait for a common starting signal before proceeding.