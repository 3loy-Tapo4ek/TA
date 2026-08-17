%skeleton "lalr1.cc"
%require "3.2"

%locations

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>
    #include <vector>
    #include <memory>
    #include <cstdint>
    #include "Ast.hpp"

    namespace yy { class parser; }
}

%code {
    #define YY_DECL yy::parser::symbol_type yylex ()
    YY_DECL;

    extern std::unique_ptr<StatementNode> root;
}

%start program
/* Terminals a.k.a TOKENS */
%token <int> INT_LITERAL
%token <std::string> STRING_LITERAL IDENTIFIER

/* TYPES and MODIFICATORS */
%token MUTABLE INTEGER STRING POINTER ARRAY OF

/* BLOCKS */
%token START FINISH WHILE INSTEAD CHECKZERO BREAK

/* FUNCTIONS and ITERATORS */
%token CALL FOR WITH AS ITERATOR RETURN CURRENT LAST

/* TOBOT COMMANDS */
%token TOP BOTTOM LEFT RIGHT TIMESHIFT BIND

/* OPERATORNS and SEPARATORS */
%token ASSIGN PLUS MINUS STAR SLASH PERCENT
%token AMPERSAND QUESTION EQUAL LESS GREATER
%token OPENPAREN CLOSEPAREN OPENBRACKET CLOSEBRACKET COMMA SEMICOLON

/*  (*,/,%),(+,-),(=,>,<),(:=) */

%right ASSIGN
%left EQUAL LESS GREATER
%left PLUS MINUS
%left STAR SLASH PERCENT

%precedence UMINUS DEREF ADDR SIZE
%precedence THEN
%precedence INSTEAD

/* Not Termnitals */
%type <std::unique_ptr<StatementNode>> program statement block_statement declaration_statement
%type <std::vector<std::unique_ptr<StatementNode>>> statement_list

%type <std::unique_ptr<ExprNode>> expression

%type <std::vector<std::unique_ptr<ExprNode>>> expression_list
%type <std::vector<std::unique_ptr<ExprNode>>> expr_seq

%type <DataType> type_spec

%type <std::vector<std::pair<DataType, std::string>>> param_list param_seq
%type <std::pair<DataType, std::string>> param
%type <std::unique_ptr<StatementNode>> function_declaration


/* GRAMMAR RULES GRAMMAR RULES GRAMMAR RULES GRAMMAR RULES GRAMMAR RULES */

%%
program:
    statement_list { root = std::make_unique<BlockNode>(std::move($1)); $$ = nullptr;}
    ;

/* rules for EXPRESSIONS */
expression:
        /* LITERALS and VARIABLES */
        INT_LITERAL {$$ = std::make_unique<NumberNode>($1);}
        |
        STRING_LITERAL {$$ = std::make_unique<StringNode>(std::move($1));}
        |
        IDENTIFIER {$$ = std::make_unique<VariableNode>(std::move($1));}
        |
        CURRENT {$$ = std::make_unique<VariableNode>("current");}
        |
        LAST {$$ = std::make_unique<StringNode>("last");}
        /* PARENS */
        |
        OPENPAREN expression CLOSEPAREN {$$ = std::move($2);}
        /* BINARY OPERATIONS */
        |
        expression PLUS expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Add, std::move($1), std::move($3));}
        |
        expression MINUS expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Subtract, std::move($1), std::move($3));}
        |
        expression STAR expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Multiply, std::move($1), std::move($3));}
        |
        expression SLASH expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Divide, std::move($1), std::move($3));}
        |
        expression PERCENT expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Remainder, std::move($1), std::move($3));}
        |
        expression EQUAL expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::Equal, std::move($1), std::move($3));}
        |
        expression LESS expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::LessThan, std::move($1), std::move($3));}
        |
        expression GREATER expression {$$ = std::make_unique<BinaryOpNode>(BinaryOp::GreaterThan, std::move($1), std::move($3));}
        /* Unary OPERATIONS */
        |
        STAR expression %prec DEREF {$$ = std::make_unique<UnaryOpNode>(UnaryOp::Dereference, std::move($2));}
        |
        AMPERSAND expression %prec ADDR {$$ = std::make_unique<UnaryOpNode>(UnaryOp::AddressOf, std::move($2));}
        |
        QUESTION expression %prec SIZE {$$ = std::make_unique<UnaryOpNode>(UnaryOp::SizeOf, std::move($2));}
        /* ACCESSING array/string */
        |
        IDENTIFIER OPENBRACKET expression CLOSEBRACKET {$$ = std::make_unique<ArrayAccessNode>(std::move($1), std::move($3));}
        /* FUNCTION or ITERATOR call */
        |
        CALL IDENTIFIER WITH OPENBRACKET expression_list CLOSEBRACKET {$$ = std::make_unique<CallNode>(std::move($2), "", std::move($5));}
        |
        CALL IDENTIFIER FOR IDENTIFIER WITH OPENBRACKET expression_list CLOSEBRACKET {$$ = std::make_unique<CallNode>(std::move($2), std::move($4), std::move($7));}
        |
        /* ASSIGN */
        expression ASSIGN expression {$$ = std::make_unique<AssignNode>(std::move($1), std::move($3));}
        /* ROBOT COMMANDS */
        |
        TOP    { $$ = std::make_unique<RobotCommandNode>(RobotAction::Top); }
        |
        BOTTOM { $$ = std::make_unique<RobotCommandNode>(RobotAction::Bottom); }
        |
        LEFT   { $$ = std::make_unique<RobotCommandNode>(RobotAction::Left); }
        |
        RIGHT  { $$ = std::make_unique<RobotCommandNode>(RobotAction::Right); }
        |
        TIMESHIFT expression {$$ = std::make_unique<RobotCommandNode>(RobotAction::Timeshift, std::move($2));}
        |
        BIND expression {$$ = std::make_unique<RobotCommandNode>(RobotAction::Bind, std::move($2));}
        ;
