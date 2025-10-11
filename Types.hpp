
#pragma once
#include <cstdint>
#include <string>
struct Request {
    uint64_t totalPointsNumber;
    int id;
};

struct Response {
    uint64_t pointsInCricle;
    uint64_t totalPointsNumber;
    int id;
};

static const std::string POINTS_TOTAL = "pointsForEach";
static const std::string POINTS_IN_CIRCLE = "pointsInCircle";
static const std::string ID = "id";