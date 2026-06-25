/**
 * @file Timer.hpp
 * @brief High-resolution RAII-based execution timer.
 *
 * Provides a simple RAII class using std::chrono to print execution
 * times on destruction or query elapsed milliseconds programmatically.
 */

#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <utility>

class Timer {
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    bool active;

public:
    /**
     * @brief Construct a Timer. Starts the clock immediately.
     * @param timerName Name to print upon destruction.
     */
    explicit Timer(std::string timerName) 
        : name(std::move(timerName)), 
          start(std::chrono::high_resolution_clock::now()), 
          active(true) {}

    /**
     * @brief Destructor. Automatically prints elapsed time if the timer has not been stopped.
     */
    ~Timer() {
        if (active) {
            double elapsed = elapsed_ms();
            std::cout << "[Timer] " << name << " took " << elapsed << " ms\n";
        }
    }

    /**
     * @brief Stops the timer and prevents automatic printing in the destructor.
     * @return double Elapsed time in milliseconds.
     */
    double stop() {
        active = false;
        return elapsed_ms();
    }

    /**
     * @brief Queries the elapsed time since construction.
     * @return double Elapsed time in milliseconds.
     */
    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return static_cast<double>(duration) / 1000.0;
    }
};
