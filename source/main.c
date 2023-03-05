#include <common.h>

Token* Lexer(char *source);
Node* Parser(Token *token);
void Generator(Node *node);

char *currentSource;

int main(int argc, char **argv){
    if(argc != 2){
        RuntimeError("Expected one argument containing source code.");
    }
    currentSource = argv[1];
    Generator(Parser(Lexer(currentSource)));
    return 0;
}
