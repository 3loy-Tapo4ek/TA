#pragma once

#include <string>
#include <map>
#include <stdexcept>

namespace regex_engine {

class RegexMatch {
public:
    std::string full_match; // Полное совпадение
    
    // Конструктор
    explicit RegexMatch(std::string match) : full_match(std::move(match)) {}

    // Метод для добавления группы изнутри движка
    void addGroup(const std::string& name, const std::string& value);

    // Доступ по индексу (имени группы)
    std::string operator[](const std::string& name) const;

    // Поддержка итератора (для range-based for)
    using Iterator = std::map<std::string, std::string>::const_iterator;
    Iterator begin() const { return groups_.begin(); }
    Iterator end() const { return groups_.end(); }

private:
    std::map<std::string, std::string> groups_;
};

} // namespace regex_engine