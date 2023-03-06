#include <stdlib.h>
#include <string.h>
#include <common.h>

static Token *current;

int StartWith(char *source, char *keyword){
    return !memcmp(source, keyword, strlen(keyword));
}

void NewToken(TokenType type, char *string, int length){
    Token *new = malloc(sizeof(Token));
    new->type = type;
    new->string = string;
    new->length = length;
    new->value = 0;
    current = current->next = new;
}

Token* Lexer(char *source){
    char *base;
    int i;
    Token head;
    current = &head;
    while(*source){
        if(*source == '\t' || *source == '\n' || *source == '\r' || *source == ' '){
            source += 1;
        }
        else if(StartWith(source, "==") || StartWith(source, "!=") || StartWith(source, "<=") || StartWith(source, ">=")){
            NewToken(KEYWORD_TOKEN, source, 2);
            source += 2;
        }
        else if(strchr("+-*/()<>", *source)){
            NewToken(KEYWORD_TOKEN, source, 1);
            source += 1;
        }
        else if('0' <= *source && *source <= '9'){
            base = source;
            i = strtol(source, &source, 10);
            NewToken(NUMBER_TOKEN, base, source - base);
            current->value = i;
        }
        else {
            SyntaxError(source, "Invalid token.");
        }
    }
    NewToken(TAIL_TOKEN, source, 0);
    return head.next;
}
