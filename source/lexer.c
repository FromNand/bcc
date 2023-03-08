#include <stdlib.h>
#include <string.h>
#include <common.h>

static Token *current;

static int StartWith(char *source, char *keyword){
    return !memcmp(source, keyword, strlen(keyword));
}

static int IsIdentifierCharacter(char character){
    return ('0' <= character && character <= '9') || ('A' <= character && character <= 'Z') || ('a' <= character && character <= 'z') || character == '_';
}

static void NewToken(TokenKind kind, char *string, int length){
    Token *new = malloc(sizeof(Token));
    new->kind = kind;
    new->string = string;
    new->length = length;
    new->value = 0;
    current = current->next = new;
}

Token* Lexer(char *source){
    char *base;
    static char *keywords[] = {"int", "if", "else", "for", "while", "return"};
    int i, j;
    Token head;
    current = &head;
    while(*source){
        for(i = 0; i < sizeof(keywords) / sizeof(char*); i++){
            j = strlen(keywords[i]);
            if(StartWith(source, keywords[i]) && !IsIdentifierCharacter(source[j])){
                NewToken(KEYWORD_TOKEN, source, j);
                source += j;
                break;
            }
        }
        if(*source == '\t' || *source == '\n' || *source == '\r' || *source == ' '){
            source += 1;
        }
        else if(StartWith(source, "==") || StartWith(source, "!=") || StartWith(source, "<=") || StartWith(source, ">=")){
            NewToken(KEYWORD_TOKEN, source, 2);
            source += 2;
        }
        else if(strchr("+-*/=&,;(){}<>", *source)){
            NewToken(KEYWORD_TOKEN, source, 1);
            source += 1;
        }
        else if('0' <= *source && *source <= '9'){
            base = source;
            i = strtol(source, &source, 10);
            NewToken(NUMBER_TOKEN, base, source - base);
            current->value = i;
        }
        else if(IsIdentifierCharacter(*source)){
            for(base = source++; IsIdentifierCharacter(*source); source++);
            NewToken(IDENTIFIER_TOKEN, base, source - base);
        }
        else {
            SyntaxError(source, "Invalid token.");
        }
    }
    NewToken(TAIL_TOKEN, source, 0);
    return head.next;
}
