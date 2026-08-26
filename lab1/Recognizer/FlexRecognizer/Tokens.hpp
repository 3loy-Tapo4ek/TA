#pragma once

#pragma once

enum class Token : int
{
    END_OF_FILE = 0,
    INT = 1,
    SHORT = 2,
    LONG = 3,
    ID = 4,
    ASSIGN = 5,
    NUMBER = 6,
    PLUS = 7,
    MINUS = 8,
    STAR = 9,
    SLASH = 10,
    ERROR = -1
};