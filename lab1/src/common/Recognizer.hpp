#pragma once

#include <utility>
#include <string>
#include <string_view>


class IRecognizer
{
public:
    virtual std::pair<bool, std::string> test(std::string_view) = 0;

    virtual ~IRecognizer() {};
};

class Recognizer : public IRecognizer
{
public:
    std::pair<bool, std::string> test(std::string_view) override;

    ~Recognizer() override = default;
};