#define RuntimeError(format, ...) RuntimeErrorInternal(__FILE__, __LINE__, format, ##__VA_ARGS__)
void RuntimeErrorInternal(char *file, int line, char *format, ...);
void SyntaxError(char *location, char *format, ...);

typedef enum {
    KEYWORD_TOKEN, NUMBER_TOKEN, TAIL_TOKEN
} TokenType;

typedef struct Token {
    TokenType type;
    char *string;
    int length;
    int value;
    struct Token *next;
} Token;

typedef enum {
    NUMBER_NODE,
    MULTIPLICATION_NODE, DIVISION_NODE,
    ADDITION_NODE, SUBTRACTION_NODE,
    PROGRAM_NODE
} NodeType;

typedef struct Node {
    NodeType type;
    Token *token;
    struct Node *child1;
    struct Node *child2;
} Node;
