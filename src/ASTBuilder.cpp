#include <memory>
#include <iostream>
#include <map>
#include <cstring>
#include "ASTBuilder.h"
#include "main.h"

void ASTBuilder::exitPrimitiveType(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    IrType* node =nullptr;
    if (*node_text == "i8" || *node_text == "i16" || *node_text == "i32" || *node_text == "i64" || *node_text == "i128") {
        node = new IrTypeInt(cst_node); // Assuming IrTypeInt can be reused for Rust's int types
        this->ast_stack.push(node);
    } 
    else if (*node_text == "u8" || *node_text == "u16" || *node_text == "u32" || *node_text == "u64" || *node_text == "u128") {
        node = new IrTypeUInt(cst_node); // Assuming IrTypeUInt for unsigned int types
        this->ast_stack.push(node);
    }
    else if (*node_text == "f32" || *node_text == "f64") {
        node = new IrTypeFloat(cst_node); 
        this->ast_stack.push(node);
    }
    else if (*node_text == "bool") {
        node = new IrTypeBool(cst_node); 
        this->ast_stack.push(node);
    }
    else if (*node_text == "char") {
        node = new IrTypeChar(cst_node);
        this->ast_stack.push(node);
    }
    else if (*node_text == "str") {
        node = new IrTypeString(cst_node);
        this->ast_stack.push(node);
    }
    else if (*node_text == "void") {
        node = new IrTypeVoid(cst_node);
        this->ast_stack.push(node);
    }
    else {
        std::cerr << "Error: Unknown primitive type" << std::endl;
    }
}

void ASTBuilder::exitArrayType(const TSNode & cst_node) {
    Ir* node = nullptr;
    // Use stack to get the type and size
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for array type" << std::endl;
    }

    IrExpr* arraySize = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* arrayType = dynamic_cast<IrType*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (arrayType && arraySize) {
        node = new IrTypeArray(arrayType, arraySize, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid array type or size" << std::endl;
    }
}

