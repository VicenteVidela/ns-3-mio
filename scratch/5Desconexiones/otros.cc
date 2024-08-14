#include "otros.h"

// Function to print progress
void PrintProgress(double currentTime, double stopTime) {
    int barWidth = 50;
    double progress = (currentTime / stopTime) * 100.0;
    int pos = barWidth * progress / 100.0;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << progress << " %\r";
    std::cout.flush();
}