#include "Recognizer.hpp"


std::pair<bool, std::string> Recognizer::test(std::string_view)
{
    return std::pair<bool, std::string>(true, "test");
}
