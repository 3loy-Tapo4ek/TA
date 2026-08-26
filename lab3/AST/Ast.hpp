#pragma once

//basic nodes
#include "Node.hpp"
#include "ExprNode.hpp"
#include "StatementNode.hpp"
#include "INodeVisitor.hpp"

//expression nodes
#include "ArrayAccessNode.hpp"
#include "BinaryOpNode.hpp"
#include "CallNode.hpp"
#include "NumberNode.hpp"
#include "StringNode.hpp"
#include "UnaryOpNode.hpp"
#include "VariableNode.hpp"
#include "AssignNode.hpp"

//statement nodes
#include "BlockNode.hpp"
#include "CheckZeroNode.hpp"
#include "FunctionDeclNode.hpp"
#include "ReturnNode.hpp"
#include "RobotCommandNode.hpp"
#include "VariableDeclNode.hpp"
#include "WhileNode.hpp"
#include "ExprStatementNode.hpp"