void ASTBuilder::exitIdentifier(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    IrIdent* node = new IrIdent(node_text, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitMutableSpec(const TSNode & cst_node) {
    Ir* node = new IrMutableSpec(cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParameter(const TSNode & cst_node) {
    Ir* node =nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for parameter declaration" << std::endl;
    }

    IrType* paramType = dynamic_cast<IrType*>( this->ast_stack.top());
    this->ast_stack.pop();

    IrIdent* paramName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    // Check for mutable binding
    IrMutableSpec* mut = nullptr;
    if (this->ast_stack.size() > 0) {
        mut = dynamic_cast<IrMutableSpec*>(this->ast_stack.top());
        if (mut) {
            this->ast_stack.pop();
        }
    }

    if (paramType && paramName) {
        node = new IrParamDecl(paramType, paramName, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid parameter type or name" << std::endl;
    }
}

void ASTBuilder::exitDeclaration(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name/pattern
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for declaration" << std::endl;
        return;
    }

    // Get the type if specified
    IrType* declType = nullptr;
    TSNode type_node = ts_node_child_by_field_name(cst_node, "type", 4);
    if (!ts_node_is_null(type_node)) {
        declType = dynamic_cast<IrType*>(this->ast_stack.top());
        if (declType) {
            this->ast_stack.pop();
        } else {
            std::cerr << "Error: Invalid declaration type" << std::endl;
            return;
        }
    }

    // Get the name/pattern
    IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    if (declName) {
        this->ast_stack.pop();
    } else {
        std::cerr << "Error: Invalid declaration name/pattern" << std::endl;
        return;
    }

    // Check for mutable binding
    IrMutableSpec* mut = nullptr;
    bool mutable_spec = false;
    if (this->ast_stack.size() > 0) {
        mut = dynamic_cast<IrMutableSpec*>(this->ast_stack.top());
        if (mut) {
            mutable_spec = true;
            this->ast_stack.pop();
        } 
    }

    // Create the declaration node
    node = new IrDecl(mutable_spec, declName, declType, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParamList(const TSNode & cst_node){

    IrParamList* paramList = new IrParamList(cst_node);
    IrParamDecl* paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());

    while (paramDecl) {
        this->ast_stack.pop();
        paramList->addToParamsList(paramDecl);
        if (this->ast_stack.empty()) break; // Prevent underflow
        paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());
    }

    this->ast_stack.push(paramList);
}

void ASTBuilder::exitFunctionDeclarator(const TSNode & cst_node){
//     Ir* node = nullptr;
//     // Use stack to get the type and name
//     if (this->ast_stack.size() < 2) {
//         std::cerr << "Error: Not enough elements on the stack for function declaration" << std::endl;
//     }

//     IrParamList* paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
//     this->ast_stack.pop();

//     IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
//     this->ast_stack.pop();

//     if (declName && paramList) {
//         node = new IrFunctionDecl(declName, paramList, cst_node);
//         this->ast_stack.push(node);
//     } else {
//         std::cerr << "Error: Invalid function declaration type or name" << std::endl;
//     }
}

void ASTBuilder::exitFunctionDefinition(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 3) {
        std::cerr << "Error: Not enough elements on the stack for function definition" << std::endl;
    }

    IrCompoundStmt* compoundStmt = dynamic_cast<IrCompoundStmt*>(this->ast_stack.top());
    this->ast_stack.pop(); // block

    // Check for return type (optional)
    IrType* returnType = nullptr;
    if (this->ast_stack.size() > 0) {
        returnType = dynamic_cast<IrType*>(this->ast_stack.top());
        if (returnType) {
            this->ast_stack.pop();
        }
    }
    // If return type is not specified, default to unit type `()`
    if (!returnType) {
        returnType = new IrTypeUnit(cst_node); 
    }

    IrParamList* paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
    this->ast_stack.pop(); 
    
    IrIdent* ident = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (ident && paramList && returnType && compoundStmt) {
        node = new IrFunctionDef(ident, paramList, returnType, compoundStmt, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid function definition" << std::endl;
    }
}

void ASTBuilder::exitBinaryExpr(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for binary expression" << std::endl;
    }

    IrExpr* rightOperand = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrExpr* leftOperand = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    // Get the operation
    // get the second child of the cst_node
    TSNode second_child = ts_node_child(cst_node, 1);
    std::string* operation = getNodeText(second_child);

    if (leftOperand && rightOperand) {
        node = new IrBinaryExpr(operation, leftOperand, rightOperand, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid binary expression" << std::endl;
    }
}

void ASTBuilder::exitUnaryExpr(const TSNode & cst_node) {
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for unary expression" << std::endl;
    }

    IrExpr* operand = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    // Get the operation
    TSNode second_child = ts_node_child(cst_node, 0);
    std::string* operation = getNodeText(second_child);

    if (operand) {
        node = new IrUnaryExpr(operation, operand, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid unary expression" << std::endl;
    }
}

void ASTBuilder:: exitReturnStatement(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for return statement" << std::endl;
    }

    IrExpr* result = dynamic_cast<IrExpr*>(this->ast_stack.top());

    if (result) {
        this->ast_stack.pop();
        node = new IrStmtReturnExpr(result,cst_node);
        this->ast_stack.push(node);
    } 
    else if(ts_node_named_child_count(cst_node) == 0) {
        node = new IrStmtReturnVoid(cst_node);
        this->ast_stack.push(node);
    }
    else {
        std::cerr << "Error: Invalid return statement" << std::endl;
    }
}

void ASTBuilder:: exitCompoundStatement(const TSNode & cst_node){
    IrCompoundStmt* node = nullptr;

    // optional expression
    if (this->ast_stack.size() > 0) {
        IrExpr* expr = dynamic_cast<IrExpr*>(this->ast_stack.top());
        node = new IrCompoundStmt(expr, cst_node);
        if (expr) {
            this->ast_stack.pop();
        }
    }
    
    IrStatement* stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    while (stmt) {
        this->ast_stack.pop();
        node->addStmtToFront(stmt);
        stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    }
    this->ast_stack.push(node);
}

void ASTBuilder::exitLiteralNumber(const TSNode & cst_node){
    std::string* node_text = getNodeText(cst_node);
    IrLiteralNumber* node = new IrLiteralNumber(std::stoi(*node_text), cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitArgList(const TSNode & cst_node){
    IrArgList* argList = new IrArgList(cst_node);
    uint32_t arg_count = ts_node_named_child_count(cst_node);
    for (uint32_t i = 0; i < arg_count; i++) {
        IrExpr* arg = dynamic_cast<IrExpr*>(this->ast_stack.top());
        this->ast_stack.pop();
        argList->addToArgsList(arg);
    }
    this->ast_stack.push(argList);
}

void ASTBuilder::exitCallExpr(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for call expression" << std::endl;
    }

    IrArgList* argList = dynamic_cast<IrArgList*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrIdent* functionName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (functionName && argList) {
        node = new IrCallExpr(functionName, argList, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid call expression" << std::endl;
    }
}

void ASTBuilder::exitMatchExpr(const TSNode & cst_node) {
    // Ensure there are enough elements on the stack
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for match expression" << std::endl;
        return;
    }

    // Pop the match arms from the stack
    std::vector<IrMatchArm*> arms;
    while (!this->ast_stack.empty() && dynamic_cast<IrMatchArm*>(this->ast_stack.top())) {
        IrMatchArm* arm = dynamic_cast<IrMatchArm*>(this->ast_stack.top());
        this->ast_stack.pop();
        arms.insert(arms.begin(), arm); // Insert at the beginning to maintain order
    }

    // Pop the match expression from the stack
    Ir* matchExpr = this->ast_stack.top();
    this->ast_stack.pop();

    // Create the match expression node
    IrMatchExpr* matchExprNode = new IrMatchExpr(matchExpr, arms, cst_node);
    this->ast_stack.push(matchExprNode);
}

void ASTBuilder::exitLoopExpr(const TSNode & cst_node) {
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for loop expression" << std::endl;
        return;
    }

    Ir* body = this->ast_stack.top();
    this->ast_stack.pop();

    IrLoopExpr* loopExpr = new IrLoopExpr(body, cst_node);
    this->ast_stack.push(loopExpr);
}

void ASTBuilder::exitWhileExpr(const TSNode & cst_node) {
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for while expression" << std::endl;
        return;
    }

    Ir* body = this->ast_stack.top();
    this->ast_stack.pop();

    Ir* condition = this->ast_stack.top();
    this->ast_stack.pop();

    IrWhileExpr* whileExpr = new IrWhileExpr(condition, body, cst_node);
    this->ast_stack.push(whileExpr);
}

void ASTBuilder::exitForExpr(const TSNode & cst_node) {
    Ir* node = nullptr;
    if (this->ast_stack.size() < 3) {
        std::cerr << "Error: Not enough elements on the stack for for expression" << std::endl;
        return;
    }

    IrCompoundStmt* body = dynamic_cast<IrCompoundStmt*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrExpr* condition = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrIdent* ident = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (ident && condition && body) {
        node = new IrForExpr(ident, condition, body, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid for expression" << std::endl;
    }
}

void ASTBuilder::exitAssignExpr(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for assign expression" << std::endl;
    }

    IrExpr* rhs = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrExpr* lhs = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (lhs && rhs) {
        node = new IrAssignExpr(lhs, rhs, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid assign expression" << std::endl;
    }
}

void ASTBuilder::exitIfExpr(const TSNode & cst_node){
    Ir* node = nullptr;
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for if expression" << std::endl;
    }

    // Check for else clause (optional)
    IrExpr* elseBlock = nullptr;
    if (ts_node_named_child_count(cst_node) == 3) {
        elseBlock = dynamic_cast<IrElseClause*>(this->ast_stack.top());
        this->ast_stack.pop();
    }

    Ir* thenBlock = dynamic_cast<Ir*>(this->ast_stack.top());
    this->ast_stack.pop();

    Ir* condition = dynamic_cast<Ir*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (condition && thenBlock && elseBlock) {
        node = new IrIfExpr(condition, thenBlock, elseBlock, cst_node);
        this->ast_stack.push(node);
    } else if (condition && elseBlock) {
        std::cerr << "Error: Invalid thenBlock" << std::endl;
    } else if (thenBlock) {
        std::cerr << "Error: Invalid condition" << std::endl;
    } else {
        std::cerr << "Error: Invalid if expression" << std::endl;
    }
}

void ASTBuilder::exitElseClause(const TSNode & cst_node) {
    Ir* node = nullptr;

    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for else clause" << std::endl;
        return;
    }

    // Get the else block or if expression
    IrExpr* elseExpr = dynamic_cast<IrExpr*>(this->ast_stack.top());
    IrStatement* elseStmt = nullptr;

    if (!elseExpr) {
        elseStmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    }

    this->ast_stack.pop();

    if (elseExpr) {
        node = new IrElseClause(elseExpr, cst_node);
    } else if (elseStmt) {
        node = new IrElseClause(elseStmt, cst_node);
    } else {
        std::cerr << "Error: Invalid else clause" << std::endl;
        return;
    }

    this->ast_stack.push(node);
}

