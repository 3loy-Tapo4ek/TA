#include "RegexMatch.hpp"

namespace regex_engine {

void RegexMatch::addGroup(const std::string& name, const std::string& value) {
    groups_[name] = value;
}

std::string RegexMatch::operator[](const std::string& name) const {
    auto it = groups_.find(name);
    if (it != groups_.end()) {
        return it->second;
    }
    throw std::out_of_range("Capture group '" + name + "' not found.");
}

} // namespace regex_engine