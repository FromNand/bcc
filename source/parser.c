#include <stdlib.h>
#include <string.h>
#include <common.h>

Node* Expression(void);

#define CHILDS_MAX 10
static Token *previous, *current;

void UpdateToken(void){
    previous = current;
    current = current->next;
}

int ConsumeToken(TokenType type){
    if(current->type != type){
        return 0;
    }
    UpdateToken();
    return 1;
}

int ConsumeKeyword(char *keyword){
    if(current->type != KEYWORD_TOKEN || memcmp(current->string, keyword, strlen(keyword)) || current->length != strlen(keyword)){
        return 0;
    }
    UpdateToken();
    return 1;
}

void ExpectKeyword(char *keyword){
    if(current->type != KEYWORD_TOKEN || memcmp(current->string, keyword, strlen(keyword)) || current->length != strlen(keyword)){
        SyntaxError(current->string, "\"%s\" expected.", keyword);
    }
    UpdateToken();
}

Node* NewNode(NodeType type){
    Node *new = malloc(sizeof(Node));
    new->type = type;
    new->token = previous;
    new->child1 = NULL;
    new->child2 = NULL;
    new->childs = NULL;
    return new;
}

Node* BinaryNode(Node *node, Node *child1, Node *child2){
    node->child1 = child1;
    node->child2 = child2;
    return node;
}

Node* Primary(void){
    Node *node;
    if(ConsumeToken(NUMBER_TOKEN)){
        node = NewNode(NUMBER_NODE);
    }
    else {
        ExpectKeyword("(");
        node = Expression();
        ExpectKeyword(")");
    }
    return node;
}

Node* Unary(void){
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

Node* Multiplication(void){
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

Node* Addition(void){
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

Node* Relational(void){
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

Node* Equality(void){
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

Node* Expression(void){
    return Equality();
}

Node* Statement(void){
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
    return node;
}
