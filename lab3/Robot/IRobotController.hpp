#pragma once

#include <string>

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class IRobotController {
public:
    virtual ~IRobotController() = default;

    virtual bool move(Direction dir) = 0;

    virtual void timeshift(int steps) = 0;

    virtual void timeshift(const std::string& label) = 0;

    virtual void bind(const std::string& label) = 0;

    virtual bool isExitReached() const noexcept = 0;
};