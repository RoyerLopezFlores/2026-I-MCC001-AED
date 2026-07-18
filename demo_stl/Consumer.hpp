#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>


template<typename T, size_t Size = 1024>
class SPSCQueue {
private:
    std::vector<T> buffer;
    std::atomic<size_t> head{0};
    std::atomic<size_t> tail{0};
    std::atomic<bool> finished{false};

public:
    SPSCQueue() : buffer(Size) {}

    bool push(T&& value) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Size;

        if (next_tail == head.load(std::memory_order_acquire)) {
            return false; // Cola llena
        }

        buffer[current_tail] = std::move(value);
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t current_head = head.load(std::memory_order_relaxed);

        if (current_head == tail.load(std::memory_order_acquire)) {
            return false; // Cola vacía
        }

        value = std::move(buffer[current_head]);
        head.store((current_head + 1) % Size, std::memory_order_release);
        return true;
    }

    void set_finished() { finished.store(true, std::memory_order_release); }
    bool is_finished() { return finished.load(std::memory_order_acquire); }
};


