#pragma once

#include "Environment.hpp"
#include <memory>

//ScopeGuard switches current_environmen_ to previous_env_ and switches back after end of scope
class ScopeGuard 
{
private:
    std::shared_ptr<Environment>& current_env_ref_;
    std::shared_ptr<Environment> previous_env_;

    public:
    ScopeGuard(std::shared_ptr<Environment>& current, std::shared_ptr<Environment> new_env)
    : current_env_ref_(current), previous_env_(current) { current_env_ref_ = std::move(new_env); }

    ~ScopeGuard() { current_env_ref_ = previous_env_;}

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};