void ASTBuilder::exitRangeExpr(const TSNode & cst_node) {
    Ir* node = nullptr;
    // Use stack to get the start and end
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for range expression" << std::endl;
    }

    IrExpr* end = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrExpr* start = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (start && end) {
        node = new IrRangeExpr(start, end, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid range expression" << std::endl;
    }
}

void ASTBuilder::exitExprStmt(const TSNode & cst_node){
    IrExprStmt* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for expression statement" << std::endl;
    }

    IrExpr* expr = dynamic_cast<IrExpr*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (expr) {
        node = new IrExprStmt(expr, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid expression statement" << std::endl;
    }
}

void ASTBuilder::exitTransUnit(const TSNode & cst_node) {
    IrTransUnit* node = new IrTransUnit(cst_node);
    uint32_t child_count = ts_node_named_child_count(cst_node);

    for (uint32_t i = 0; i < child_count; i++) {
        Ir* child = this->ast_stack.top();
        this->ast_stack.pop();

        IrDecl* child_d = dynamic_cast<IrDecl*>(child);
        if (child_d) {
            node->addToDeclarationList(child_d);
            continue;
        }

        IrFunctionDef* child_f = dynamic_cast<IrFunctionDef*>(child);
        if (child_f) {
            node->addToFunctionList(child_f);
            continue;
        }

        IrStatement* child_s = dynamic_cast<IrStatement*>(child);
        if (child_s) {
            node->addToStatementList(child_s);
            continue;
        }

        std::cerr << "Error: Invalid child in translation unit" << std::endl;
        return;
    }
    this->ast_stack.push(node);
}

