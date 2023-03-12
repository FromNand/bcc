#define RuntimeError(format, ...) RuntimeErrorInternal(__FILE__, __LINE__, format, ##__VA_ARGS__)
void RuntimeErrorInternal(char *file, int line, char *format, ...);
void SyntaxError(char *position, char *format, ...);

typedef enum {
    KEYWORD_TOKEN, NUMBER_TOKEN, IDENTIFIER_TOKEN, TAIL_TOKEN
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char *string;
    int length;
    int value;
    struct Token *next;
} Token;

typedef enum {
    NUMBER_NODE, LOCAL_VARIABLE_NODE,
    MINUS_NODE, ADDRESS_NODE, DEREFERENCE_NODE,
    MULTIPLICATION_NODE, DIVISION_NODE,
    ADDITION_NODE, SUBTRACTION_NODE,
    LESS_NODE, LESS_EQUAL_NODE,
    EQUALITY_NODE, INEQUALITY_NODE,
    ASSIGNMENT_NODE,
    BLOCK_NODE, IF_NODE, FOR_WHILE_NODE, RETURN_NODE,
    PROGRAM_NODE
} NodeKind;

typedef struct Node {
    NodeKind kind;
    Token *token;
    int number1;
    struct Node *child1;
    struct Node *child2;
    struct Node *child3;
    struct Node *child4;
    struct Node **childs;
} Node;
