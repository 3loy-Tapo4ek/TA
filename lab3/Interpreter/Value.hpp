#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <stdexcept>

struct Pointer
{
    std::string target_name_;
    size_t offset_ = 0;

    bool operator==(const Pointer&) const = default;
};
class Value;

using Array = std::vector<Value>;
using VariantType = std::variant<std::monostate, int, std::string, Pointer, Array>;


class Value
{
private:
    VariantType data_;
public:
    Value() = default;
    ~Value() = default;

    explicit Value(int value) : data_(value) {}
    explicit Value(std::string string) : data_(std::move(string)) {}
    explicit Value(Pointer pointer) : data_(pointer) {}
    explicit Value(Array array) : data_(std::move(array)) {}

    //checkers
    bool isNull() const noexcept { return std::holds_alternative<std::monostate>(data_); }
    bool isInt() const noexcept { return std::holds_alternative<int>(data_); }
    bool isString() const noexcept { return std::holds_alternative<std::string>(data_); }
    bool isPointer() const noexcept { return std::holds_alternative<Pointer>(data_); }
    bool isArray() const noexcept { return std::holds_alternative<Array>(data_); }

    //getters
    const int asInt() const
    {
        if (!isInt()) { throw std::runtime_error("Ожидался int");}
        return std::get<int>(data_);
    }

    const std::string& asString() const
    {
        if (!isString()) { throw std::runtime_error("Ожидался string");}
        return std::get<std::string>(data_);
    }

    const Pointer& asPointer() const
    {
        if (!isPointer()) { throw std::runtime_error("Ожидался pointer");}
        return std::get<Pointer>(data_);
    }

    const Array& asArray() const 
    {
        if (!isArray()) { throw std::runtime_error("Ожидался array");}
        return std::get<Array>(data_);
    }

    Array& asArray()
    {
        if (!isArray()) { throw std::runtime_error("Ожидался array");}
        return std::get<Array&>(data_);
    }

};
