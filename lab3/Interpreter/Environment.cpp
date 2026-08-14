#include "Environment.hpp"

void Environment::Define(const std::string& name, Value val, bool isMutable)
{
    if (bindings_.contains(name))
    {
        throw std::runtime_error("Переменна уже объявлена в этом блоке");
    }

    bindings_.insert({name, {val, isMutable}});
};

void Environment::Set(const std::string& name, Value val) {
    auto& var = resolve(name);
    
    if (!var.is_mutable)
    {
        throw std::runtime_error("Нелья менять константную переменную '" + name + "'");
    }
    var.value = std::move(val);
}

Variable& Environment::resolve(const std::string& name) {
    auto it = bindings_.find(name);
    if (it != bindings_.end())
    {
        return it->second;
    }
    if (parent_)
    {
        return parent_->resolve(name);
    }

    throw std::runtime_error("Переменная '" + name + "' не определена");
}

const Variable& Environment::resolve(const std::string& name) const {
    auto it = bindings_.find(name);

    if (it != bindings_.end())
    {
        return it->second;
    }
    if (parent_)
    {
        return parent_->resolve(name);
    }
    throw std::runtime_error("Переменная '" + name + "' не определена");
}

Value Environment::get(const std::string& name) const
{
    return resolve(name).value;
};
