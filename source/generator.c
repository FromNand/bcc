#include <stdio.h>
#include <common.h>

void Generator(Node *node){
    switch(node->type){
        case NUMBER_NODE:
            printf("    push    %d\n", node->token->value);
            return;
        case PROGRAM_NODE:
            printf(".intel_syntax noprefix\n");
            printf(".global main\n\n");
            printf("main:\n");
            Generator(node->child1);
            printf("    pop     rax\n");
            printf("    ret\n");
            return;
        default:
            Generator(node->child1);
            Generator(node->child2);
            printf("    pop     rdi\n");
            printf("    pop     rax\n");
    }
    switch(node->type){
        case MULTIPLICATION_NODE:
            printf("    imul    rax, rdi\n");
            break;
        case DIVISION_NODE:
            printf("    cqo\n");
            printf("    idiv    rdi\n");
            break;
        case ADDITION_NODE:
            printf("    add     rax, rdi\n");
            break;
        case SUBTRACTION_NODE:
            printf("    sub     rax, rdi\n");
            break;
        default:
            SyntaxError(node->token->string, "Invalid node.");
    }
    printf("    push    rax\n");
}
