Here is a C++20 cheat sheet covering the essential topics for a demanding technical interview.

### \#\# ⚡ Concurrency & Multithreading

#### **Atomics & Lock-Free Programming**

Used for high-performance, non-blocking synchronization between threads.

  * **`std::atomic<T>`**: Guarantees that operations are indivisible. Essential for lock-free data structures. The `compare_exchange_strong` function is a common pattern for safe, conditional updates.
    ```cpp
    std::atomic<int> val{5};
    int expected = 5;
    // Atomically change val to 10 ONLY if its current value is 5.
    // If not, 'expected' is updated with the current value of val.
    val.compare_exchange_strong(expected, 10);
    ```
  * **Memory Ordering**: Controls the visibility of memory operations between threads. The **Acquire-Release** semantic is the most common pattern for signaling.
    ```cpp
    std::atomic<bool> is_ready{false};
    int data = 0;

    // Thread A (Producer)
    data = 42;
    // Release store: Ensures 'data = 42' is visible to any thread
    // that performs an acquire-load on 'is_ready'.
    is_ready.store(true, std::memory_order_release);

    // Thread B (Consumer)
    // Acquire load: Ensures that if 'is_ready' is true,
    // the read of 'data' will see 42.
    if (is_ready.load(std::memory_order_acquire)) {
      // Guaranteed to see data = 42
    }
    ```

-----

#### **C++20 Primitives**

  * **`std::jthread`**: A safer version of `std::thread` that automatically joins upon destruction (RAII). It also supports cooperative cancellation via a `std::stop_token`.
    ```cpp
    void task(std::stop_token token) {
      while (!token.stop_requested()) { /* do work */ }
    }
    // main()
    std::jthread worker(task);
    // 'worker' destructor automatically requests stop and then joins. No manual cleanup needed.
    ```
  * **`std::counting_semaphore`**: Limits concurrent access to a pool of N resources.
    ```cpp
    // Allow up to 4 concurrent accesses.
    std::counting_semaphore semaphore(4);

    void use_limited_resource() {
      semaphore.acquire(); // Blocks if count is 0, else decrements.
      // ... critical section ...
      semaphore.release(); // Increments count, potentially unblocking a waiting thread.
    }
    ```
  * **`std::latch`**: A single-use barrier to synchronize a group of threads at a specific point.
    ```cpp
    std::latch start_gate(3); // Waits for 3 threads to count down.

    void worker() {
      // ... do setup work ...
      start_gate.count_down(); // Signal readiness.
      start_gate.wait();       // Block until all 3 have counted down.
      // ... all workers proceed from here simultaneously ...
    }
    ```

-----

#### **Core Synchronization**

  * **`std::mutex` & `std::scoped_lock`**: The standard tools for protecting shared data. `std::scoped_lock` provides RAII safety and deadlock avoidance for one or more mutexes.
    ```cpp
    std::mutex mtx1, mtx2;
    void critical_section() {
      // Safely locks both mutexes; automatically unlocks on scope exit.
      std::scoped_lock lock(mtx1, mtx2);
      // ... modify data protected by mtx1 and mtx2 ...
    }
    ```
  * **`std::condition_variable`**: Allows threads to wait efficiently for a specific condition to become true.
    ```cpp
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    // Waiting thread
    std::unique_lock lock(mtx);
    cv.wait(lock, []{ return ready; }); // Atomically unlocks, waits, and re-locks.

    // Notifying thread
    {
      std::lock_guard lock(mtx);
      ready = true;
    }
    cv.notify_one();
    ```

-----

#### **Asynchronous Tasks**

  * **`std::async` & `std::future`**: A high-level way to run a function that returns a value in the background.
    ```cpp
    std::future<int> result_future = std::async(std::launch::async, [](){
      return 10 * 10;
    });
    // .get() blocks until the task is complete and returns the value.
    int result = result_future.get(); // result is 100
    ```

-----

### \#\# 🧠 Memory Management

#### **RAII & Smart Pointers**

**RAII (Resource Acquisition Is Initialization)** is the core C++ principle of tying a resource's lifetime to an object's scope. Smart pointers are the primary tool for this.

  * **`std::unique_ptr`**: Exclusive, lightweight ownership. This should be your default choice. It has no performance overhead compared to a raw pointer.
    ```cpp
    // Create a unique_ptr.
    std::unique_ptr<int> p1 = std::make_unique<int>(42);
    // Transfer ownership. p1 is now null.
    std::unique_ptr<int> p2 = std::move(p1);
    ```
  * **`std::shared_ptr`**: Shared ownership with reference counting. Use when multiple objects need to co-own a resource.
    ```cpp
    std::shared_ptr<int> s1 = std::make_shared<int>(100);
    std::shared_ptr<int> s2 = s1; // Both pointers own the object. Ref count is 2.
    ```

-----

#### **Move & Copy Semantics (Rule of 5/0)**

  * **Move Semantics**: Avoid expensive copies by "stealing" resources from temporary objects. `std::move` signals that an object can be moved from.
    ```cpp
    class Data {
      int* buffer_;
    public:
      // Move Constructor: Steals the pointer from 'other'.
      Data(Data&& other) noexcept : buffer_(other.buffer_) {
        other.buffer_ = nullptr; // Leave the source in a valid, empty state.
      }
    };
    ```
  * **The Rule of Five/Zero**: If you define any of the five special member functions (destructor, copy/move constructor, copy/move assignment), you should consider all five. The **Rule of Zero** is the modern goal: by using smart pointers and standard containers, you often don't need to write any of them yourself.

-----

### \#\# 🚀 Generics & Modern Features

#### **Templates**

Templates enable you to write generic, type-safe, high-performance code by generating type-specific versions at compile time. This avoids the runtime overhead of `virtual` functions.

```cpp
template<typename T>
T add(T a, T b) {
  return a + b;
}
// The compiler generates add(int, int) and add(double, double) automatically.
auto sum1 = add(5, 10);
auto sum2 = add(3.14, 2.71);
```

-----

#### **C++20 Ranges**

A modern, composable way to work with sequences. Operations are chained with the pipe (`|`) operator, and "views" provide lazy, non-allocating adaptors.

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5, 6};
auto results = nums
             | std::views::filter([](int n){ return n % 2 == 0; })
             | std::views::transform([](int n){ return n * n; });
// No work is done until 'results' is iterated. No intermediate vectors are created.
```

-----

#### **C++20 Spaceship Operator (`<=>`)**

Automatically generates all six comparison operators (`<`, `>`, `==`, etc.) in the correct, member-wise order.

```cpp
struct Point {
  int x;
  int y;
  // Let the compiler generate all comparisons based on member order (x then y).
  auto operator<=>(const Point&) const = default;
};
```

-----

### \#\# 📊 Core Data Structures & Algorithms

#### **STL Container Performance**

| Container | Key Strength | Time Complexity (Avg.) | When to Use |
| :--- | :--- | :--- | :--- |
| **`std::vector`** | Contiguous, cache-friendly | Access: $O(1)$ | **Default container**. Excellent performance due to cache locality. |
| **`std::unordered_map`**| Fastest key-value lookups | Access/Insert: $O(1)$ | **Default map**. Fastest for lookups where order is not needed. |
| **`std::map`**| Sorted key-value pairs | Access/Insert: $O(\\log N)$ | When data must be sorted by key (e.g., an order book). |

-----

#### **Streaming Large Data**

Process files line-by-line to handle data that doesn't fit in memory.

```cpp
std::ifstream file("huge_file.log");
std::string line;
while (std::getline(file, line)) {
  // Process one line at a time with minimal memory usage.
}
```
