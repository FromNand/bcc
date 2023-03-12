#include <stdlib.h>
#include <string.h>
#include <common.h>

Node* Expression(void);

typedef struct LocalVariable {
    char *string;
    int length;
    int offset;
    struct LocalVariable *next;
} LocalVariable;

#define CHILDS_MAX (10)
static int localVariableCounter;
static LocalVariable *localVariableHead;
static Token *previous, *current, *next;

void UpdateToken(void){
    previous = current;
    current = next;
    next = next->next;
}

int ConsumeToken(TokenKind kind){
    if(next->kind != kind){
        return 0;
    }
    UpdateToken();
    return 1;
}

int ConsumeKeyword(char *keyword){
    if(next->kind != KEYWORD_TOKEN || memcmp(next->string, keyword, strlen(keyword)) || next->length != strlen(keyword)){
        return 0;
    }
    UpdateToken();
    return 1;
}

void RequireKeyword(char *keyword){
    if(next->kind != KEYWORD_TOKEN || memcmp(next->string, keyword, strlen(keyword)) || next->length != strlen(keyword)){
        SyntaxError(next->string, "'%s' is required.", keyword);
    }
    UpdateToken();
}

Node* NewNode(NodeKind kind){
    Node *new = malloc(sizeof(Node));
    new->kind = kind;
    new->token = current;
    new->number1 = 0;
    new->child1 = NULL;
    new->child2 = NULL;
    new->child3 = NULL;
    new->child4 = NULL;
    new->childs = NULL;
    return new;
}

Node* MakeBinaryTree(Node *node, Node *child1, Node *child2){
    node->child1 = child1;
    node->child2 = child2;
    return node;
}

Node* Primary(void){
    LocalVariable *localVariable;
    Node *node;
    if(ConsumeToken(NUMBER_TOKEN)){
        node = NewNode(NUMBER_NODE);
    }
    else if(ConsumeToken(IDENTIFIER_TOKEN)){
        node = NewNode(LOCAL_VARIABLE_NODE);
        for(localVariable = localVariableHead; localVariable; localVariable = localVariable->next){
            if(!memcmp(localVariable->string, node->token->string, node->token->length) && localVariable->length == node->token->length){
                node->number1 = localVariable->offset;
                break;
            }
        }
        if(!localVariable){
            SyntaxError(node->token->string, "'%.*s' is undefined.", node->token->length, node->token->string);
        }
    }
    else if(ConsumeKeyword("(")){
        node = Expression();
        RequireKeyword(")");
    }
    else {
        SyntaxError(next->string, "Number, variable, or (expression) is required.");
    }
    return node;
}

Node* Unary(void){
    Node *node;
    if(ConsumeKeyword("+")){
        node = Unary();
    }
    else if(ConsumeKeyword("-")){
        node = NewNode(MINUS_NODE);
        node->child1 = Unary();
    }
    else if(ConsumeKeyword("&")){
        node = NewNode(ADDRESS_NODE);
        node->child1 = Unary();
    }
    else if(ConsumeKeyword("*")){
        node = NewNode(DEREFERENCE_NODE);
        node->child1 = Unary();
    }
    else {
        node = Primary();
    }
    return node;
}

Node* Multiplication(void){
    Node *node = Unary(), *new;
    while(1){
        if(ConsumeKeyword("*")){
            new = NewNode(MULTIPLICATION_NODE);
            node = MakeBinaryTree(new, node, Unary());
        }
        else if(ConsumeKeyword("/")){
            new = NewNode(DIVISION_NODE);
            node = MakeBinaryTree(new, node, Unary());
        }
        else {
            return node;
        }
    }
}

Node* Addition(void){
    Node *node = Multiplication(), *new;
    while(1){
        if(ConsumeKeyword("+")){
            new = NewNode(ADDITION_NODE);
            node = MakeBinaryTree(new, node, Multiplication());
        }
        else if(ConsumeKeyword("-")){
            new = NewNode(SUBTRACTION_NODE);
            node = MakeBinaryTree(new, node, Multiplication());
        }
        else {
            return node;
        }
    }
}

Node* Relational(void){
    Node *node = Addition(), *new;
    while(1){
        if(ConsumeKeyword("<")){
            new = NewNode(LESS_NODE);
            node = MakeBinaryTree(new, node, Addition());
        }
        else if(ConsumeKeyword("<=")){
            new = NewNode(LESS_EQUAL_NODE);
            node = MakeBinaryTree(new, node, Addition());
        }
        else if(ConsumeKeyword(">")){
            new = NewNode(LESS_NODE);
            node = MakeBinaryTree(new, Addition(), node);
        }
        else if(ConsumeKeyword(">=")){
            new = NewNode(LESS_EQUAL_NODE);
            node = MakeBinaryTree(new, Addition(), node);
        }
        else {
            return node;
        }
    }
}

