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

    // Перемещение: возвращает true (1) при успехе, false (0) при столкновении со стеной
    virtual bool move(Direction dir) = 0;

    // Возврат на steps позиций назад в истории
    virtual void timeshift(int steps) = 0;

    // Возврат к именованной контрольной точке
    virtual void timeshift(const std::string& label) = 0;

    // Привязка строки к текущей позиции
    virtual void bind(const std::string& label) = 0;

    // Проверка, дошел ли робот до клетки с выходом
    virtual bool isExitReached() const noexcept = 0;
};