#include <common.h>

Token* Lexer(char *source);
Node* Parser(Token *token);
void Generator(Node *node);

char *currentSource;

int main(int argc, char **argv){
    if(argc != 2){
        RuntimeError("One argument containing source code expected.");
    }
    currentSource = argv[1];
    Generator(Parser(Lexer(currentSource)));
    return 0;
}
