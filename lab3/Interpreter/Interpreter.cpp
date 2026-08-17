#include "Interpreter.hpp"

Value Interpreter::Evaluate(const ExprNode& expression)
{
    expression.accept(*this);

    return std::move(last_evaluated_value_);
};

void Interpreter::Execute(const StatementNode& statement)
{
    statement.accept(*this);
};

void Interpreter::Interpret(const StatementNode& root)
{
    Execute(root);
};

//visitors for basic expressions
void Interpreter::visit(const NumberNode& node)
{
    last_evaluated_value_ = Value(node.getValue());
};

void Interpreter::visit(const StringNode& node)
{
    last_evaluated_value_ = Value(node.getValue());
};

void Interpreter::visit(const VariableNode& node)
{
    last_evaluated_value_ = current_environment_->resolve(node.getName()).value;
};

void Interpreter::visit(const ExprStatementNode& node)
{
    Evaluate(*node.getExpression());
};

//visiting binary operations
void Interpreter::visit(const BinaryOpNode& node)
{
    Value left = Evaluate(*node.getLeft());
    Value right = Evaluate(*node.getRight());

    switch (node.getOperation())
    {
    case BinaryOp::Add:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = Value(left.asInt() + right.asInt());
            return;
        }
        else if (left.isString() && right.isString())
        {
            last_evaluated_value_ = Value(left.asString() + right.asString());
            return;
        }

        else if(left.isPointer() && right.isInt())
        {
            Pointer pointer = left.asPointer();
            pointer.offset_ += right.asInt();
            last_evaluated_value_ = Value(pointer);

            return;
        }
        else if(left.isInt() && right.isPointer())
        {
            Pointer pointer = right.asPointer();
            pointer.offset_ += left.asInt();
            last_evaluated_value_ = Value(pointer);
            return;
        }
        throw RuntimeError("Недопустимые типы операндов");

    case BinaryOp::Subtract:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = Value(left.asInt() - right.asInt());
            return;
        }

        else if(left.isPointer() && right.isInt())
        {
            Pointer pointer = left.asPointer();
            pointer.offset_ -= right.asInt();
            last_evaluated_value_ = Value(pointer);
            return;
        }
        throw RuntimeError("Недопустимые типы операндов");

    case BinaryOp::Multiply:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = Value(left.asInt() * right.asInt());
            return;
        }
        throw RuntimeError("Недопустимые типы операндов");

    case BinaryOp::Divide:
        if (!left.isInt() || !right.isInt()) throw RuntimeError("Недопустимые типы операндов");
        if (right.asInt() == 0) {throw RuntimeError("Деление на ноль");}

        last_evaluated_value_ = Value(left.asInt() / right.asInt());
        return;

    case BinaryOp::Remainder:
        if (!left.isInt() || !right.isInt()) throw RuntimeError("Недопустимые типы операндов");
        if (right.asInt() == 0) {throw RuntimeError("Деление на ноль");}

        last_evaluated_value_ = Value(left.asInt() % right.asInt());
        return;
    
    //bool binary operations
    case BinaryOp::Equal:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = makeBoolValue(left.asInt() == right.asInt());
            return;
        }

        else if (left.isString() && right.isString())
        {
            last_evaluated_value_ = makeBoolValue(left.asString() == right.asString());
            return;
        }

        else if (left.isPointer() && right.isPointer())
        {
            last_evaluated_value_ = makeBoolValue(left.asPointer() == right.asPointer());
            return;
        }

        throw RuntimeError("Нельзя сравнивать значения разных типов");

    case BinaryOp::LessThan:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = makeBoolValue(left.asInt() < right.asInt());
            return;
        } 

        else if (left.isString() && right.isString())
        {
            last_evaluated_value_ = makeBoolValue(left.asString() < right.asString());
            return;
        }

        throw RuntimeError("Недопустимые типы для оператора '<'");

    case BinaryOp::GreaterThan:
        if (left.isInt() && right.isInt())
        {
            last_evaluated_value_ = makeBoolValue(left.asInt() > right.asInt());
            return;
        }
        else if (left.isString() && right.isString())
        {
            last_evaluated_value_ = makeBoolValue(left.asString() > right.asString());
            return;
        }

        throw RuntimeError("Недопустимые типы для оператора '>'");
    default:
        break;
    }
}

Value Interpreter::makeBoolValue(bool condition)
{
    if (condition)
    {
        return Value(1);
    }
    else
    {
        return Value(0);
    }
}