Node* Equality(void){
    Node *node = Relational(), *new;
    while(1){
        if(ConsumeKeyword("==")){
            new = NewNode(EQUALITY_NODE);
            node = MakeBinaryTree(new, node, Relational());
        }
        else if(ConsumeKeyword("!=")){
            new = NewNode(INEQUALITY_NODE);
            node = MakeBinaryTree(new, node, Relational());
        }
        else {
            return node;
        }
    }
}

Node* Assignment(void){
    Node *node = Equality(), *new;
    if(ConsumeKeyword("=")){
        new = NewNode(ASSIGNMENT_NODE);
        node = MakeBinaryTree(new, node, Assignment());
    }
    return node;
}

Node* Expression(void){
    return Assignment();
}

Node* Statement(void){
    LocalVariable *localVariable;
    Node *node, *node2;
    if(ConsumeKeyword(";")){
        node = NULL;
    }
    else if(ConsumeKeyword("{")){
        node = NewNode(BLOCK_NODE);
        node->childs = malloc(sizeof(Node*) * CHILDS_MAX);
        while(!ConsumeKeyword("}")){
            if(node->number1 >= CHILDS_MAX - 1){
                SyntaxError(next->string, "Up to %d statements can be used in a block statement.", CHILDS_MAX - 1);
            }
            node->childs[node->number1++] = Statement();
        }
    }
    else if(ConsumeKeyword("int")){
        node = NewNode(BLOCK_NODE);
        node->childs = malloc(sizeof(Node*) * CHILDS_MAX);
        do {
            if(node->number1){
                RequireKeyword(",");
            }
            while(ConsumeKeyword("*"));
            if(node->number1 >= CHILDS_MAX - 1){
                SyntaxError(next->string, "Up to %d variables can be initialized at one time.", CHILDS_MAX - 1);
            }
            if(!ConsumeToken(IDENTIFIER_TOKEN)){
                SyntaxError(next->string, "'%.*s' cannot be used in a variable name.", next->length, next->string);
            }
            for(localVariable = localVariableHead; localVariable; localVariable = localVariable->next){
                if(!memcmp(localVariable->string, current->string, current->length) && localVariable->length == current->length){
                    SyntaxError(current->string, "'%.*s' is redefined.", current->length, current->string);
                }
            }
            localVariable = malloc(sizeof(LocalVariable));
            localVariable->string = current->string;
            localVariable->length = current->length;
            localVariable->offset = localVariableCounter++;
            localVariable->next = localVariableHead;
            localVariableHead = localVariable;
            if(ConsumeKeyword("=")){
                node->childs[node->number1] = NewNode(ASSIGNMENT_NODE);
                node2 = NewNode(LOCAL_VARIABLE_NODE);
                node2->token = previous;
                node2->number1 = localVariableHead->offset;
                MakeBinaryTree(node->childs[node->number1++], node2, Expression());
            }
            else {
                node->childs[node->number1++] = NULL;
            }
        } while(!ConsumeKeyword(";"));
    }
    else if(ConsumeKeyword("if")){
        node = NewNode(IF_NODE);
        RequireKeyword("(");
        node->child1 = Expression();
        RequireKeyword(")");
        node->child2 = Statement();
        if(ConsumeKeyword("else")){
            node->child3 = Statement();
        }
    }
    else if(ConsumeKeyword("for")){
        node = NewNode(FOR_WHILE_NODE);
        RequireKeyword("(");
        if(!ConsumeKeyword(";")){
            node->child1 = Expression();
            RequireKeyword(";");
        }
        if(!ConsumeKeyword(";")){
            node->child2 = Expression();
            RequireKeyword(";");
        }
        if(!ConsumeKeyword(")")){
            node->child3 = Expression();
            RequireKeyword(")");
        }
        node->child4 = Statement();
    }
    else if(ConsumeKeyword("while")){
        node = NewNode(FOR_WHILE_NODE);
        RequireKeyword("(");
        node->child2 = Expression();
        RequireKeyword(")");
        node->child4 = Statement();
    }
    else if(ConsumeKeyword("return")){
        node = NewNode(RETURN_NODE);
        node->child1 = Expression();
        RequireKeyword(";");
    }
    else {
        node = Expression();
        RequireKeyword(";");
    }
    return node;
}

Node* Parser(Token *token){
    Node *node;
    next = token;
    node = NewNode(PROGRAM_NODE);
    node->token = next;
    node->child1 = Statement();
    node->number1 = localVariableCounter;
    return node;
}
