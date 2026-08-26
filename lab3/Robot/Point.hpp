#pragma once
#include <compare>

struct Point
{
    int x = 0;
    int y = 0;

    auto operator<=>(const Point&) const = default;
};