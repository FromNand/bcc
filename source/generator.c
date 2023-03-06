#include <stdio.h>
#include <common.h>

void Generator(Node *node){
    switch(node->type){
        case NUMBER_NODE:
            printf("    mov     rax, %d\n", node->token->value);
            return;
        case NEGATION_NODE:
            Generator(node->child1);
            printf("    neg     rax\n");
            return;
        case PROGRAM_NODE:
            printf(".intel_syntax noprefix\n");
            printf(".global main\n\n");
            printf("main:\n");
            Generator(node->child1);
            printf("    ret\n");
            return;
        default:
            Generator(node->child2);
            printf("    push    rax\n");
            Generator(node->child1);
            printf("    pop     rdi\n");
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
}
