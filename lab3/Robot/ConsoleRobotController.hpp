#pragma once

#include "IRobotController.hpp"
#include "Maze.hpp"
#include "Point.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class ConsoleRobotController : public IRobotController
{
private:
    Maze maze_;

    Point current_pos_;

    std::vector<Point> history_;
    std::unordered_map<std::string, Point> bookmarks_;
    
    int delay_ms_ = 120;
    bool exit_reached_ = false;

    void render(const std::string& status);

public:
    explicit ConsoleRobotController(Maze maze);

    bool move(Direction dir) override;
    void timeshift(int steps) override;
    void timeshift(const std::string& label) override;
    void bind(const std::string& label) override;
    bool isExitReached() const noexcept override;

    void setDelay(int ms) noexcept;
};