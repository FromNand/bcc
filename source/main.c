#include <common.h>

Token* Lexer(char *input);
Node* Parser(Token *token);
void Generator(Node *node);

char *input;

int main(int argc, char **argv){
    if(argc != 2){
        RuntimeError("One argument containing the source code is required.");
    }
    input = argv[1];
    Generator(Parser(Lexer(input)));
    return 0;
}
