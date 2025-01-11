#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <chrono>

double GetTime() {
	const auto now = std::chrono::system_clock::now();
	const auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * 0.001;
}

#define MAX_NUM_STOPWATCH_TIMERS 10
struct StopWatch {
    struct Timer {
        std::string name;
        double t = 0;
        double duration = 0;
        bool running = false;
    } timers[MAX_NUM_STOPWATCH_TIMERS];

    void start(const char* name) {
        if (name == nullptr) {
            printf("Failed to start timer: Name is null\n");
            return;
        }
        double t = GetTime();
        // First check if the timer already exists and overwrite it
        bool exists = false;
        for (int i = 0; i < MAX_NUM_STOPWATCH_TIMERS; i++) {
            if (timers[i].name == name) {
                timers[i].t = t;
                timers[i].running = true;
                exists = true;
                break;
            }
        }
        // If the timer does not exist, create a new one (either use an empty slot or overwrite a timer that's done)
        if (!exists) {
            bool found = false;
            for (int i = 0; i < MAX_NUM_STOPWATCH_TIMERS; i++) {
                if (!timers[i].running) {
                    timers[i].name = name;
                    timers[i].t = t;
                    timers[i].running = true;
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("Failed to start timer: No available slots\n");
            }
        }
    }
    Timer* stop(const char* name) {
        if (name == nullptr) {
            printf("Failed to stop timer: Name is null\n");
            return nullptr;
        }
        double t = GetTime();
        for (int i = 0; i < MAX_NUM_STOPWATCH_TIMERS; i++) {
            if (timers[i].name == name) {
                if (timers[i].running) timers[i].duration = t - timers[i].t;
                timers[i].running = false;
                return &timers[i];
            }
        }
        printf("Failed to stop timer: Timer \"%s\" not found\n", name);
        return nullptr;
    }
    void log(const char* name) {
        if (name == nullptr) {
            printf("Failed to log timer: Name is null\n");
            return;
        }
        Timer* timer = stop(name);
        if (!timer) {
            printf("Failed to log timer: Timer \"%s\" not found\n", name);
            return;
        }
        // printf("%s in %.3f ms\n", message, duration * 1000.0);
        printf("%s in %.1f ms\n", timer->name.c_str(), timer->duration * 1000.0);
    }
    double time(const char* name) {
        if (name == nullptr) {
            printf("Failed to get time: Name is null\n");
            return 0;
        }
        for (int i = 0; i < MAX_NUM_STOPWATCH_TIMERS; i++) {
            if (timers[i].name == name) {
                if (timers[i].running) {
                    double t = GetTime();
                    return t - timers[i].t;
                }
                return timers[i].duration;
            }
        }
        printf("Failed to get time: Timer \"%s\" not found\n", name);
        return 0;
    }
} timer;