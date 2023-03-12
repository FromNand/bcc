#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <common.h>

static char *code;
static Token *token;

int StartWith(char *keyword){
    return !memcmp(code, keyword, strlen(keyword));
}

void NewToken(TokenKind kind, char *string, int length){
    Token *new = malloc(sizeof(Token));
    new->kind = kind;
    new->string = string;
    code += new->length = length;
    new->value = 0;
    token = token->next = new;
}

Token* Lexer(char *input){
    char *string, *keywords[] = {"int", "if", "else", "for", "while", "return"};
    int i;
    Token head;
    code = input;
    token = &head;
    while(*code){
        if(isspace(*code)){
            code += 1;
        }
        else if(StartWith("==") || StartWith("!=") || StartWith("<=") || StartWith(">=")){
            NewToken(KEYWORD_TOKEN, code, 2);
        }
        else if(strchr("+-*/&=,;(){}<>", *code)){
            NewToken(KEYWORD_TOKEN, code, 1);
        }
        else if(isdigit(*code)){
            i = strtol(code, &string, 10);
            NewToken(NUMBER_TOKEN, code, string - code);
            token->value = i;
        }
        else if(isalpha(*code) || *code == '_'){
            for(i = 0; i < sizeof(keywords) / sizeof(char*); i++){
                string = code + strlen(keywords[i]);
                if(StartWith(keywords[i]) && !isalnum(*string) && *string != '_'){
                    NewToken(KEYWORD_TOKEN, code, string - code);
                    break;
                }
            }
            if(i == sizeof(keywords) / sizeof(char*)){
                for(string = code + 1; isalnum(*string) || *string == '_'; string++);
                NewToken(IDENTIFIER_TOKEN, code, string - code);
            }
        }
        else {
            SyntaxError(code, "Invalid token.");
        }
    }
    NewToken(TAIL_TOKEN, code, 0);
    return head.next;
}