// Function to create an AST node from a CST node
void ASTBuilder::exit_cst_node(const TSNode & cst_node) {
    const char* type = ts_node_type(cst_node);
    TSSymbol symbol_type = ts_node_symbol(cst_node);
    
    if (program["--verbose"] == true){
        if (ts_node_is_named(cst_node))
        std::cout << "Exiting CST node: "<< "Named, " << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
        else
        std::cout << "Exiting CST node: " << "Not Named, " << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:"<< std::to_string(symbol_type) << std::endl;
    }
    
    switch (symbol_type)
    {
        case 1: // Identifier
            exitIdentifier(cst_node);
            break;
        case 30: // primitive_type
            exitPrimitiveType(cst_node);
            break;
        case 218: // array_type
            exitArrayType(cst_node);
            break;
        case 211: // parameter_declaration
            exitParameter(cst_node);
            break;
        case 201: // let_declaration
            exitDeclaration(cst_node);
            break;
        case 208: // parameters
            exitParamList(cst_node);
            break;
        case 230: // function_declarator
            exitFunctionDeclarator(cst_node);
            break;
        case 247: // binary expression
            exitBinaryExpr(cst_node);
            break;
        case 125: // integer_literal
            exitLiteralNumber(cst_node);
            break;
        case 275: // return_statement
            exitReturnStatement(cst_node);
            break;
        case 290: // block
            exitCompoundStatement(cst_node);
            break;
        case 186: // function_item
            exitFunctionDefinition(cst_node);
            break;
        case 254: // arguments
            exitArgList(cst_node);
            break;
        case 253: // call_expr
            exitCallExpr(cst_node);
            break;
        case 248: // assign_expr
            exitAssignExpr(cst_node);
            break;
        case 158: //expression_statement
            exitExprStmt(cst_node);
            break;
        case 155: // source_file
            exitTransUnit(cst_node);
            break;
        case 120: // mutable_specifier
            exitMutableSpec(cst_node);
            break;
        case 264: // if_expr
            exitIfExpr(cst_node);
            break;
        case 268: // else_clause
            exitElseClause(cst_node);
            break;
        case 244: // unary_expr
            exitUnaryExpr(cst_node);
            break;
        case 276: // for_expr
            exitForExpr(cst_node);
            break;
        case 243: // range_expr
            exitRangeExpr(cst_node);
            break;
        default:
            std::cerr << "Error: Unknown CST node type: " << std::to_string(symbol_type) << std::endl;
            break;
    }

    return;
}

void ASTBuilder::enter_cst_node(const TSNode & cst_node){
    // TSSymbol symbol_type = ts_node_symbol(cst_node);
    // if (symbol_type == 290){
    //     std::cout << "ENTERING CST node: " << ts_language_symbol_name(this->language, symbol_type) <<", " << ts_node_grammar_type(cst_node) << std::endl;
    // }
    
}

void ASTBuilder::traverse_tree(const TSNode & node) {

    enter_cst_node(node);

    uint32_t named_child_count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < named_child_count; i++) {
        TSNode child = ts_node_named_child(node, i);
        traverse_tree(child);
    }

    // uint32_t child_count = ts_node_child_count(node);
    // for (uint32_t i = 0; i < child_count; i++) {
    //     TSNode child = ts_node_child(node, i);
    //     traverse_tree(child);
    // }


    exit_cst_node(node);

}