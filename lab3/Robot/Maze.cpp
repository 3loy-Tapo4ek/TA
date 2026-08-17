#include "Maze.hpp"
#include <fstream>
#include <stdexcept>

Maze Maze::fromFile(const std::string& filepath) {
    Maze maze;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл лабиринта: " + filepath);
    }

    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            if (line[x] == 'S') {
                maze.start_pos_ = {x, y};
            } else if (line[x] == 'E') {
                maze.exit_pos_ = {x, y};
            }
        }
        maze.grid_.push_back(line);
        y++;
    }
    return maze;
}

bool Maze::isWall(const Point& p) const noexcept {
    if (p.y < 0 || p.y >= static_cast<int>(grid_.size())) return true;
    if (p.x < 0 || p.x >= static_cast<int>(grid_[p.y].size())) return true;
    return grid_[p.y][p.x] == '#';
}

const Point& Maze::getStart() const noexcept {
    return start_pos_;
}

const Point& Maze::getExit() const noexcept {
    return exit_pos_;
}

const std::vector<std::string>& Maze::getGrid() const noexcept {
    return grid_;
}