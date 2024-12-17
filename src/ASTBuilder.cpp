#include <memory>
#include <iostream>
#include <map>
#include <cstring>
#include "ASTBuilder.h"
#include "main.h"

#include "ASTBuilder.tpp"

std::string ASTBuilder::getNodeText(const TSNode &node) {
    unsigned start = ts_node_start_byte(node);
    unsigned end = ts_node_end_byte(node);
    return source_code->substr(start, end - start); // Return by value
}

void ASTBuilder::exitPrimitiveType(const TSNode & cst_node) {
    std::string node_text = getNodeText(cst_node);
    Ir* node =nullptr;

    if (node_text == "int") {
        node = new IrTypeInt(cst_node);
        this->ast_stack.push(node);
    } 
    else if (node_text == "void")
    {
        node = new IrTypeVoid(cst_node);
        this->ast_stack.push(node);
    }
    else if (node_text == "bool")
    {
        node = new IrTypeBool(cst_node);
        this->ast_stack.push(node);
    }
    else {
        std::cerr << "Error: Unknown primitive type" << std::endl;
    }
}

void ASTBuilder::exitIdentifier(const TSNode & cst_node) {
    std::string node_text = getNodeText(cst_node);
    Ir* node = new IrIdent(node_text, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitParameter(const TSNode & cst_node) {
    // Check if the stack has at least one element (type is mandatory)
    if (this->ast_stack.empty()) {
        std::cerr << "Error: Not enough elements on the stack for parameter declaration" << std::endl;
        return;
    }

    IrIdent* paramName = nullptr;
    // Check if the top of the stack is a parameter name (optional)
    if (dynamic_cast<IrIdent*>(this->ast_stack.top())) {
        paramName = dynamic_cast<IrIdent*>(this->ast_stack.top());
        this->ast_stack.pop();
    }

    // Check if the stack has a type (mandatory)
    if (this->ast_stack.empty()) {
        std::cerr << "Error: Missing type specifier for parameter declaration" << std::endl;
        return;
    }

    IrType* paramType = dynamic_cast<IrType*>(this->ast_stack.top());
    if (!paramType) {
        std::cerr << "Error: Invalid type specifier for parameter declaration" << std::endl;
        delete paramName;
        return;
    }
    this->ast_stack.pop();

    // Create a new parameter declaration node
    Ir* node = new IrParamDecl(paramType, paramName, cst_node);
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

// function_declarator
void ASTBuilder::exitFunctionDeclarator(const TSNode &cst_node) {
    try {
        IrParamList* paramList = nullptr;

        // Pop the parameter list (optional)
        if (!this->ast_stack.empty() && dynamic_cast<IrParamList*>(this->ast_stack.top())) {
            paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
            this->ast_stack.pop();
        } else {
            paramList = new IrParamList(cst_node); // Empty param list
        }

        // Safely pop the function name (identifier)
        IrDeclarator* declarator = nullptr;

        if (!this->ast_stack.empty()) {
            if (IrIdent* ident = dynamic_cast<IrIdent*>(this->ast_stack.top())) {
                // IrIdent is a valid IrDeclarator
                this->ast_stack.pop();
                declarator = ident;
            } else {
                throw std::runtime_error("Error: Expected function name as IrIdent but found " 
                                         + std::string(typeid(*this->ast_stack.top()).name()));
            }
        } else {
            throw std::runtime_error("Error: AST stack is empty while processing function_declarator.");
        }

        // Construct the IrFunctionDecl node
        IrFunctionDecl* funcDecl = new IrFunctionDecl(declarator, paramList, cst_node);
        this->ast_stack.push(funcDecl);

        std::cout << "Pushed IrFunctionDecl onto stack: " << funcDecl->prettyPrint("") << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error in function declarator: " << e.what() << std::endl;
    }
}


void ASTBuilder::exitFunctionDefinition(const TSNode &cst_node) {
    try {
        // mandatory 3 elements
        IrCompoundStmt* compoundStmt = this->popFromStack<IrCompoundStmt>(cst_node);
        IrFunctionDecl* functionDecl = this->popFromStack<IrFunctionDecl>(cst_node);
        IrType* returnType = this->popFromStack<IrType>(cst_node);

        IrFunctionDef* funcDef = new IrFunctionDef(returnType, functionDecl, compoundStmt, cst_node);
        this->ast_stack.push(funcDef);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
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
    std::string operation = getNodeText(second_child); 

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

void ASTBuilder::exitCompoundStatement(const TSNode &cst_node) {
    IrCompoundStmt* compoundStmt = new IrCompoundStmt(cst_node);

    // Iterate through the stack to collect statements and declarations
    while (!this->ast_stack.empty()) {
        // Try to dynamically cast the top of the stack to IrStatement
        IrStatement* stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());
        if (stmt) {
            this->ast_stack.pop();
            compoundStmt->addStmtToFront(stmt); // Add statements or declarations to the compound statement
        } else {
            break; // Stop when no more IrStatement objects are found
        }
    }

    // Push the compound statement onto the stack
    this->ast_stack.push(compoundStmt);
    std::cout << "Pushed IrCompoundStmt onto stack.\n";
}

void ASTBuilder::exitLiteralNumber(const TSNode & cst_node){
    std::string node_text = getNodeText(cst_node);
    IrLiteralNumber* node = new IrLiteralNumber(std::stoi(node_text), cst_node);
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

void ASTBuilder::exitTransUnit(const TSNode &cst_node) {
    IrTransUnit* transUnitNode = new IrTransUnit(cst_node);
    uint32_t child_count = ts_node_named_child_count(cst_node);

    // // Debugging output to verify the stack size
    // std::cout << "AST stack size before processing: " << this->ast_stack.size() << std::endl;

    // Process only as many items as are available on the stack
    uint32_t items_to_process = std::min(child_count, static_cast<uint32_t>(this->ast_stack.size()));

    for (uint32_t i = 0; i < items_to_process; i++) {
        if (this->ast_stack.empty()) {
            std::cout << "Error: AST stack is empty at child index" << std::endl;
        }

        Ir* topLevelItem = this->ast_stack.top();

        if (auto* decl = dynamic_cast<IrDecl*>(topLevelItem)) {
            this->ast_stack.pop();
            transUnitNode->addToDeclarationList(decl);
        } else if (auto* func = dynamic_cast<IrFunctionDef*>(topLevelItem)) {
            this->ast_stack.pop();
            transUnitNode->addToFunctionList(func);
        } else if (auto* preprocInclude = dynamic_cast<IrPreprocInclude*>(topLevelItem)) {
            this->ast_stack.pop();
            transUnitNode->addToPreprocIncludeList(preprocInclude);
        } else {
            std::cerr << "Error: Unrecognized top-level item type." << std::endl;
        }
    }

    // Push the transUnitNode onto the stack
    this->ast_stack.push(transUnitNode);
}

void ASTBuilder::exitStringContent(const TSNode &cst_node) {
    std::string content = getNodeText(cst_node);
    if (content.empty()) {
        std::cerr << "Error: Unable to retrieve string content" << std::endl;
        return;
    }
    Ir* node = new IrLiteralStringContent(content, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitLiteralString(const TSNode & cst_node){
    if (this->ast_stack.empty()) {
        std::cerr << "Error: Not enough elements on the stack for string literal" << std::endl;
        return;
    }    

    Ir* node = nullptr;
    // Use stack to get the type and name
    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for string literal" << std::endl;
    }

    IrLiteralStringContent* content = dynamic_cast<IrLiteralStringContent*>(this->ast_stack.top());

    if (content) {
        this->ast_stack.pop();
        node = new IrLiteralString(content, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid string literal" << std::endl;
    }
}

void ASTBuilder::exitPreprocInclude(const TSNode &cst_node) {
    Ir* node = nullptr;

    if (this->ast_stack.size() < 1) {
        std::cerr << "Error: Not enough elements on the stack for preprocessor include" << std::endl;
    }

    IrLiteralString* path = dynamic_cast<IrLiteralString*>(this->ast_stack.top());

    if (path) {
        this->ast_stack.pop();
        node = new IrPreprocInclude(path, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Invalid path for preprocessor include" << std::endl;
    }
}

void ASTBuilder::exitStorageClassSpecifier(const TSNode & cst_node) {
    std::string specifierText = getNodeText(cst_node);

    if (!specifierText.empty()) {
        Ir* node = new IrStorageClassSpecifier(specifierText, cst_node);
        this->ast_stack.push(node);
    } else {
        std::cerr << "Error: Unable to retrieve storage class specifier text" << std::endl;
    }
}

void ASTBuilder::exitArrayDeclarator(const TSNode &cst_node) {
    try {
        auto* sizeExpr = this->popFromStack<IrExpr>(cst_node);
        auto* baseDeclarator = this->popFromStack<IrDeclarator>(cst_node);
        Ir* arrayDeclaratorNode = new IrArrayDeclarator(baseDeclarator, sizeExpr, cst_node);
        this->ast_stack.push(arrayDeclaratorNode);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void ASTBuilder::exitSubscriptExpression(const TSNode &cst_node) {
    IrExpr* indexExpr = nullptr;
    IrExpr* baseExpr = nullptr;

    // Pop the index expression (mandatory)
    if (!this->ast_stack.empty() && dynamic_cast<IrExpr*>(this->ast_stack.top())) {
        indexExpr = dynamic_cast<IrExpr*>(this->ast_stack.top());
        this->ast_stack.pop();
    } else {
        std::cerr << "Error: Missing or invalid index expression in subscript_expression" << std::endl;
        return;
    }

    // Pop the base expression (mandatory)
    if (!this->ast_stack.empty() && dynamic_cast<IrExpr*>(this->ast_stack.top())) {
        baseExpr = dynamic_cast<IrExpr*>(this->ast_stack.top());
        this->ast_stack.pop();
    } else {
        std::cerr << "Error: Missing or invalid base expression in subscript_expression" << std::endl;
        delete indexExpr;
        return;
    }

    // Create a new IrSubscriptExpr node
    Ir* node = new IrSubscriptExpr(baseExpr, indexExpr, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitDeclaration(const TSNode &cst_node) {
    try {
        // Optional: Pop storage class specifier
        IrStorageClassSpecifier* specifier = nullptr;

        // Check for IrInitDeclarator
        if (!this->ast_stack.empty() && dynamic_cast<IrInitDeclarator*>(this->ast_stack.top())) {
            IrInitDeclarator* initDecl = dynamic_cast<IrInitDeclarator*>(this->ast_stack.top());
            this->ast_stack.pop();

            IrType* type = this->popFromStack<IrType>(cst_node);

            if (!this->ast_stack.empty()) {
                specifier = dynamic_cast<IrStorageClassSpecifier*>(this->ast_stack.top());
                if (specifier) {
                    this->ast_stack.pop();
                } else {
                    specifier = nullptr;
                }
            }

            IrDecl* decl = new IrDecl(type, specifier, initDecl, cst_node);
            this->ast_stack.push(decl);
            return;
        }

        // Otherwise, handle simple declarator
        IrDeclarator* declarator = this->popFromStack<IrDeclarator>(cst_node);

        if (!this->ast_stack.empty() && dynamic_cast<IrStorageClassSpecifier*>(this->ast_stack.top())) {
            specifier = dynamic_cast<IrStorageClassSpecifier*>(this->ast_stack.top());
            this->ast_stack.pop();
        }

        IrType* type = this->popFromStack<IrType>(cst_node);

        // After popping the type, check if there's a storage class specifier on top
        if (!this->ast_stack.empty()) {
            specifier = dynamic_cast<IrStorageClassSpecifier*>(this->ast_stack.top());
            if (specifier) {
                this->ast_stack.pop();
            } else {
                specifier = nullptr;
            }
        }

        IrDecl* decl = new IrDecl(type, specifier, declarator, cst_node);
        this->ast_stack.push(decl);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error in declaration: " << e.what() << std::endl;
    }
}


void ASTBuilder::exitInitDeclarator(const TSNode &cst_node) {
    try {
        // Pop the initializer (expression, e.g., 2)
        IrExpr* initializer = this->popFromStack<IrExpr>(cst_node);

        // Pop the declarator (identifier, e.g., b)
        IrDeclarator* declarator = this->popFromStack<IrDeclarator>(cst_node);

        // Combine into an IrInitDeclarator node
        IrInitDeclarator* initDecl = new IrInitDeclarator(declarator, initializer, cst_node);

        // Push the IrInitDeclarator node onto the stack
        this->ast_stack.push(initDecl);
        std::cout << "Pushed IrInitDeclarator onto stack.\n";

    } catch (const std::runtime_error& e) {
        std::cerr << "Error in init_declarator: " << e.what() << std::endl;
    }
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
        case 258: // parameter_list
            exitParamList(cst_node);
            break; 
        case 230: // function_declarator
            exitFunctionDeclarator(cst_node);
            break;
        case 290: // binary_expression
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
        case 160: // comment
            break;
        case 153: // string_content
            exitStringContent(cst_node);
            break;
        case 320: // string_literal
            exitLiteralString(cst_node);
            break;
        case 164: // preproc_include
            exitPreprocInclude(cst_node);            
            break;
        case 242: // storage_class_specifier
            exitStorageClassSpecifier(cst_node);
            break;
        case 236: // array_declarator
            exitArrayDeclarator(cst_node);
            break;
        case 298: // subscript_expression
            exitSubscriptExpression(cst_node);
            break;
        case 198: // declaration
            exitDeclaration(cst_node);
            break;
        case 240: // init_declarator already handled in declaration
            exitInitDeclarator(cst_node);
            break;
        case 161: // translation_unit
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


        // std::cout << "Stack state before exiting:\n";
        // std::stack<Ir*> tempStack = this->ast_stack; // Copy the stack for debugging

        // while (!tempStack.empty()) {
        //     Ir* node = tempStack.top();
        //     tempStack.pop();

        //     if (node) {
        //         std::cout << "- " << typeid(*node).name() << "\n";
        //     } else {
        //         std::cout << "- null\n";
        //     }
        // }
        // std::cout << "End of stack.\n";