expression_list:
        %empty { $$ = std::vector<std::unique_ptr<ExprNode>>(); }
        |
        expr_seq { $$ = std::move($1); }
        ;

expr_seq:
        expression {
                $$ = std::vector<std::unique_ptr<ExprNode>>();
                $$.push_back(std::move($1));
                }
        |
        expr_seq COMMA expression  {
                            $$ = std::move($1);
                            $$.push_back(std::move($3));
                                }
        ;

/* rules for STATEMENTS */
block_statement:
        START statement_list FINISH {$$ = std::make_unique<BlockNode>(std::move($2));}
        ;
statement:
        block_statement {$$ = std::move($1);}
        | 
        declaration_statement { $$ = std::move($1); }
        |
        function_declaration { $$ = std::move($1); }
        |
        expression SEMICOLON {$$ = std::make_unique<ExprStatementNode>(std::move($1));}
        /* WHILE */
        |
        WHILE OPENPAREN expression CLOSEPAREN statement %prec THEN {$$ = std::make_unique<WhileNode>(std::move($3), std::move($5), std::move(nullptr));}
        |
        WHILE OPENPAREN expression CLOSEPAREN statement INSTEAD statement {$$ = std::make_unique<WhileNode>(std::move($3), std::move($5), std::move($7));}
        /* CHECKZERO */
        |
        CHECKZERO OPENPAREN expression CLOSEPAREN statement %prec THEN {$$ = std::make_unique<CheckZeroNode>(std::move($3), std::move($5), std::move(nullptr));}
        |
        CHECKZERO OPENPAREN expression CLOSEPAREN statement INSTEAD statement {$$ = std::make_unique<CheckZeroNode>(std::move($3), std::move($5), std::move($7));}
        /* RETURN from function */
        |
        RETURN expression SEMICOLON {$$ = std::make_unique<ReturnNode>(std::move($2));}
        |
        BREAK SEMICOLON {$$ = nullptr;}
        ;

statement_list:
        %empty { $$ = std::vector<std::unique_ptr<StatementNode>>(); }
        |
        statement_list statement {
                            $$ = std::move($1);
                            $$.push_back(std::move($2));
                                }
        ;

declaration_statement:
        /* With initiate */
        MUTABLE type_spec IDENTIFIER ASSIGN expression SEMICOLON {$$ = std::make_unique<VariableDeclNode>($2, std::move($3), true, std::move($5));}
        |
        type_spec IDENTIFIER ASSIGN expression SEMICOLON {$$ = std::make_unique<VariableDeclNode>($1, std::move($2), false, std::move($4));}
        /* Without initiate */
        |
        MUTABLE type_spec IDENTIFIER SEMICOLON {$$ = std::make_unique<VariableDeclNode>($2, std::move($3), true);}
        /* Arrays: [mutable] array of <type> <name>[<size>]; */
        |
        MUTABLE ARRAY OF type_spec IDENTIFIER OPENBRACKET expression CLOSEBRACKET SEMICOLON {$$ = std::make_unique<VariableDeclNode>(DataType::Array, std::move($5), true, nullptr, std::move($7));}
        |
        ARRAY OF type_spec IDENTIFIER OPENBRACKET expression CLOSEBRACKET SEMICOLON {$$ = std::make_unique<VariableDeclNode>(DataType::Array, std::move($4), false, nullptr, std::move($6));}
        ;

/* rules for VARIABLES and FUNCTION declaration*/
type_spec:
        INTEGER { $$ = DataType::Integer; }
        |
        STRING  { $$ = DataType::String; }
        |
        POINTER { $$ = DataType::Pointer; }
        ;

function_declaration:
        /* Just function */
        type_spec IDENTIFIER OPENPAREN param_list CLOSEPAREN block_statement {$$ = std::make_unique<FunctionDeclNode>($1, std::move($2), std::move($4), false, false, DataType::Integer, std::move($6));}
        /* Function-iterator */
        |
        type_spec IDENTIFIER OPENPAREN param_list CLOSEPAREN AS ITERATOR FOR type_spec block_statement {$$ = std::make_unique<FunctionDeclNode>($1, std::move($2), std::move($4), true, false, $9, std::move($10));}
        | 
        type_spec IDENTIFIER OPENPAREN param_list CLOSEPAREN AS MUTABLE ITERATOR FOR type_spec block_statement {$$ = std::make_unique<FunctionDeclNode>($1, std::move($2), std::move($4), true, true, $10, std::move($11));}
        ;

param_list:
        %empty { $$ = std::vector<std::pair<DataType, std::string>>(); }
        |
        param_seq { $$ = std::move($1); }
        ;

param_seq:
        param   {
            $$ = std::vector<std::pair<DataType, std::string>>();
            $$.push_back(std::move($1));
                }
        |
        param_seq COMMA param {
            $$ = std::move($1);
            $$.push_back(std::move($3));
            }
        ;

param:
    type_spec IDENTIFIER { $$ = std::make_pair($1, std::move($2)); }
    ;
%%

void yy::parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "Syntax Error at line " << loc.begin.line 
              << ", column " << loc.begin.column << ": " << msg << std::endl;
}