//visiting unary operations
void Interpreter::visit(const UnaryOpNode& node)
{
    const ExprNode* operand = node.getOperand().get();

    switch (node.getOperation())
    {
    case UnaryOp::AddressOf:
        {
            const auto* var_node = dynamic_cast<const VariableNode*>(operand);
            const auto* array_node = dynamic_cast<const ArrayAccessNode*>(operand);
            if (var_node)
            {
                last_evaluated_value_ = Value(Pointer{ var_node->getName(), 0 });
                return;
            }
            else if (array_node)
            {
                Value id = Evaluate(*array_node->getIndex());
                if (!id.isInt() || id.asInt() < 0) { throw RuntimeError("Индекс массива должен быть неотрицательным числом");}
            
                last_evaluated_value_ = Value(Pointer{ array_node->getArrayName(), static_cast<size_t>(id.asInt()) });
                return;
            }
            throw RuntimeError("Оператор '&' применим только к переменным и элементам массива");
        }
    case UnaryOp::SizeOf:
        {
            Value value = Evaluate(*node.getOperand());
            if (value.isArray())
            {
                last_evaluated_value_ = Value(value.asArray().size());
            } 
            else
            {
                last_evaluated_value_ = Value(1);
            }

            return;
        }

    case UnaryOp::Dereference:
        {
            Value value = Evaluate(*node.getOperand());
            
            if (!value.isPointer()) { throw RuntimeError("Оператор '*' применим только к указателям");}
            
            const Pointer& pointer = value.asPointer();

            auto& target_variable = current_environment_->resolve(pointer.target_name_);

            if (target_variable.value.isArray())
            {
                auto& array = target_variable.value.asArray();
                if (pointer.offset_ >= array.size()) {throw RuntimeError("Разыменование указателя за пределами массива");}
                
                last_evaluated_value_ = array[pointer.offset_];
                return;
            }

            if (target_variable.value.isPointer() || target_variable.value.isInt() || target_variable.value.isString())
            {
                if (pointer.offset_ != 0) { throw RuntimeError("Некорректное смещение указателя для скалярной переменной");}
                
                last_evaluated_value_ = target_variable.value;
                return;
            }

            throw RuntimeError("Неизвестный тип данных для разыменования");
        }

    default:
        throw RuntimeError("Неизвестная унарная операция");
    }
};

//visiting statements
void Interpreter::visit(const VariableDeclNode& node)
{
    switch (node.getType())
    {
    case DataType::Array:
        {
            int size = 10;
            if (node.getArraySize())
            {
                size = Evaluate(*node.getArraySize()).asInt();
            }

            Array new_array(size, Value(0));
            current_environment_->Define(node.getName(), Value(new_array), node.isMutable());
            return;
        }
    case DataType::Integer:
        {
            if (node.getInitialValue())
            {
                auto initital_value = Evaluate(*node.getInitialValue()).asInt();
                current_environment_->Define(node.getName(), Value(initital_value), node.isMutable());
                return;
            }
            else
            {
                if (!node.isMutable()) {throw RuntimeError("Константа '" + node.getName() + "' должна быть инициализирована");}
                current_environment_->Define(node.getName(), Value(0), node.isMutable());
                return;
            }
        }
    case DataType::String:
        {
            if (node.getInitialValue())
            {
                auto initital_value = Evaluate(*node.getInitialValue()).asString();
                current_environment_->Define(node.getName(), Value(initital_value), node.isMutable());
                return;
            }
            else
            {
                if (!node.isMutable()) {throw RuntimeError("Константа '" + node.getName() + "' должна быть инициализирована");}
                current_environment_->Define(node.getName(), Value(""), node.isMutable());
                return;
            }
        }
    case DataType::Pointer:
        {
            if (node.getInitialValue())
            {
                auto initital_value = Evaluate(*node.getInitialValue()).asPointer();
                current_environment_->Define(node.getName(), Value(initital_value), node.isMutable());
                return;
            }
            else
            {
                if (!node.isMutable()) {throw RuntimeError("Константа '" + node.getName() + "' должна быть инициализирована");}
                current_environment_->Define(node.getName(), Value(Pointer{}), node.isMutable());
                return;
            }
        }
    default:
        break;
    }
};

