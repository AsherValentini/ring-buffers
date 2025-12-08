#include <iostream>
#include <thread>
#include <cstdint>
#include <chrono>
#include <sched.h> // cpu_set_t, CPU_ZERO, CPU_SET
#include <pthread.h> // pthread_sett_affinity_np (non portable linux extension not a POSIX call)

#include "ring_buffer.hpp"

using namespace std::chrono_literals;

namespace cores {
    void pin_to_core(int core_id) {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(core_id, &set);
        pthread_setaffinity_np(
            pthread_self(),
            sizeof(cpu_set_t),
            &set
        );
    }
}
int main() {
    // Construct the bench mark atomic based ring buffer

    asher::SPSCQ<uint64_t> atomic_ring_buffer(1024);

    auto start_time = std::chrono::high_resolution_clock::now();
    int elements = 10'000;

    std::thread producer([&](){
        cores::pin_to_core(0);

        std::size_t pushed_counter = 0;
        std::uint64_t item = 1;
        while(pushed_counter < elements) {
            if(atomic_ring_buffer.push(item)) pushed_counter++;
        }
    });


    std::thread consumer([&](){
        cores::pin_to_core(1);

        std::size_t popped_counter = 0;
        std::uint64_t item;
        while(popped_counter < elements) {
            if(atomic_ring_buffer.pop(item)) popped_counter++;
        }
    });

    if(consumer.joinable()) {
        consumer.join();
    }
    if(producer.joinable()) {
        producer.join();
    }


    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = end_time - start_time;

    long long milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

    std::cout << "Push -> Pop " << elements << "\n"
              << nanoseconds << " [ns]\n"
              << milliseconds << " [ms]"
              << std::endl;


	return 0;
}
