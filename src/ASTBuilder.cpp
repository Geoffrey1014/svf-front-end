#include <memory>
#include <iostream>
#include <map>
#include <cstring>
#include "ASTBuilder.h"
#include "main.h"

std::string ASTBuilder::getNodeText(const TSNode &node) {
    unsigned start = ts_node_start_byte(node);
    unsigned end = ts_node_end_byte(node);
    return source_code->substr(start, end - start); // Return by value
}

void ASTBuilder::debugStackState() const {
    std::cout << "Stack state:\n";
    std::stack<Ir*> tempStack = this->ast_stack;

    while (!tempStack.empty()) {
        Ir* node = tempStack.top();
        tempStack.pop();

        if (node) {
            std::cout << "- " << typeid(*node).name() <<
            " -value " << node->toString() << "\n";
        }
    }
    std::cout << "End of stack.\n";
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
    else if (node_text == "char")
    {
        node = new IrTypeChar(cst_node);
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

// parameter_declaration
void ASTBuilder::exitParameter(const TSNode &cst_node) {
    try {
        IrDeclDeclarator* declarator = nullptr;

        // optional(field('declarator', choice($._declarator,$._abstract_declarator))
        if (!this->ast_stack.empty() && dynamic_cast<IrDeclDeclarator*>(this->ast_stack.top())) {
            declarator = popFromStack<IrDeclDeclarator>(cst_node);
        }

        // mandatory type specifier
        IrType* paramType = popFromStack<IrType>(cst_node);

        Ir* node = new IrParamDecl(paramType, declarator, cst_node);
        this->ast_stack.push(node);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitParameter: " << e.what() << std::endl;
    }
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

        // Pop the parameter list
        if (!this->ast_stack.empty() && dynamic_cast<IrParamList*>(this->ast_stack.top())) {
            paramList = dynamic_cast<IrParamList*>(this->ast_stack.top());
            this->ast_stack.pop();
        } else {
            paramList = new IrParamList(cst_node); // Empty param list
        }

        // Safely pop the function name (identifier)
        IrDeclDeclarator* declarator = nullptr;

        if (!this->ast_stack.empty()) {
            Ir* node = this->ast_stack.top();
            if (IrIdent* ident = dynamic_cast<IrIdent*>(node)) {
                // IrIdent is a valid IrDeclarator
                this->ast_stack.pop();
                declarator = ident;
            } else {
                throw std::runtime_error("Error: Expected function name as IrIdent but found "
                                         + std::string(typeid(*node).name()));
            }
        } else {
            throw std::runtime_error("Error: AST stack is empty while processing function_declarator.");
        }

        // Construct the IrFunctionDecl node
        IrFunctionDecl* funcDecl = new IrFunctionDecl(declarator, paramList, cst_node);
        std::string name = funcDecl->getName();
        // if (name.empty()) {
        //     std::cout << "Empty function name in function declarator" << std::endl;
        // } else {
        //     std::cout << "Declarator name is: " << name << std::endl;
        // }
        this->ast_stack.push(funcDecl);
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

    uint32_t stmt_count = ts_node_named_child_count(cst_node);

    for (uint32_t i = 0; i < stmt_count; i++) {
        if (this->ast_stack.empty()) {
            std::cerr << "Error: AST stack empty while building compound statement" << std::endl;
            break;
        }

        // Pop statement from the stack and add to the compound statement
        IrStatement* stmt = dynamic_cast<IrStatement*>(this->ast_stack.top());

        if (stmt) {
            this->ast_stack.pop();
            compoundStmt->addStmtToFront(stmt);  // Preserve order
        } else {
            break;
        }
    }
    this->ast_stack.push(compoundStmt);
}

void ASTBuilder::exitLiteralNumber(const TSNode & cst_node){
    std::string node_text = getNodeText(cst_node);
    IrLiteralNumber* node = new IrLiteralNumber(std::stoi(node_text), cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitLiteralChar(const TSNode & cst_node){
    std::string node_text = getNodeText(cst_node);
    IrLiteralChar* node = new IrLiteralChar(node_text[0], cst_node);
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

void ASTBuilder::exitAssignExpr(const TSNode &cst_node) {
    try {
        TSNode operator_node = ts_node_child(cst_node, 1);  // Operator is the second child
        std::string opText = getNodeText(operator_node);  // Retrieve operator ('=', '+=', etc.)

        IrExpr* rhs = this->popFromStack<IrExpr>(cst_node);
        IrExpr* lhs = this->popFromStack<IrExpr>(cst_node);

        IrAssignExpr* assignExpr = new IrAssignExpr(lhs, rhs, opText, cst_node);
        this->ast_stack.push(assignExpr);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitAssignExpr: " << e.what() << std::endl;
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

    while (!this->ast_stack.empty()) {
        Ir* node = this->ast_stack.top();
        this->ast_stack.pop();

        // If the node is IrMultiDecl, "release" its sub-declarations
        if (auto* multiDecl = dynamic_cast<IrMultiDecl*>(node)) {

            // This returns a non-const std::deque<IrDecl*>
            auto allDecls = multiDecl->releaseDeclarations();

            while (!allDecls.empty()) {
                IrDecl* singleDecl = allDecls.front();
                allDecls.pop_front();

                if (auto decl = dynamic_cast<IrDecl*>(singleDecl)) {
                    transUnitNode->addTopLevelNodeFront(decl);
                } else {
                    std::cerr << "Warning: Unrecognized node inside IrMultiDecl.\n";
                    delete singleDecl;
                }
            }
            delete multiDecl;
        }
        else if (dynamic_cast<IrDecl*>(node) ||
                 dynamic_cast<IrFunctionDef*>(node) ||
                 dynamic_cast<IrPreprocInclude*>(node) ||
                 dynamic_cast<IrTypeDef*>(node) ||
                 dynamic_cast<IrPreprocDef*>(node) ||
                 dynamic_cast<IrExprStmt*>(node))
        {
            transUnitNode->addTopLevelNodeFront(node);
        } else {
            std::cerr << "Warning: Skipping unrecognized node." << std::endl;
            delete node;
        }
    }
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

        if (arraylevel <= 0) {
            throw std::runtime_error("Error: Invalid array level");
        }
        arraylevel -= 1;
        if (arraylevel == 0) {
            deque<IrLiteral*> dims;
            while (dynamic_cast<IrLiteral*>(this->ast_stack.top()) ) {
                IrLiteral* literal = this->popFromStack<IrLiteral>(cst_node);
                dims.push_front(literal);
            }
            IrIdent* id = this->popFromStack<IrIdent>(cst_node);
            IrType* baseType = this->popFromStack<IrType>(cst_node);
            IrTypeArray* arrayType = new IrTypeArray(baseType, dims, cst_node);
            ast_stack.push(arrayType);
            ast_stack.push(id);
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void ASTBuilder::exitPointerDeclarator(const TSNode &cst_node) {
    try {
        IrDeclDeclarator* baseDeclarator = popFromStack<IrDeclDeclarator>(cst_node);

        IrType* baseType = dynamic_cast<IrType*>(ast_stack.top());
        ast_stack.pop();

        IrPointerType* pointerType = new IrPointerType(baseType, cst_node);
        ast_stack.push(pointerType);
        ast_stack.push(baseDeclarator);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitPointerDeclarator: " << e.what() << std::endl;
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

    if (!this->ast_stack.empty() && dynamic_cast<IrExpr*>(this->ast_stack.top())) {
        baseExpr = dynamic_cast<IrExpr*>(this->ast_stack.top());
        this->ast_stack.pop();
    } else {
        std::cerr << "Error: Missing or invalid base expression in subscript_expression" << std::endl;
        delete indexExpr;
        return;
    }

    IrSubscriptExpr* node = new IrSubscriptExpr(baseExpr, indexExpr, cst_node);
    node->setLevel(arraylevel);
    this->ast_stack.push(node);
}

void ASTBuilder::exitDeclaration(const TSNode &cst_node) {
    try {
        std::deque<IrInitDeclarator*> initDecls;
        std::deque<IrDeclDeclarator*> simpleDecls;
        while (!this->ast_stack.empty()) {
            IrInitDeclarator* initDecl = dynamic_cast<IrInitDeclarator*>(this->ast_stack.top());
            IrDeclDeclarator* simpleDecl = dynamic_cast<IrDeclDeclarator*>(this->ast_stack.top());

            if (initDecl) {
                this->ast_stack.pop();
                initDecls.push_back(initDecl);
            }
            else if (simpleDecl) {
                this->ast_stack.pop();
                simpleDecls.push_back(simpleDecl);
            }
            else {
                // Neither initDecl nor simpleDecl => stop
                break;
            }
        }

        // 2) Pop the base type (e.g. int)
        IrType* originalType = this->popFromStack<IrType>(cst_node);

        // 3) Optionally pop storage class specifier (e.g. static)
        IrStorageClassSpecifier* specifier = nullptr;
        if (!this->ast_stack.empty()) {
            specifier = dynamic_cast<IrStorageClassSpecifier*>(this->ast_stack.top());
            if (specifier) {
                this->ast_stack.pop();
            }
        }

        // 4) Create a container node for multiple single-variable declarations
        IrMultiDecl* multiDecl = new IrMultiDecl(cst_node);

        // If you want to skip the "bool flag" and always clone, you can.
        // For demonstration, we show the bool-flag approach:
        bool usedOriginalPointer = false;

        // 5a) Handle initDeclarators (e.g. int a=1, b=2;)
        if (!initDecls.empty()) {
            for (auto* initDecl : initDecls) {
                // Decide whether to reuse the original pointer or clone
                IrType* typeForThis = nullptr;
                if (!usedOriginalPointer) {
                    typeForThis = originalType;
                    usedOriginalPointer = true;
                } else {
                    // For subsequent variables, clone the type so each IrDecl has its own pointer
                    typeForThis = originalType->clone();
                }
                // Build a single-variable IrDecl (initialized)
                IrDecl* decl = new IrDecl(typeForThis, specifier, initDecl, cst_node);
                // Add it to our container
                multiDecl->addDeclaration(decl);
            }
        }
        // 5b) Handle simpleDeclarators (e.g. int a, b;)
        else if (!simpleDecls.empty()) {
            for (auto* simpleDecl : simpleDecls) {
                IrType* typeForThis = nullptr;
                if (!usedOriginalPointer) {
                    typeForThis = originalType;
                    usedOriginalPointer = true;
                } else {
                    typeForThis = originalType->clone();
                }

                // Build a single-variable IrDecl (uninitialized)
                IrDecl* decl = new IrDecl(typeForThis, specifier, simpleDecl, cst_node);
                multiDecl->addDeclaration(decl);
            }
        }

        this->ast_stack.push(multiDecl);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error in declaration: " << e.what() << std::endl;
    }
}


void ASTBuilder::exitInitDeclarator(const TSNode &cst_node) {
    try {

        IrExpr* initializer = this->popFromStack<IrExpr>(cst_node);
        IrDeclDeclarator* declarator = this->popFromStack<IrDeclDeclarator>(cst_node);

        IrInitDeclarator* initDecl = new IrInitDeclarator(declarator, initializer, cst_node);
        this->ast_stack.push(initDecl);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error in init_declarator: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitAbstractPointerDeclarator(const TSNode &cst_node) {
    try {
        IrDeclDeclarator* baseDeclarator = nullptr;

        // Check for an optional base declarator on the stack
        if (!this->ast_stack.empty()) {
            baseDeclarator = dynamic_cast<IrDeclDeclarator*>(this->ast_stack.top());
            if (baseDeclarator) {
                this->ast_stack.pop();
            }
        }

        IrAbstractPointerDeclarator* pointerDeclarator = new IrAbstractPointerDeclarator(baseDeclarator, cst_node);
        this->ast_stack.push(pointerDeclarator);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitAbstractPointerDeclarator: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitFieldDeclaration(const TSNode &cst_node) {
    try {
        // Handle optional bitfield clause for future if we need it
        // field declarator
        IrDeclDeclarator* declarator = nullptr;
        if (!this->ast_stack.empty() && dynamic_cast<IrDeclDeclarator*>(this->ast_stack.top())) {
            declarator = popFromStack<IrDeclDeclarator>(cst_node);
        }
        // type specifier
        IrType* type = popFromStack<IrType>(cst_node);

        IrFieldDecl* fieldDecl = new IrFieldDecl(type, declarator, cst_node);
        this->ast_stack.push(fieldDecl);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitFieldDeclaration: " << e.what() << std::endl;
    }
}
void ASTBuilder::exitFieldDeclarationList(const TSNode &cst_node) {
    try {
        // always create an IrFieldDeclList, even if it's empty
        IrFieldDeclList* fieldDeclList = new IrFieldDeclList(cst_node);

        while (!this->ast_stack.empty()) {
            IrFieldDecl* fieldDecl = dynamic_cast<IrFieldDecl*>(this->ast_stack.top());
            if (!fieldDecl) break;
            this->ast_stack.pop();
            fieldDeclList->addField(fieldDecl);
        }

        this->ast_stack.push(fieldDeclList);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitFieldDeclarationList: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitStructSpecifier(const TSNode& cst_node) {
    try {
        IrFieldDeclList* fieldDeclList = nullptr;
        if (!this->ast_stack.empty() && dynamic_cast<IrFieldDeclList*>(this->ast_stack.top())) {
            fieldDeclList = popFromStack<IrFieldDeclList>(cst_node);
        } else {
            fieldDeclList = new IrFieldDeclList(cst_node); // Create an empty field declaration list
        }

        IrIdent* name = nullptr;
        if (!this->ast_stack.empty() && dynamic_cast<IrIdent*>(this->ast_stack.top())) {
            name = popFromStack<IrIdent>(cst_node);
        }

        IrTypeStruct* structType = new IrTypeStruct(name, fieldDeclList, cst_node);
        this->ast_stack.push(structType);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitStructSpecifier: " << e.what() << std::endl;
    }
}

void ASTBuilder:: exitTypeDefinition(const TSNode &cst_node) {
    try {
        IrTypeIdent* alias = popFromStack<IrTypeIdent>(cst_node);
        // IrIdent* alias = popFromStack<IrIdent>(cst_node);
        IrType* type = popFromStack<IrType>(cst_node);

        // alias->markAsTypeAlias();
        IrTypeDef* typeDef = new IrTypeDef(type, alias, cst_node);
        this->ast_stack.push(typeDef);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitTypeDefinition: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitTypeIdentifier(const TSNode &cst_node) {
    try {
        std::string node_text = getNodeText(cst_node);
        IrTypeIdent* typeIdenifier = new IrTypeIdent(node_text, cst_node);
        this->ast_stack.push(typeIdenifier);
    } catch (const std::exception &e) {
        std::cerr << "Error in exitTypeIdentifier: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitFieldExpression(const TSNode & cst_node) {

    IrIdent* fieldName = this->popFromStack<IrIdent>(cst_node);

    TSNode operator_node = ts_node_child(cst_node, 1); // second child is the op node
    std::string opText = getNodeText(operator_node);
    bool isArrow = (opText == "->");

    IrExpr* baseExpr = this->popFromStack<IrExpr>(cst_node);

    // Create the field expression node
    IrFieldExpr* fieldExpr = new IrFieldExpr(baseExpr, fieldName, isArrow, cst_node);
    this->ast_stack.push(fieldExpr);
}

void ASTBuilder::exitPointerExpression(const TSNode &cst_node) {

    IrExpr* argument = this->popFromStack<IrExpr>(cst_node);

    TSNode operator_node = ts_node_child(cst_node, 0);
    std::string opText = getNodeText(operator_node);

    bool isAddressOf = (opText == "&");
    bool isDereference = (opText == "*");

    IrPointerExpr* pointerExpr = new IrPointerExpr(argument, isAddressOf, isDereference, cst_node);
    this->ast_stack.push(pointerExpr);
}

void ASTBuilder::exitPreprocArg(const TSNode &cst_node) {
    std::string node_text = getNodeText(cst_node);
    Ir* node = new IrPreprocArg(node_text, cst_node);
    this->ast_stack.push(node);
}

void ASTBuilder::exitPreprocDef(const TSNode &cst_node) {
    try {
        IrPreprocArg* value = nullptr;

        // 1. Pop preproc_arg (value) if it exists
        if (!this->ast_stack.empty()) {
            if (IrPreprocArg* arg = dynamic_cast<IrPreprocArg*>(this->ast_stack.top())) {
                this->ast_stack.pop();
                value = arg;  // Store the argument directly
            }
        }
        IrIdent* identifier = this->popFromStack<IrIdent>(cst_node);
        IrPreprocDef* node = new IrPreprocDef(identifier, cst_node, value);

        this->ast_stack.push(node);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitPreprocDef: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitParenthesizedExpr(const TSNode &cst_node) {
    try {
        IrExpr* expr = this->popFromStack<IrExpr>(cst_node);
        IrParenthesizedExpr* parenExpr = new IrParenthesizedExpr(expr, cst_node);
        this->ast_stack.push(parenExpr);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitParenthesizedExpr: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitUnaryExpr(const TSNode &cst_node) {
    try {
        IrExpr* expr = this->popFromStack<IrExpr>(cst_node);
        TSNode operatorNode = ts_node_child(cst_node, 0);
        std::string op = getNodeText(operatorNode);

        IrUnaryExpr* unaryExpr = new IrUnaryExpr(op, expr, cst_node);
        this->ast_stack.push(unaryExpr);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitUnaryExpr: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitIfStatement(const TSNode &cst_node) {
    try {
        IrElseClause* alternative = nullptr;
        if (ts_node_child_count(cst_node) == 4) {
            alternative = this->popFromStack<IrElseClause>(cst_node);
        }

        IrStatement* consequence = this->popFromStack<IrStatement>(cst_node);
        IrParenthesizedExpr* condition = this->popFromStack<IrParenthesizedExpr>(cst_node);

        IrIfStmt* ifStmt = new IrIfStmt(condition, consequence, alternative, cst_node);
        this->ast_stack.push(ifStmt);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitIfStatement: " << e.what() << std::endl;
    }
}

// Process the else clause before the if statement
void ASTBuilder::exitElseClause(const TSNode &cst_node) {
    try {
        IrStatement* alt = this->popFromStack<IrStatement>(cst_node);
        IrElseClause* elseNode = new IrElseClause(alt, cst_node);
        this->ast_stack.push(elseNode);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitElseClause: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitForStatement(const TSNode &cst_node) {
    try {
        IrStatement* body = nullptr;
        IrExpr* update = nullptr;
        IrExpr* condition = nullptr;

        body = this->popFromStack<IrStatement>(cst_node);

        // Check if 'update' exists by inspecting CST children directly
        TSNode updateNode = ts_node_child_by_field_name(cst_node, "update", 6);
        if (!ts_node_is_null(updateNode)) {
            update = dynamic_cast<IrExpr*>(this->ast_stack.top());
            if (update) this->ast_stack.pop();
        }

        // Check if 'condition' exists
        TSNode conditionNode = ts_node_child_by_field_name(cst_node, "condition", 9);
        if (!ts_node_is_null(conditionNode)) {
            condition = dynamic_cast<IrExpr*>(this->ast_stack.top());
            if (condition) this->ast_stack.pop();
        }

        IrAssignExpr* initializer = popFromStack<IrAssignExpr>(cst_node);

        IrForStmt* forStmt = new IrForStmt(initializer, condition, update, body, cst_node);
        this->ast_stack.push(forStmt);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitForStatement: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitInitList(const TSNode &cst_node) {
    try {
        IrInitializerList* initList = new IrInitializerList(cst_node);

        uint32_t child_count = ts_node_named_child_count(cst_node);
        for (uint32_t i = 0; i < child_count; i++) {
            IrExpr* expr = dynamic_cast<IrExpr*>(this->ast_stack.top());
            if (expr) {
                this->ast_stack.pop();
                initList->addElement(expr);
            }
        }

        this->ast_stack.push(initList);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitInitializerList: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitBreakStatement(const TSNode &cst_node) {
    IrBreakStmt* breakStmt = new IrBreakStmt(cst_node);
    this->ast_stack.push(breakStmt);
}

void ASTBuilder::exitCaseStatement(const TSNode &cst_node) {
    try {
        std::deque<IrStatement*> caseBody; // Store all statements inside the case

        // body: non_case_statement, declaration, (CURRENTLY NOT IMPLEMENTED type_definition)
        while (!ast_stack.empty() && dynamic_cast<IrStatement*>(ast_stack.top()) &&
            !dynamic_cast<IrCaseStmt*>(ast_stack.top())) {
            caseBody.push_front(popFromStack<IrStatement>(cst_node));
        }

        // Check if it's a default case
        IrExpr* caseValue = nullptr;
        std::string caseType = getNodeText(cst_node);
        // std::string::npos: special constant that means "not found"
        if (caseType.find("default") == std::string::npos) {
            caseValue = popFromStack<IrExpr>(cst_node);
        }

        IrCaseStmt* caseStmt = new IrCaseStmt(caseValue, caseBody, cst_node);
        ast_stack.push(caseStmt);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error in exitCaseStatement: " << e.what() << std::endl;
    }
}


void ASTBuilder::exitSwitchStatement(const TSNode &cst_node) {
    try {
        // 1. Pop the compound statement, which contains all case statements
        IrCompoundStmt* body = popFromStack<IrCompoundStmt>(cst_node);

        // 2. Pop the switch condition (inside parenthesis)
        IrParenthesizedExpr* switchExpr = popFromStack<IrParenthesizedExpr>(cst_node);

        // 3. Create an IrSwitchStmt node and push it onto the stack
        IrSwitchStmt* switchStmt = new IrSwitchStmt(switchExpr, body, cst_node);
        ast_stack.push(switchStmt);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error in exitSwitchStatement: " << e.what() << std::endl;
    }
}

void ASTBuilder::exitWhileStatement(const TSNode &cst_node) {
    try {
        IrStatement* body = this->popFromStack<IrStatement>(cst_node);

        TSNode conditionNode = ts_node_child_by_field_name(cst_node, "condition", 9);
        IrParenthesizedExpr* condition = nullptr;
        if (!ts_node_is_null(conditionNode)) {
            condition = this->popFromStack<IrParenthesizedExpr>(cst_node);
        } else {
            std::cerr << "Error: While statement missing condition.\n";
            return;
        }

        IrWhileStmt* whileStmt = new IrWhileStmt(condition, body, cst_node);
        this->ast_stack.push(whileStmt);
    } catch (const std::exception& e) {
        std::cerr << "Error in exitWhileStatement: " << e.what() << std::endl;
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
        case 360: // field_identifier, map field_identifier to exitIdentifier
            exitIdentifier(cst_node);
            break;
        case 362: // type_identifier
            exitTypeIdentifier(cst_node);
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
        case 147: // character
            exitLiteralChar(cst_node);
            break;
        case 318: // char_literal
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
            arraylevel -= 1;
            break;
        case 198: // declaration
            exitDeclaration(cst_node);
            break;
        case 240: // init_declarator already handled in declaration
            exitInitDeclarator(cst_node);
            break;
        case 229: // abstract_pointer_declarator
            exitAbstractPointerDeclarator(cst_node);
            break;
        case 226: // pointer_declarator
            exitPointerDeclarator(cst_node);
            break;
        case 253: // field_declaration
            exitFieldDeclaration(cst_node);
            break;
        case 251: // field_declaration_list
            exitFieldDeclarationList(cst_node);
            break;
        case 249: // struct_specifier
            exitStructSpecifier(cst_node);
            break;
        case 199: // type_definition
            exitTypeDefinition(cst_node);
            break;
        case 161: // translation_unit
            exitTransUnit(cst_node);
            break;
        case 310: // field_expression
            exitFieldExpression(cst_node);
            break;
        case 288: // pointer_expression
            exitPointerExpression(cst_node);
            break;
        case 18: // preproc_arg
            exitPreprocArg(cst_node);
            break;
        case 165: // preproc_def
            exitPreprocDef(cst_node);
            break;
        case 312: // parenthesized_expression
            exitParenthesizedExpr(cst_node);
            break;
        case 289: // unary_expression
            exitUnaryExpr(cst_node);
            break;
        case 267: // if_statement
            exitIfStatement(cst_node);
            break;
        case 268: // else_clause
            exitElseClause(cst_node);
            break;
        case 273: // for_statement
            exitForStatement(cst_node);
            break;
        case 313: // initializer_list
            exitInitList(cst_node);
            break;
        case 271: // while_statement
            exitWhileStatement(cst_node);
            break;
        case 276: // break_statement
            exitBreakStatement(cst_node);
            break;
        case 270: // case_statement
            exitCaseStatement(cst_node);
            break;
        case 269: // switch_statement
            exitSwitchStatement(cst_node);
            break;
        default:
            std::cerr << "Error: Unhandled CST node type: " ;
            std::cerr << ts_language_symbol_name(this->language, symbol_type) << ", symbol_type id:" << std::to_string(symbol_type) << std::endl;
            break;
    }
    return;
}

void ASTBuilder::enter_cst_node(const TSNode & cst_node){
     TSSymbol symbol_type = ts_node_symbol(cst_node);
     if (symbol_type == 236){
         arraylevel += 1;
     }
     else if (symbol_type == 298){
         arraylevel += 1;
     }

}

void ASTBuilder::traverse_tree(const TSNode & node) {

    enter_cst_node(node);

    uint32_t named_child_count = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < named_child_count; i++) {
        TSNode child = ts_node_named_child(node, i);
        traverse_tree(child);
    }

    exit_cst_node(node);
}

    // uint32_t child_count = ts_node_child_count(node);
    // for (uint32_t i = 0; i < child_count; i++) {
    //     TSNode child = ts_node_child(node, i);
    //     traverse_tree(child);
    // }

