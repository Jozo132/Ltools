#pragma once

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

int32_t previous_memory = 0;

int32_t getHeapUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        return (int32_t) memInfo.WorkingSetSize;
    } else {
        std::cerr << "Failed to get memory info.\n";
        return 0;
    }
#else // Other platforms
    return 0;
#endif
}

void printHeapUsage() {
    int32_t memory = getHeapUsage();
    if (previous_memory == 0) {
        std::cout << "Heap usage: " << memory << " bytes\n";
    } else {
        int32_t diff = memory - previous_memory;
        std::cout << "Heap usage: " << memory << " bytes (" << (diff > 0 ? "+" : "") << diff << ")\n";
    }
    previous_memory = memory;
}