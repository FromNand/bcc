#include <stdio.h>
#include <common.h>

void Generator(Node *node){
    int i = 0;
    switch(node->type){
        case NUMBER_NODE:
            printf("    mov     rax, %d\n", node->token->value);
            return;
        case NEGATION_NODE:
            Generator(node->child1);
            printf("    neg     rax\n");
            return;
        case NULL_NODE:
            return;
        case PROGRAM_NODE:
            printf(".intel_syntax noprefix\n");
            printf(".global main\n\n");
            printf("main:\n");
            while(node->childs[i]){
                Generator(node->childs[i++]);
            }
            printf("    ret\n");
            return;
        default:
    }
    Generator(node->child2);
    printf("    push    rax\n");
    Generator(node->child1);
    printf("    pop     rdi\n");
    switch(node->type){
        case MULTIPLICATION_NODE:
            printf("    imul    rax, rdi\n");
            return;
        case DIVISION_NODE:
            printf("    cqo\n");
            printf("    idiv    rdi\n");
            return;
        case ADDITION_NODE:
            printf("    add     rax, rdi\n");
            return;
        case SUBTRACTION_NODE:
            printf("    sub     rax, rdi\n");
            return;
        default:
    }
    printf("    cmp     rax, rdi\n");
    switch(node->type){
        case LESS_NODE:
            printf("    setl    al\n");
            break;
        case LESS_EQUAL_NODE:
            printf("    setle   al\n");
            break;
        case EQUALITY_NODE:
            printf("    sete    al\n");
            break;
        case INEQUALITY_NODE:
            printf("    setne   al\n");
            break;
        default:
            SyntaxError(node->token->string, "Invalid node.");
    }
    printf("    movzb   rax, al\n");
}
