#include <stdlib.h>
#include <string.h>
#include <common.h>

static Node* Expression(void);

typedef struct LocalVariable {
    char *string;
    int length;
    int offset;
    struct LocalVariable *next;
} LocalVariable;

#define CHILDS_MAX (10)
static int localVariableCounter;
static LocalVariable *localVariableHead;
static Token *previous, *current;

static void UpdateToken(void){
    previous = current;
    current = current->next;
}

static int ConsumeToken(TokenType type){
    if(current->type != type){
        return 0;
    }
    UpdateToken();
    return 1;
}

static int ConsumeKeyword(char *keyword){
    if(current->type != KEYWORD_TOKEN || memcmp(current->string, keyword, strlen(keyword)) || current->length != strlen(keyword)){
        return 0;
    }
    UpdateToken();
    return 1;
}

static void ExpectKeyword(char *keyword){
    if(current->type != KEYWORD_TOKEN || memcmp(current->string, keyword, strlen(keyword)) || current->length != strlen(keyword)){
        SyntaxError(current->string, "\"%s\" expected.", keyword);
    }
    UpdateToken();
}

static Node* NewNode(NodeType type){
    Node *new = malloc(sizeof(Node));
    new->type = type;
    new->token = previous;
    new->number1 = 0;
    new->child1 = NULL;
    new->child2 = NULL;
    new->child3 = NULL;
    new->child4 = NULL;
    new->childs = NULL;
    return new;
}

static Node* BinaryNode(Node *node, Node *child1, Node *child2){
    node->child1 = child1;
    node->child2 = child2;
    return node;
}

static int FindLocalVariable(Token *token){
    LocalVariable *variable;
    for(variable = localVariableHead; variable; variable = variable->next){
        if(!memcmp(variable->string, token->string, token->length) && variable->length == token->length){
            return variable->offset;
        }
    }
    variable = malloc(sizeof(LocalVariable));
    variable->string = token->string;
    variable->length = token->length;
    variable->offset = localVariableCounter++;
    variable->next = localVariableHead;
    localVariableHead = variable;
    return localVariableHead->offset;
}

static Node* Primary(void){
    Node *node;
    if(ConsumeToken(NUMBER_TOKEN)){
        node = NewNode(NUMBER_NODE);
    }
    else if(ConsumeToken(IDENTIFIER_TOKEN)){
        node = NewNode(LOCAL_VARIABLE_NODE);
        node->number1 = FindLocalVariable(node->token);
    }
    else if(ConsumeKeyword("(")){
        node = Expression();
        ExpectKeyword(")");
    }
    else {
        SyntaxError(current->string, "Number, variable or (expression) expected.");
    }
    return node;
}

static Node* Unary(void){
    Node *node;
    if(ConsumeKeyword("+")){
        node = Unary();
    }
    else if(ConsumeKeyword("-")){
        node = NewNode(NEGATION_NODE);
        node->child1 = Unary();
    }
    else {
        node = Primary();
    }
    return node;
}

static Node* Multiplication(void){
    Node *node = Unary(), *new;
    while(1){
        if(ConsumeKeyword("*")){
            new = NewNode(MULTIPLICATION_NODE);
            node = BinaryNode(new, node, Unary());
        }
        else if(ConsumeKeyword("/")){
            new = NewNode(DIVISION_NODE);
            node = BinaryNode(new, node, Unary());
        }
        else {
            return node;
        }
    }
}

static Node* Addition(void){
    Node *node = Multiplication(), *new;
    while(1){
        if(ConsumeKeyword("+")){
            new = NewNode(ADDITION_NODE);
            node = BinaryNode(new, node, Multiplication());
        }
        else if(ConsumeKeyword("-")){
            new = NewNode(SUBTRACTION_NODE);
            node = BinaryNode(new, node, Multiplication());
        }
        else {
            return node;
        }
    }
}

static Node* Relational(void){
    Node *node = Addition(), *new;
    while(1){
        if(ConsumeKeyword("<")){
            new = NewNode(LESS_NODE);
            node = BinaryNode(new, node, Addition());
        }
        else if(ConsumeKeyword("<=")){
            new = NewNode(LESS_EQUAL_NODE);
            node = BinaryNode(new, node, Addition());
        }
        else if(ConsumeKeyword(">")){
            new = NewNode(LESS_NODE);
            node = BinaryNode(new, Addition(), node);
        }
        else if(ConsumeKeyword(">=")){
            new = NewNode(LESS_EQUAL_NODE);
            node = BinaryNode(new, Addition(), node);
        }
        else {
            return node;
        }
    }
}

static Node* Equality(void){
    Node *node = Relational(), *new;
    while(1){
        if(ConsumeKeyword("==")){
            new = NewNode(EQUALITY_NODE);
            node = BinaryNode(new, node, Relational());
        }
        else if(ConsumeKeyword("!=")){
            new = NewNode(INEQUALITY_NODE);
            node = BinaryNode(new, node, Relational());
        }
        else {
            return node;
        }
    }
}

static Node* Assignment(void){
    Node *node = Equality(), *new;
    if(ConsumeKeyword("=")){
        new = NewNode(ASSIGNMENT_NODE);
        node = BinaryNode(new, node, Assignment());
    }
    return node;
}

static Node* Expression(void){
    return Assignment();
}

static Node* Statement(void){
    int i = 0;
    Node *node;
    if(ConsumeKeyword(";")){
        node = NewNode(NULL_NODE);
    }
    else if(ConsumeKeyword("{")){
        node = NewNode(BLOCK_NODE);
        node->childs = malloc(sizeof(Node*) * CHILDS_MAX);
        while(!ConsumeKeyword("}")){
            if(i >= CHILDS_MAX - 1){
                SyntaxError(current->string, "At most %d statements are allowed in block statement.", CHILDS_MAX - 1);
            }
            node->childs[i++] = Statement();
        }
        node->childs[i] = NULL;
    }
    else if(ConsumeKeyword("if")){
        node = NewNode(IF_NODE);
        ExpectKeyword("(");
        node->child1 = Expression();
        ExpectKeyword(")");
        node->child2 = Statement();
        if(ConsumeKeyword("else")){
            node->child3 = Statement();
        }
    }
    else if(ConsumeKeyword("for")){
        node = NewNode(FOR_WHILE_NODE);
        ExpectKeyword("(");
        if(!ConsumeKeyword(";")){
            node->child1 = Expression();
            ExpectKeyword(";");
        }
        if(!ConsumeKeyword(";")){
            node->child2 = Expression();
            ExpectKeyword(";");
        }
        if(!ConsumeKeyword(")")){
            node->child3 = Expression();
            ExpectKeyword(")");
        }
        node->child4 = Statement();
    }
    else if(ConsumeKeyword("while")){
        node = NewNode(FOR_WHILE_NODE);
        ExpectKeyword("(");
        node->child2 = Expression();
        ExpectKeyword(")");
        node->child4 = Statement();
    }
    else if(ConsumeKeyword("return")){
        node = NewNode(RETURN_NODE);
        node->child1 = Expression();
        ExpectKeyword(";");
    }
    else {
        node = Expression();
        ExpectKeyword(";");
    }
    return node;
}

Node* Parser(Token *token){
    Node *node;
    current = token;
    node = NewNode(PROGRAM_NODE);
    node->child1 = Statement();
    node->number1 = localVariableCounter;
    return node;
}
