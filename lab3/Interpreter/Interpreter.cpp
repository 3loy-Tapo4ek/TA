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
    try
    {
        Execute(root);
    }
    catch (const ExitReachedSignal&) {}
};

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
};

//visitors for basic expressions

//NumberNode
void Interpreter::visit(const NumberNode& node)
{
    last_evaluated_value_ = Value(node.getValue());
};

//StringNode
void Interpreter::visit(const StringNode& node)
{
    last_evaluated_value_ = Value(node.getValue());
};

//VariableNode
void Interpreter::visit(const VariableNode& node)
{
    last_evaluated_value_ = current_environment_->resolve(node.getName()).value;
};

//ExprStatementNode
void Interpreter::visit(const ExprStatementNode& node)
{
    Evaluate(*node.getExpression());
};

//ArrayAccessNode
void Interpreter::visit(const ArrayAccessNode& node)
{
    const auto& array = current_environment_->resolve(node.getArrayName());

    Value id = Evaluate(*node.getIndex());

    if (!id.isInt() || id.asInt() < 0) { throw RuntimeError("Индекс массива должен быть неотрицательным целым числом");}
    if (id.asInt() >= array.value.asArray().size()) {throw RuntimeError("Выход за границы массива");}
    
    last_evaluated_value_ = array.value.asArray()[id.asInt()];
}

//visiting operations

//BinaryOpNode
void Interpreter::visit(const BinaryOpNode& node)
{
    Value left = Evaluate(*node.getLeft());
    Value right = Evaluate(*node.getRight());
    last_evaluated_value_ = evaluateBinary(node.getOperation(), left, right);
}

//UnaryOpNode
void Interpreter::visit(const UnaryOpNode& node)
{
    Value operand_val;
    if (node.getOperation() != UnaryOp::AddressOf)
    {
        operand_val = Evaluate(*node.getOperand());
    }
    last_evaluated_value_ = evaluateUnary(node.getOperation(), node.getOperand().get(), operand_val);
};

//visiting statements

//BlockNode
void Interpreter::visit(const BlockNode& node)
{
   ScopeGuard scope(current_environment_, std::make_shared<Environment>(current_environment_));
    
    for (const auto& statement : node.getStatements()) 
    {
        if (statement) {Execute(*statement);}
    }
};

//CheckZeroNode
void Interpreter::visit(const CheckZeroNode& node)
{
    Value condition = Evaluate(*node.getCondition());
    if (!condition.isInt()) {throw RuntimeError("Условие checkzero должно быть типа int");}

    if (condition.asInt() == 0){ Execute(*node.getBody());}
    else if (condition.asInt() != 0 && node.getInsteadBody()) {Execute(*node.getInsteadBody());}
};

