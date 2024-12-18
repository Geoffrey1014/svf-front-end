#include <memory>
#include <iostream>
#include <map>
#include <cstring>
#include "ASTBuilder.h"
#include "main.h"

void ASTBuilder::exitPrimitiveType(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node =nullptr;
    if (*node_text == "i8" || *node_text == "i16" || *node_text == "i32" || *node_text == "i64" || *node_text == "i128") {
        node = new IrTypeInt(cst_node); // Assuming IrTypeInt can be reused for Rust's int types
        this->ast_stack.push(node);
    } 
    else if (*node_text == "u8" || *node_text == "u16" || *node_text == "u32" || *node_text == "u64" || *node_text == "u128") {
        node = new IrTypeUInt(cst_node); // Assuming IrTypeUInt for unsigned int types
        this->ast_stack.push(node);
    }
    else if (*node_text == "f32" || *node_text == "f64") {
        node = new IrTypeFloat(cst_node); // Assuming IrTypeFloat for floating-point types
        this->ast_stack.push(node);
    }
    else if (*node_text == "bool") {
        node = new IrTypeBool(cst_node); // Assuming IrTypeBool for boolean types
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

void ASTBuilder::exitIdentifier(const TSNode & cst_node) {
    std::string* node_text = getNodeText(cst_node);
    Ir* node = new IrIdent(node_text, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParameter(const TSNode & cst_node) {
    Ir* node =nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for parameter declaration" << std::endl;
    }

    IrIdent* paramName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* paramType = dynamic_cast<IrType*>( this->ast_stack.top());
    this->ast_stack.pop();


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

    // Check for mutable binding
    bool is_mutable = false;
    TSNode mut_node = ts_node_child_by_field_name(cst_node, "mut", 3);
    if (!ts_node_is_null(mut_node)) {
        is_mutable = true;
    }

    // Get the name/pattern
    IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    if (declName) {
        this->ast_stack.pop();
    } else {
        std::cerr << "Error: Invalid declaration name/pattern" << std::endl;
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

    // Create the declaration node
    node = new IrDecl(declName, declType, is_mutable, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParamList(const TSNode & cst_node){

    IrParamList* paramList = new IrParamList(cst_node);
    IrParamDecl* paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());

    while (paramDecl) {
        this->ast_stack.pop();
        paramList->addToParamsList(paramDecl);
        paramDecl = dynamic_cast<IrParamDecl*>(this->ast_stack.top());
    }

    this->ast_stack.push(paramList);
}

void ASTBuilder::exitFunctionDeclarator(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 2) {
        std::cerr << "Error: Not enough elements on the stack for function declaration" << std::endl;
    }

    IrParamList* paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrIdent* declName = dynamic_cast<IrIdent*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (declName && paramList) {
        node = new IrFunctionDecl(declName, paramList, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid function declaration type or name" << std::endl;
    }
}

void ASTBuilder::exitFunctionDefinition(const TSNode & cst_node){
    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 3) {
        std::cerr << "Error: Not enough elements on the stack for function definition" << std::endl;
    }

    IrCompoundStmt* compoundStmt = dynamic_cast<IrCompoundStmt*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrFunctionDecl* functionDecl = dynamic_cast<IrFunctionDecl*>(this->ast_stack.top());
    this->ast_stack.pop();

    IrType* returnType = dynamic_cast<IrType*>(this->ast_stack.top());
    this->ast_stack.pop();

    if (returnType && functionDecl && compoundStmt) {
        node = new IrFunctionDef(returnType, functionDecl, compoundStmt, cst_node);
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
    // std::cout << "Operation: " << *operation << std::endl;

    if (leftOperand && rightOperand) {
        node = new IrBinaryExpr(operation, leftOperand, rightOperand, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid binary expression" << std::endl;
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
    Ir* node = nullptr;

    IrCompoundStmt* compoundStmt = new IrCompoundStmt(cst_node);
    IrStatement* stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    while (stmt) {
        this->ast_stack.pop();
        compoundStmt->addStmtToFront(stmt);
        stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
    }
    this->ast_stack.push(compoundStmt);
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

// Methods to handle Rust-specific constructs

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
    if (this->ast_stack.size() < 4) {
        std::cerr << "Error: Not enough elements on the stack for for expression" << std::endl;
        return;
    }

    Ir* body = this->ast_stack.top();
    this->ast_stack.pop();

    Ir* increment = this->ast_stack.top();
    this->ast_stack.pop();

    Ir* condition = this->ast_stack.top();
    this->ast_stack.pop();

    Ir* initialization = this->ast_stack.top();
    this->ast_stack.pop();

    IrForExpr* forExpr = new IrForExpr(initialization, condition, increment, body, cst_node);
    this->ast_stack.push(forExpr);
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

void ASTBuilder::exitTransUnit(const TSNode & cst_node){
    IrTransUnit* node = new IrTransUnit(cst_node);
    uint32_t child_count = ts_node_named_child_count(cst_node);

    for (uint32_t i = 0; i < child_count; i++) {
        IrDecl* child_d = dynamic_cast<IrDecl*>(this->ast_stack.top());
        if (!child_d) {
            IrFunctionDef* child_f = dynamic_cast<IrFunctionDef*>(this->ast_stack.top());
            if (!child_f) {
                std::cerr << "Error: Invalid child in translation unit" << std::endl;
                return;
            }
            this->ast_stack.pop();
            node->addToFunctionList(child_f);
        }
        else {
            this->ast_stack.pop();
            node->addToDeclerationList(child_d);
        }
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
        case 93: // primitive_type
            exitPrimitiveType(cst_node);
            break;
        case 260: // parameter_declaration
            exitParameter(cst_node);
            break;
        case 198: // declaration
            exitDeclaration(cst_node);
            break;
        case 258: // parameter_list
            exitParamList(cst_node);
            break;
        case 230: // function_declarator
            exitFunctionDeclarator(cst_node);
            break;
        case 290: // function_definition
            exitBinaryExpr(cst_node);
            break;
        case 141: // literal_number
            exitLiteralNumber(cst_node);
            break;
        case 275: // return_statement
            exitReturnStatement(cst_node);
            break;
        case 241: // compound_statement
            exitCompoundStatement(cst_node);
            break;
        case 196: // function_definition
            exitFunctionDefinition(cst_node);
            break;
        case 309: // arg_list
            exitArgList(cst_node);
            break;
        case 299: // call_expr
            exitCallExpr(cst_node);
            break;
        case 287: // assign_expr
            exitAssignExpr(cst_node);
            break;
        case 266: //expression_statement
            exitExprStmt(cst_node);
            break;
        case 161:
            exitTransUnit(cst_node);
            break;
        default:
            std::cerr << "Error: Unknown CST node type" << std::endl;
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