#include "ConsoleRobotController.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>

ConsoleRobotController::ConsoleRobotController(Maze maze)
    : maze_(std::move(maze)), current_pos_(maze_.getStart()) {
    history_.push_back(current_pos_);
    render("Старт робота");
}

bool ConsoleRobotController::move(Direction dir) {
    Point next = current_pos_;
    switch (dir) {
        case Direction::Up:    next.y -= 1; break;
        case Direction::Down:  next.y += 1; break;
        case Direction::Left:  next.x -= 1; break;
        case Direction::Right: next.x += 1; break;
    }

    if (maze_.isWall(next)) {
        render("Препятствие (стена)!");
        return false;
    }

    current_pos_ = next;
    history_.push_back(current_pos_);

    if (current_pos_ == maze_.getExit()) {
        exit_reached_ = true;
        render("ВЫХОД НАЙДЕН! ПОБЕДА!");
    } else {
        render("Движение...");
    }

    return true;
}

void ConsoleRobotController::timeshift(int steps) {
    if (steps <= 0) return;
    if (static_cast<size_t>(steps) >= history_.size()) {
        current_pos_ = history_.front();
    } else {
        current_pos_ = history_[history_.size() - 1 - steps];
    }
    history_.push_back(current_pos_);
    render("Timeshift на " + std::to_string(steps) + " шагов назад");
}

void ConsoleRobotController::timeshift(const std::string& label) {
    auto it = bookmarks_.find(label);
    if (it == bookmarks_.end()) {
        throw std::runtime_error("Метка чекпоинта '" + label + "' не найдена");
    }
    current_pos_ = it->second;
    history_.push_back(current_pos_);
    render("Timeshift к метке: " + label);
}

void ConsoleRobotController::bind(const std::string& label) {
    bookmarks_[label] = current_pos_;
    render("Установлен чекпоинт: " + label);
}

bool ConsoleRobotController::isExitReached() const noexcept {
    return exit_reached_;
}

void ConsoleRobotController::setDelay(int ms) noexcept {
    delay_ms_ = ms;
}

void ConsoleRobotController::render(const std::string& status) {
    std::cout << "\033[H\033[J"; // Очистка экрана ANSI
    std::cout << "=== СИМУЛЯЦИЯ КЛЕТОЧНОГО РОБОТА ===\n";
    std::cout << "Статус: " << status << "\n";
    std::cout << "Позиция: (" << current_pos_.x << ", " << current_pos_.y << ")\n\n";

    const auto& grid = maze_.getGrid();
    for (int y = 0; y < static_cast<int>(grid.size()); ++y) {
        for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
            Point p{x, y};
            if (p == current_pos_) {
                std::cout << "\033[1;32mR\033[0m "; // Зеленый робот
            } else if (p == maze_.getExit()) {
                std::cout << "\033[1;31mE\033[0m "; // Красный выход
            } else if (grid[y][x] == '#') {
                std::cout << "█ ";                 // Стена
            } else {
                std::cout << ". ";                 // Пустой проход
            }
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
    if (delay_ms_ > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms_));
    }
}