void Interpreter::visit(const AssignNode& node)
{
    Value right = Evaluate(*node.getValue());

    const ExprNode* left = node.getTarget().get();
    const auto* var_node = dynamic_cast<const VariableNode*>(left);

    if (var_node)
    {
        auto& var = current_environment_->resolve(var_node->getName());
        if (!var.is_mutable) {throw RuntimeError("Нельзя изменить константу '" + var_node->getName() + "'");}

        if (var.value.isInt() && !right.isInt()) { throw RuntimeError("Несоответствие типов: ожидался integer для переменной '" + var_node->getName() + "'");}
        if (var.value.isString() && !right.isString()) { throw RuntimeError("Несоответствие типов: ожидался string для переменной '" + var_node->getName() + "'");}
        if (var.value.isPointer() && !right.isPointer()) {throw RuntimeError("Несоответствие типов: ожидался pointer для переменной '" + var_node->getName() + "'");}

        var.value = right;
        last_evaluated_value_ = right;
        return;
    }

    const auto* array_node = dynamic_cast<const ArrayAccessNode*>(left);
    if (array_node)
    {
        auto& var = current_environment_->resolve(array_node->getArrayName());
        if (!var.is_mutable) { throw RuntimeError("Нельзя изменить константный массив '" + array_node->getArrayName() + "'");}

        Value id = Evaluate(*array_node->getIndex());
        if (!id.isInt() || id.asInt() < 0) { throw RuntimeError("Индекс массива должен быть неотрицательным числом");}

        size_t idx = static_cast<size_t>(id.asInt());
        auto& arr = var.value.asArray();

        if (idx >= arr.size())
        {
            size_t new_size = ((idx / 10) + 1) * 10;
            arr.resize(new_size, Value(0));
        }

        arr[idx] = right;
        last_evaluated_value_ = right;
        return;
    }

    throw RuntimeError("Недопустимая левая часть оператора присваивания");
}

void Interpreter::visit(const BlockNode& node)
{
    auto previous_environment = current_environment_;

    current_environment_ = std::make_shared<Environment>(previous_environment);

    try
    {
        for (const auto& statements : node.getStatements())
        {
            if (statements) {Execute(*statements);}
        }
    } catch (...) {
        current_environment_ = previous_environment;
        throw; 
    }

    current_environment_ = previous_environment;
};

void Interpreter::visit(const CheckZeroNode& node)
{
    Value condition = Evaluate(*node.getCondition());
    if (!condition.isInt()) {throw RuntimeError("Условие должно быть типа int");}

    if (condition.asInt() == 0){ Execute(*node.getBody());}
    else if (condition.asInt() != 0 && node.getInsteadBody()) {Execute(*node.getInsteadBody());}
}

void Interpreter::visit(const WhileNode& node)
{
    bool is_first_iteration = true;

    while (true)
    {
        Value condition = Evaluate(*node.getCondition());
        if (!condition.isInt()) {throw RuntimeError("Условие цикла должно быть integer");}
        if (condition.asInt() != 0) 
        {
            is_first_iteration = false;
            try
            {
                Execute(*node.getBody());
            }
            catch (const BreakSignal&) { break; }
        }

        else
        {
            if(is_first_iteration && node.getInsteadBody() != nullptr) {Execute(*node.getInsteadBody());}
            break;
        }
    }
}

void Interpreter::visit(const ReturnNode& node)
{
    Value return_value;
    if (node.getExpression()) 
    {
        return_value = Evaluate(*node.getExpression());
    }

    throw ReturnSignal(std::move(return_value));
}

void Interpreter::visit(const ArrayAccessNode& node)
{
    const auto& array = current_environment_->resolve(node.getArrayName());

    Value id = Evaluate(*node.getIndex());

    if (!id.isInt() || id.asInt() < 0) { throw RuntimeError("Индекс массива должен быть неотрицательным целым числом");}

    if (id.asInt() >= 0)
    {
        if (id.asInt() >= array.value.asArray().size()) {throw RuntimeError("Выход за границы массива");}
        last_evaluated_value_ = array.value.asArray()[id.asInt()];
    }
}

void Interpreter::visit(const FunctionDeclNode& node)
{
    current_environment_->defineFunction(node.getName(), node, current_environment_);
}

