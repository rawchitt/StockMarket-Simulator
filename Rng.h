#pragma once
// ============================================================
//  Rng.h  --  Single globally-seeded Mersenne Twister
//
//  MinGW's std::random_device is broken (always returns 5).
//  MinGW's high_resolution_clock can have millisecond or even
//  second resolution, causing the same seed every launch.
//
//  Fix: XOR together MULTIPLE entropy sources so the seed is
//  always unique even on the same machine at the same second.
// ============================================================
#include <random>
#include <chrono>
#include <ctime>
#ifdef _WIN32
  #include <windows.h>   // QueryPerformanceCounter
  #include <process.h>   // _getpid
#else
  #include <unistd.h>
#endif

inline std::mt19937& globalRng() {
    static std::mt19937 rng = []() {
        // Source 1: high_resolution_clock (nanoseconds if available)
        uint64_t t1 = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());

        // Source 2: steady_clock (different epoch on some implementations)
        uint64_t t2 = static_cast<uint64_t>(
            std::chrono::steady_clock::now().time_since_epoch().count());

        // Source 3: system_clock (calendar time in nanoseconds/microseconds)
        uint64_t t3 = static_cast<uint64_t>(
            std::chrono::system_clock::now().time_since_epoch().count());

        // Source 4: Windows high-performance counter (sub-microsecond on real HW)
        uint64_t t4 = 0;
#ifdef _WIN32
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        t4 = static_cast<uint64_t>(li.QuadPart);
#endif

        // Source 5: Process ID (unique per process)
        uint64_t pid = 0;
#ifdef _WIN32
        pid = static_cast<uint64_t>(GetCurrentProcessId());
#else
        pid = static_cast<uint64_t>(getpid());
#endif

        // Source 6: Address of a stack variable (ASLR randomness)
        uint64_t addr = reinterpret_cast<uint64_t>(&t1);

        // Combine all sources with XOR + bit mixing
        uint64_t seed = t1 ^ t2 ^ t3 ^ t4 ^ pid ^ addr;
        // Avalanche the bits so even small differences spread widely
        seed ^= (seed >> 33);
        seed *= 0xff51afd7ed558ccdULL;
        seed ^= (seed >> 33);
        seed *= 0xc4ceb9fe1a85ec53ULL;
        seed ^= (seed >> 33);

        return std::mt19937(static_cast<uint32_t>(seed));
    }();
    return rng;
}
