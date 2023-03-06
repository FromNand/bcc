#include <stdlib.h>
#include <string.h>
#include <common.h>

Node* Expression(void);

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

Node* Multiplication(void){
    Node *node = Primary(), *new;
    while(1){
        if(ConsumeKeyword("*")){
            new = NewNode(MULTIPLICATION_NODE);
            node = BinaryNode(new, node, Primary());
        }
        else if(ConsumeKeyword("/")){
            new = NewNode(DIVISION_NODE);
            node = BinaryNode(new, node, Primary());
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

Node* Expression(void){
    return Addition();
}

Node* Parser(Token *token){
    Node *node;
    current = token;
    node = NewNode(PROGRAM_NODE);
    node->child1 = Expression();
    return node;
}
