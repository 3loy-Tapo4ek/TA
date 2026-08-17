#pragma once

#include <vector>
#include <string>
#include "Point.hpp"

class Maze{
private:
    std::vector<std::string> grid_;
    Point start_pos_{0, 0};
    Point exit_pos_{0, 0};

public:
    Maze() = default;

    static Maze fromFile(const std::string& filepath);

    bool isWall(const Point& p) const noexcept;
    const Point& getStart() const noexcept;
    const Point& getExit() const noexcept;
    const std::vector<std::string>& getGrid() const noexcept;
};