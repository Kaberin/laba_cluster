#include "Task.hpp"
#include <random>
#include <cstdint>
#include <iomanip>

Response PiWithMonteCarlo(const Request& request) {
    std::random_device rd;
    std::mt19937 rand(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    uint64_t totalPoints = request.totalPointsNumber;
    uint64_t pointsInside = 0;
    for (int i = 0; i < totalPoints; ++i) {
        double x = dist(rand);
        double y = dist(rand);
        double r = sqrt(x * x + y * y);
        if (r <= 1.0) {
            pointsInside++;
        }
    }
    return { pointsInside, totalPoints, request.id };
}