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

### The Core Problem: Why Smart Pointers?

In C++, when you allocate memory on the heap with `new`, you are responsible for freeing it with `delete`. Forgetting to do this causes a **memory leak**. If you `delete` the memory but still have a pointer to it, you create a **dangling pointer**, which can lead to crashes if used.

Smart pointers solve this by automating memory management using a core C++ principle called **RAII**.

-----

### The Guiding Principle: RAII (Resource Acquisition Is Initialization) 📚

RAII is a fundamental concept in C++. It means that the lifetime of a resource (like heap memory, a file handle, or a network socket) is tied to the lifetime of an object on the stack.

  * **Acquisition:** You acquire the resource in the object's constructor.
  * **Release:** You release the resource in the object's destructor.

Since C++ guarantees that destructors are called when an object goes out of scope, the resource is **automatically and safely cleaned up**. Smart pointers are essentially lightweight wrapper objects that hold a raw pointer and apply the RAII principle to it.

-----

### 1\. `std::unique_ptr` — The Exclusive Owner 🔑

A `std::unique_ptr` represents **exclusive, unique ownership** of a resource. Think of it as having the only physical key to a house. Only one `unique_ptr` can point to an object at any given time.

**How it Works:**
It's a very simple, lightweight wrapper. When the `unique_ptr` object is destroyed (e.g., it goes out of scope), its destructor automatically calls `delete` on the raw pointer it holds.

**Key Feature: Move Semantics**
You **cannot copy** a `unique_ptr` because that would violate exclusive ownership (you can't duplicate a physical key).

```cpp
std::unique_ptr<MyObject> ptr1 = std::make_unique<MyObject>();
// std::unique_ptr<MyObject> ptr2 = ptr1; // ERROR: This will not compile!
```

However, you can **move** it, which transfers ownership. The original pointer is then set to `nullptr`.

```cpp
std::unique_ptr<MyObject> ptr1 = std::make_unique<MyObject>();
std::unique_ptr<MyObject> ptr2 = std::move(ptr1); // This is fine!

// Now, ptr2 owns the object, and ptr1 is nullptr.
```

**When to Use It:**

  * **This should be your default choice.** Use it whenever you need a pointer to a heap-allocated object and don't need to share ownership.
  * **Factory functions:** A function that creates an object and returns a pointer to it should return a `unique_ptr`. This makes it clear to the caller that they now own the object and are responsible for its lifetime.
  * Implementing the **PIMPL (Pointer to Implementation) idiom**.

**Thread Safety:**
Modifying the `unique_ptr` object itself is not thread-safe (e.g., two threads calling `reset()` on it). However, the object it *points to* can be accessed from multiple threads if that object is designed to be thread-safe (e.g., it uses its own internal mutexes).

-----

### 2\. `std::shared_ptr` — The Democratic Owner 🤝

A `std::shared_ptr` allows for **shared, non-exclusive ownership**. Multiple `shared_ptr`s can point to and "own" the same object. The object is kept alive as long as at least one `shared_ptr` points to it.

**How it Works: Reference Counting**
`std::shared_ptr` uses a mechanism called reference counting. It maintains a separate block of memory called a **control block** which stores:

1.  A **strong reference count**: The number of `shared_ptr`s owning the object.
2.  A **weak reference count**: The number of `weak_ptr`s observing the object (more on this later).
3.  A pointer to the managed object.

<!-- end list -->

  * When a `shared_ptr` is **copied**, the strong reference count is **incremented**.
  * When a `shared_ptr` is **destroyed**, the strong reference count is **decremented**.
  * When the strong reference count reaches **zero**, the managed object is deleted, and the control block is freed.

**Thread Safety:**
The **reference count itself is thread-safe**. C++ guarantees that incrementing and decrementing the count are atomic operations. This means you can safely copy and destroy `shared_ptr`s across different threads without causing data races *on the reference count*.

However, just like `unique_ptr`, this does **not** make the managed object itself thread-safe. If multiple threads access the object's data through `shared_ptr`s, you still need to use mutexes or other synchronization mechanisms.

**When to Use It:**

  * Use it only when you genuinely need shared ownership—when the lifetime of an object is managed by multiple, independent parts of your code, and it's not clear which part will be the last one to use it.
  * Graphs where multiple nodes might point to the same child node.
  * Objects stored in containers that are passed around your system.

-----

### 3\. `std::weak_ptr` — The Observer 👀

A `std::weak_ptr` is a **non-owning** smart pointer. It "observes" an object that is managed by one or more `shared_ptr`s but does **not** participate in ownership or affect the reference count.

**How it Works:**
You can only create a `weak_ptr` from a `shared_ptr`. It points to the same control block but only increments the *weak* reference count. To actually access the object, you must call the `.lock()` method.

  * `weak_ptr.lock()` returns a `shared_ptr`.
  * If the object still exists, you get a valid `shared_ptr`.
  * If the object has already been deleted (the strong reference count went to zero), you get a `nullptr` `shared_ptr`.

This provides a safe way to check if an object is still alive before trying to use it.

**Key Use Case: Breaking Circular Dependencies**
This is the most famous use case for `weak_ptr`. Consider two objects that point to each other:

```cpp
struct Child;
struct Parent {
    std::shared_ptr<Child> child;
};

struct Child {
    std::shared_ptr<Parent> parent; // <-- This creates a cycle!
};

// ...
auto p = std::make_shared<Parent>();
auto c = std::make_shared<Child>();
p->child = c;
c->parent = p;
```

When `p` and `c` go out of scope, `p`'s reference count is 1 (held by `c`) and `c`'s reference count is 1 (held by `p`). Neither count will ever reach zero. **This is a memory leak.**

**The Solution:** The child's pointer back to the parent should be a `weak_ptr`.

```cpp
struct Child {
    std::weak_ptr<Parent> parent; // <-- Solution! No ownership cycle.
};
```

Now, the `Child` object doesn't own the `Parent`, so when the original `shared_ptr` to the `Parent` goes out of scope, its reference count drops to zero, and it gets deleted. This, in turn, destroys its `shared_ptr` to the `Child`, allowing the `Child` to be deleted as well.

**When to Use It:**

  * **To break reference cycles.**
  * In caches where you want to hold a pointer to an object but not prevent it from being deleted if it's no longer used elsewhere.
  * Any observer pattern where the "observer" should not keep the "subject" alive.

-----

### Summary Table

| Feature               | `std::unique_ptr`                               | `std::shared_ptr`                                    | `std::weak_ptr`                                         |
| --------------------- | ----------------------------------------------- | ---------------------------------------------------- | ------------------------------------------------------- |
| **Ownership** | **Exclusive**, unique                           | **Shared**, non-exclusive                            | **None** (observes)                                     |
| **Mechanism** | Simple RAII wrapper                             | Reference counting                                   | Observes a `shared_ptr`'s control block                 |
| **Performance** | Very fast (almost zero overhead over raw ptr)   | Slower (atomic ref-counting overhead)                | Slower (requires `.lock()` to access)                   |
| **Copyable?** | No                                              | Yes                                                  | Yes                                                     |
| **Movable?** | Yes (transfers ownership)                       | Yes                                                  | Yes                                                     |
| **Key Use Case** | **Default choice**, factory functions, PIMPL    | True shared ownership, complex object graphs         | **Breaking circular dependencies**, caches, observers |
| **Thread Safety** | Pointer itself is not safe; object can be       | **Reference count is atomic**; object data is not safe | Pointer itself is not safe; object can be             |

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
