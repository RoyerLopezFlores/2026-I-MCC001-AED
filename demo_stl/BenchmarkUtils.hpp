#pragma once

#include <chrono>
#include <functional>





#include "Persona.hpp"

inline constexpr unsigned int kNumThreads = 8;

template <typename Func, typename... Args>
auto medirTiempoMs(Func&& func, Args&&... args) {
    using ReturnT = std::invoke_result_t<Func, Args...>;
    const auto start = std::chrono::steady_clock::now();

    if constexpr (std::is_void_v<ReturnT>) {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        const auto end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    } else {
        ReturnT result = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        const auto end = std::chrono::steady_clock::now();
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        return std::make_pair(std::move(result), elapsedMs);
    }
}

size_t getSizeofPersona(const Persona& persona);

void printResultParallel(const std::string& methodName, size_t totalSize, long long elapsedMs);