void Interpreter::visit(const CallNode& node)
{
    const auto& function = current_environment_->resolveFunction(node.getFuncName());
    const auto& declaration = function.declaration_;

    if (!node.getTarget().empty())
    {
        if (!declaration.isIterator()) { throw RuntimeError("Функция '" + node.getFuncName() + "' не объявлена как iterator");}

        auto& target_var = current_environment_->resolve(node.getTarget());
        
        // Вычисляем дополнительные аргументы (with [...])
        std::vector<Value> extra_args;
        for (const auto& arg_expr : node.getArguments())
        {
            extra_args.push_back(Evaluate(*arg_expr));
        }

        size_t last_index = 0;

        if (target_var.value.isArray())
        {
            auto& arr = target_var.value.asArray();
            for (size_t i = 0; i < arr.size(); ++i)
            {
                last_index = i;

                // Создаем окружение для шага итератора
                auto iter_env = std::make_shared<Environment>(function.closure_);
                
                // Регистрируем 'current'
                iter_env->Define("current", arr[i], declaration.isMutableIterator());

                // Биндим остальные параметры из with [...]
                for (size_t p = 0; p < extra_args.size() && p < declaration.getParameters().size(); ++p)
                {
                    iter_env->Define(declaration.getParameters()[p].second, extra_args[p], true);
                }

                auto prev_env = current_environment_;
                current_environment_ = iter_env;

                Value ret_val(1);
                try {
                    Execute(*declaration.getBody());
                } catch (const ReturnSignal& sig) {
                    ret_val = sig.value;
                } catch (...) {
                    current_environment_ = prev_env;
                    throw;
                }

                // Если итератор mutable, синхронизируем изменения current обратно в массив
                if (declaration.isMutableIterator())
                {
                    arr[i] = iter_env->get("current");
                }

                current_environment_ = prev_env;

                // Если итератор вернул 0 — останавливаем проход!
                if (ret_val.isInt() && ret_val.asInt() == 0)
                {
                    break;
                }
            }
        }

        // Сохраняем 'last' в текущее окружение
        current_environment_->Define("last", Value(static_cast<int>(last_index)), true);
        last_evaluated_value_ = Value(static_cast<int>(last_index));
        return;
    }
    std::vector<Value> evaluated_args;
    for (const auto& arg_expr : node.getArguments())
    {
        evaluated_args.push_back(Evaluate(*arg_expr));
    }

    if (evaluated_args.size() != declaration.getParameters().size())
    {
        throw RuntimeError("Неверное количество аргументов при вызове функции '" + node.getFuncName() + "'");
    }

    auto function_environment = std::make_shared<Environment>(function.closure_);

    for (size_t i = 0; i < declaration.getParameters().size(); i++)
    {
        const auto& param_name = declaration.getParameters()[i].second;
        function_environment->Define(param_name, evaluated_args[i], true);
    }

    auto previous_environment = current_environment_;
    current_environment_ = function_environment;

    Value return_val;
    try
    {
        Execute(*declaration.getBody());
    } catch (const ReturnSignal& sig) { return_val = sig.value;} 
    catch (...) 
    {
        current_environment_ = previous_environment;
        throw;
    }
    current_environment_ = previous_environment;

    last_evaluated_value_ = std::move(return_val);
}

void Interpreter::visit(const RobotCommandNode& node) {
    switch (node.getAction()) {
    case RobotAction::Top:
        last_evaluated_value_ = Value(robot_.move(Direction::Up) ? 1 : 0);
        break;
    case RobotAction::Bottom:
        last_evaluated_value_ = Value(robot_.move(Direction::Down) ? 1 : 0);
        break;
    case RobotAction::Left:
        last_evaluated_value_ = Value(robot_.move(Direction::Left) ? 1 : 0);
        break;
    case RobotAction::Right:
        last_evaluated_value_ = Value(robot_.move(Direction::Right) ? 1 : 0);
        break;

    case RobotAction::Timeshift: {
        Value val = Evaluate(*node.getArgument());
        if (val.isInt()) {
            robot_.timeshift(val.asInt());
        } else if (val.isString()) {
            robot_.timeshift(val.asString());
        } else {
            throw RuntimeError("Команда 'timeshift' ожидает целое число или строку");
        }
        break;
    }

    case RobotAction::Bind: {
        Value val = Evaluate(*node.getArgument());
        if (!val.isString()) {
            throw RuntimeError("Команда 'bind' ожидает строковое выражение");
        }
        robot_.bind(val.asString());
        break;
    }
    }
}

void Interpreter::visit(const ExprNode&) {};
void Interpreter::visit(const StatementNode&) {};