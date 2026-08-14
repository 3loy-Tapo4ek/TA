cmake_minimum_required(VERSION 3.28)

set(VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(regex_library VERSION ${VERSION} LANGUAGES CXX
        DESCRIPTION "Robot maze game"
)

set(CORE_SOURCES
        AST/Nodes/BasicNodes/Node.hpp
        AST/Nodes/BasicNodes/ExprNode.hpp
        AST/Nodes/BasicNodes/StatementNode.hpp
        AST/Nodes/ExpressionNodes/ArrayAccessNode.hpp
        AST/Nodes/ExpressionNodes/BinaryOpNode.hpp
        AST/Nodes/ExpressionNodes/CallNode.hpp
        AST/Nodes/ExpressionNodes/NumberNode.hpp
        AST/Nodes/ExpressionNodes/StringNode.hpp
        AST/Nodes/ExpressionNodes/UnaryOpNodes.hpp
        AST/Nodes/ExpressionNodes/VariableNode.hpp
        AST/Nodes/StatementNodes/AssignNode.hpp
        AST/Nodes/StatementNodes/BlockNode.hpp
        AST/Nodes/StatementNodes/CheckZeroNode.hpp
        AST/Nodes/StatementNodes/FunctionDeclNode.hpp
        AST/Nodes/StatementNodes/ReturnNode.hpp
        AST/Nodes/StatementNodes/RobotCommandNode.hpp
        AST/Nodes/StatementNodes/VariableDeclNode.hpp
        AST/Nodes/StatementNodes/WhileNode.hpp
        Interpreter/Value.hpp
        Inrerpreter/Environment.hpp
        Inrerpreter/Environment.cpp
)

set(INCLUDE_DIRS
        "${CMAKE_SOURCE_DIR}/"
        "${CMAKE_SOURCE_DIR}/AST/"
        "${CMAKE_SOURCE_DIR}/AST/Nodes"
        "${CMAKE_SOURCE_DIR}/AST/Nodes/BasicNodes"
        "${CMAKE_SOURCE_DIR}/AST/Nodes/ExpressionNodes"
        "${CMAKE_SOURCE_DIR}/AST/Visitor"
        "${CMAKE_SOURCE_DIR}/Interpreter"
)

add_executable(maze_game main.cpp ${CORE_SOURCES})
target_include_directories(maze_game PRIVATE ${INCLUDE_DIRS})