//WhileNode
void Interpreter::visit(const WhileNode& node)
{
    bool is_first_iteration = true;

    while (true)
    {
        Value condition = Evaluate(*node.getCondition());
        if (!condition.isInt()) {throw RuntimeError("Условие цикла while должно быть типа int");}
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
};

//ReturnNode
void Interpreter::visit(const ReturnNode& node)
{
    Value return_value;
    if (node.getExpression()) 
    {
        return_value = Evaluate(*node.getExpression());
    }

    throw ReturnSignal(std::move(return_value));
}

//VariavleDeclNode
void Interpreter::visit(const VariableDeclNode& node)
{
    if (node.getType() == DataType::Array)
    {
        int size = 10;
        if (node.getArraySize())
        {
            size = Evaluate(*node.getArraySize()).asInt();
        }

        Value array = getDefaultValue(DataType::Array);
        array.asArray().resize(size);
        current_environment_->Define(node.getName(), array, node.isMutable());
    }
    else
    {
        if (!node.isMutable() && !node.getInitialValue())
        {
            throw RuntimeError("Константа '" + node.getName() + "' должна быть инициализирована");
        }

        Value variable = getDefaultValue(node.getType());
        if (node.getInitialValue())
        {
            variable = Evaluate(*node.getInitialValue());
        }

        current_environment_->Define(node.getName(), variable, node.isMutable());
    }
};

//AssignNode
void Interpreter::visit(const AssignNode& node)
{
    Value rhs = Evaluate(*node.getValue());
    executeAssignment(*node.getTarget(), rhs);
    last_evaluated_value_ = rhs;
}

//FunctionDeclNode
void Interpreter::visit(const FunctionDeclNode& node)
{
    current_environment_->defineFunction(node.getName(), node, current_environment_);
}

//CallNode
void Interpreter::visit(const CallNode& node)
{
    const auto& function = current_environment_->resolveFunction(node.getFuncName());
    if (!node.getTarget().empty())
    {
        executeIteratorCall(node, function);
    }
    else
    {
        executeFunctionCall(node, function);
    }
}

//RobotCommandNode
void Interpreter::visit(const RobotCommandNode& node)
{
    switch (node.getAction())
    {
        case RobotAction::Top:
        {
            executeMove(Direction::Up);
            return;
        }
        case RobotAction::Bottom:
        {
            executeMove(Direction::Down);
            return;
        }
        case RobotAction::Left:
        {
            executeMove(Direction::Left);
            return;
        }
        case RobotAction::Right:
        {
            executeMove(Direction::Right);
            return;
        }
        case RobotAction::Timeshift:
        {
            executeTimeshift(node.getArgument().get());
            return;
        }
        case RobotAction::Bind:
        {
            executeBind(node.getArgument().get());
            return;
        }
    }

    throw RuntimeError("Неизвестная команда робота");
};

//helpers

void Interpreter::executeMove(Direction dir)
{
    bool success = robot_.move(dir);
    last_evaluated_value_ = makeBoolValue(success);

    if (robot_.isExitReached())
    {
        throw ExitReachedSignal{};
    }
};

void Interpreter::executeTimeshift(const ExprNode* arg_node)
{
    if (!arg_node)
    {
        throw RuntimeError("Команда 'timeshift' требует аргумент");
    }

    Value val = Evaluate(*arg_node);
    if (val.isInt())
    {
        robot_.timeshift(val.asInt());
        return;
    }

    if (val.isString())
    {
        robot_.timeshift(val.asString());
        return;
    }

    throw RuntimeError("Команда 'timeshift' ожидает целое число или строку");
};

void Interpreter::executeBind(const ExprNode* arg_node)
{
    if (!arg_node)
    {
        throw RuntimeError("Команда 'bind' требует аргумент");
    }

    Value val = Evaluate(*arg_node);
    if (!val.isString())
    {
        throw RuntimeError("Команда 'bind' ожидает строку");
    }

    robot_.bind(val.asString());
};

//returns result of binary operation
Value Interpreter::evaluateBinary(BinaryOp op, const Value& left, const Value& right)
{
    if (op == BinaryOp::GreaterThan || op == BinaryOp::LessThan || op == BinaryOp::Equal)
    {
        return evaluateComparison(op, left, right);
    }

    else if (left.isPointer() || right.isPointer())
    {
        return evaluatePointerArifmetic(op, left, right);
    }

    return evaluateArifmetic(op, left, right);
};

//hadles {+, -, *, /, %} operations
Value Interpreter::evaluateArifmetic(BinaryOp op, const Value& left, const Value& right)
{
    switch (op)
    {
        case BinaryOp::Add:
            {
                if (left.isInt() && right.isInt()) return Value(left.asInt() + right.asInt());
                if (left.isString() && right.isString()) return Value(left.asString() + right.asString());
        
                throw RuntimeError("Неверные типы для операции '+'");
            }
        case BinaryOp::Subtract:
            {      
                if (left.isInt() && right.isInt()) return Value(left.asInt() - right.asInt());
                throw RuntimeError("Неверные типы для операции '-'");
                break;
            }
        case BinaryOp::Multiply:
            {
                if (left.isInt() && right.isInt()) return Value(left.asInt() * right.asInt());
                throw RuntimeError("Умножение применимо только к integer");
            }
        case BinaryOp::Divide:
            {   
                if (!left.isInt() || !right.isInt()) throw RuntimeError("Деление применимо только к integer");
                if (right.asInt() == 0) throw RuntimeError("Деление на ноль");
                return Value(left.asInt() / right.asInt());
            }

        case BinaryOp::Remainder:
            {
                if (!left.isInt() || !right.isInt()) throw RuntimeError("Остаток от деления применим только к integer");
                if (right.asInt() == 0) throw RuntimeError("Деление на ноль");
                return Value(left.asInt() % right.asInt());
            }
        default:
            throw RuntimeError("Неизвестная бинарная операция");
    }
};

//handles pointer arifmetic
Value Interpreter::evaluatePointerArifmetic(BinaryOp op, const Value& left, const Value& right)
{
    switch (op)
    {
        case BinaryOp::Add:
            {   
                if (left.isPointer() && right.isInt())
                {
                    Pointer p = left.asPointer(); 
                    p.offset_ += right.asInt();
                    return Value(p);
                }
                
                if (left.isInt() && right.isPointer())
                {
                    Pointer p = right.asPointer();
                    p.offset_ += left.asInt();
                    return Value(p);
                }        
                throw RuntimeError("Неверные типы для операции '+'");
            }
        case BinaryOp::Subtract:
            {      
                if (left.isPointer() && right.isInt())
                {
                    Pointer p = left.asPointer();
                    p.offset_ -= right.asInt();
                    return Value(p);
                }
                throw RuntimeError("Неверные типы для операции '-'");
            }
        default:
            throw RuntimeError("Неизвестная бинарная операция");
    }
};

//handles boolean comparisons
Value Interpreter::evaluateComparison(BinaryOp op, const Value& left, const Value& right)
{
    switch (op)
    {
        case BinaryOp::Equal:
            {
                if (left.isInt() && right.isInt()) return makeBoolValue(left.asInt() == right.asInt());
                if (left.isString() && right.isString()) return makeBoolValue(left.asString() == right.asString());
                if (left.isPointer() && right.isPointer()) return makeBoolValue(left.asPointer() == right.asPointer());
                throw RuntimeError("Нельзя сравнивать значения разных типов");
            }

        case BinaryOp::LessThan:
            {
                if (left.isInt() && right.isInt()) return makeBoolValue(left.asInt() < right.asInt());
                if (left.isString() && right.isString()) return makeBoolValue(left.asString() < right.asString());
                throw RuntimeError("Оператор '<' применим только к числам и строкам одного типа");
            }
        case BinaryOp::GreaterThan:
            {
                if (left.isInt() && right.isInt()) return makeBoolValue(left.asInt() > right.asInt());
                if (left.isString() && right.isString()) return makeBoolValue(left.asString() > right.asString());
                throw RuntimeError("Оператор '>' применим только к числам и строкам одного типа");
            }
    default:
            throw RuntimeError("Неизвестная бинарная операция");
    }
};

Value Interpreter::evaluateUnary(UnaryOp op, const ExprNode* operand_node, const Value& val) {
    switch (op)
    {
        case UnaryOp::AddressOf:
            {
                return evaluateAddressOf(operand_node);
            }
        case UnaryOp::SizeOf:
            {
                return evaluateSizeOf(val);
            }
        case UnaryOp::Dereference:
            {
                return evaluateDereference(val);
            }
    }
    throw RuntimeError("Неизвестная унарная операция");
};

Value Interpreter::evaluateAddressOf(const ExprNode* operand_node)
{
    if (const auto* var_node = dynamic_cast<const VariableNode*>(operand_node))
    {
        return Value(Pointer{var_node->getName(), 0});
    }

    if (const auto* arr_node = dynamic_cast<const ArrayAccessNode*>(operand_node))
    {
        Value id = Evaluate(*arr_node->getIndex());
        if (!id.isInt() || id.asInt() < 0) {throw RuntimeError("Индекс массива должен быть неотрицательным числом");}
        
        return Value(Pointer{arr_node->getArrayName(), static_cast<size_t>(id.asInt())});
    }

    throw RuntimeError("Оператор '&' применим только к переменным и элементам массивов");
};

Value Interpreter::evaluateSizeOf(const Value& val)
{
    if (val.isArray())
    {
        return Value(static_cast<int>(val.asArray().size()));
    } 
    else
    {
        return Value(1);
    }
};

Value Interpreter::evaluateDereference(const Value& val)
{
    if (!val.isPointer()) {throw RuntimeError("Оператор '*' применим только к указателям");}
        
    const Pointer& ptr = val.asPointer();
        
    auto& target = current_environment_->resolve(ptr.target_name_);

    if (target.value.isArray())
    {
        const auto& arr = target.value.asArray();
        if (ptr.offset_ >= arr.size()) {throw RuntimeError("Разыменование указателя за пределами массива");}
        
        return arr[ptr.offset_];
    }
    
    if (ptr.offset_ != 0) {throw RuntimeError("Смещение указателя недопустимо для скаляра");}
    return target.value;
};

void Interpreter::executeAssignment(const ExprNode& target, const Value& value)
{
    if (const auto* var_node = dynamic_cast<const VariableNode*>(&target))
    {
        assignToVariable(*var_node, value);
        return;
    }

    if (const auto* arr_node = dynamic_cast<const ArrayAccessNode*>(&target)) {
        assignToArray(*arr_node, value);
        return;
    }

    if (const auto* unary_node = dynamic_cast<const UnaryOpNode*>(&target))
    {
        if (unary_node->getOperation() == UnaryOp::Dereference)
        {
            assignToDereference(*unary_node, value);
            return;
        }
    }

    throw RuntimeError("Недопустимая левая часть оператора присваивания");
}
void Interpreter::assignToVariable(const VariableNode& node, const Value& value)
{
    auto& variable = current_environment_->resolve(node.getName());
    if (!variable.is_mutable) 
    {
        throw RuntimeError("Нельзя изменить константу '" + node.getName() + "'");
    }

    variable.value = value;
};

void Interpreter::assignToArray(const ArrayAccessNode& node, const Value& value)
{
    auto& array = current_environment_->resolve(node.getArrayName());
    if (!array.is_mutable)
    {
        throw RuntimeError("Нельзя изменить константный массив '" + node.getArrayName() + "'");
    }

    Value id = Evaluate(*node.getIndex());
    
    if (!id.isInt() || id.asInt() < 0)
    {
        throw RuntimeError("Индекс массива должен быть неотрицательным числом");
    }

    auto& arr = array.value.asArray();
    
    if (id.asInt() >= arr.size())
    {
        arr.resize(((id.asInt() / 10) + 1) * 10, Value(0));
    }

    arr[id.asInt()] = value;
};

void Interpreter::assignToDereference(const UnaryOpNode& node, const Value& value)
{
    Value ptr_val = Evaluate(*node.getOperand());
    if (!ptr_val.isPointer())
    {
        throw RuntimeError("Оператор '*' применим только к указателям");
    }

    const Pointer& ptr = ptr_val.asPointer();
    auto& target = current_environment_->resolve(ptr.target_name_);

    if (!target.is_mutable)
    {
        throw RuntimeError("Нельзя изменить константу '" + ptr.target_name_ + "' через указатель");
    }

    if (target.value.isArray())
    {
        auto& array = target.value.asArray();
        if (ptr.offset_ >= array.size())
        {
            throw RuntimeError("Запись по указателю за пределами массива");
        }
        array[ptr.offset_] = value;
        return;
    }

    if (ptr.offset_ != 0)
    {
        throw RuntimeError("Некорректное смещение указателя для скаляра");
    }

    target.value = value;
}


void Interpreter::executeFunctionCall(const CallNode& node, const Function& fn)
{
    const auto& decl = fn.declaration_;
    auto args = evaluateArguments(node.getArguments());

    if (args.size() != decl.getParameters().size())
    {
        throw RuntimeError("Неверное количество аргументов при вызове '" + node.getFuncName() + "'");
    }

    auto fn_env = std::make_shared<Environment>(fn.closure_);
    for (size_t i = 0; i < decl.getParameters().size(); ++i)
    {
        fn_env->Define(decl.getParameters()[i].second, std::move(args[i]), true);
    }

    ScopeGuard scope(current_environment_, fn_env);

    Value ret_val;
    try
    {
        Execute(*decl.getBody());
    }
    catch (const ReturnSignal& sig)
    {
        ret_val = sig.value;
    }

    last_evaluated_value_ = std::move(ret_val);
}

void Interpreter::executeIteratorCall(const CallNode& node, const Function& fn)
{
    const auto& decl = fn.declaration_;
    if (!decl.isIterator())
    {
        throw RuntimeError("Функция '" + node.getFuncName() + "' не является iterator");
    }

    auto& target_var = current_environment_->resolve(node.getTarget());
    auto extra_args = evaluateArguments(node.getArguments());

    size_t last_idx = 0;
    if (target_var.value.isArray())
    {
        auto& arr = target_var.value.asArray();
        for (size_t i = 0; i < arr.size(); ++i)
        {
            last_idx = i;
            bool should_continue = runIteratorStep(decl, fn.closure_, arr[i], extra_args);
            if (!should_continue)
            {
                break;
            }
        }
    }

    current_environment_->Define("last", Value(static_cast<int>(last_idx)), true);
    last_evaluated_value_ = Value(static_cast<int>(last_idx));
}

bool Interpreter::runIteratorStep(const FunctionDeclNode& decl, std::shared_ptr<Environment> closure, Value& element, const std::vector<Value>& extra_args)
{
    auto iter_env = std::make_shared<Environment>(closure);
    iter_env->Define("current", element, decl.isMutableIterator());

    for (size_t p = 0; p < extra_args.size() && p < decl.getParameters().size(); p++)
    {
        iter_env->Define(decl.getParameters()[p].second, extra_args[p], true);
    }

    ScopeGuard scope(current_environment_, iter_env);

    Value return_val(1);
    try
    {
        Execute(*decl.getBody());
    }
    catch (const ReturnSignal& sig)
    {
        return_val = sig.value;
    }

    if (decl.isMutableIterator())
    {
        element = iter_env->get("current");
    }

    return !(return_val.isInt() && return_val.asInt() == 0);
};

std::vector<Value> Interpreter::evaluateArguments(const std::vector<std::unique_ptr<ExprNode>>& args)
{
    std::vector<Value> evaluated;
    evaluated.reserve(args.size());
    for (const auto& arg : args)
    {
        evaluated.push_back(Evaluate(*arg));
    }
    return evaluated;
}

//return default value according to its datatype
Value Interpreter::getDefaultValue(DataType type)
{
    switch (type)
    {
        case DataType::Array:
            {
                int size = 10;

                Array new_array(size, Value(0));
                return Value(new_array);
            }
        case DataType::Integer:
            {
                return Value(0);
            }
        case DataType::String:
            {
                return Value("");
            }
        case DataType::Pointer:
            {
                return Value(Pointer());
            }
        default:
            throw RuntimeError("Неизвестный тип данных");
    }
}