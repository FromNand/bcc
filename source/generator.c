#include <stdio.h>
#include <common.h>

void Generator(Node *node);
#define LOCAL_VARIABLE_SIZE (8)
static int localVariableCounter;

void GenerateLeftValue(Node *node){
    switch(node->kind){
        case LOCAL_VARIABLE_NODE:
            printf("    lea     rax, [rbp-%d]\n", LOCAL_VARIABLE_SIZE * (localVariableCounter - node->number1));
            return;
        case DEREFERENCE_NODE:
            Generator(node->child1);
            return;
        default:
            SyntaxError(node->token->string, "Left value is required.");
    }
}

void Generator(Node *node){
    static int counter;
    int i = counter++;
    if(!node){
        return;
    }
    switch(node->kind){
        case NUMBER_NODE:
            printf("    mov     rax, %d\n", node->token->value);
            return;
        case LOCAL_VARIABLE_NODE:
            printf("    mov     rax, [rbp-%d]\n", LOCAL_VARIABLE_SIZE * (localVariableCounter - node->number1));
            return;
        case MINUS_NODE:
            Generator(node->child1);
            printf("    neg     rax\n");
            return;
        case ADDRESS_NODE:
            GenerateLeftValue(node->child1);
            return;
        case DEREFERENCE_NODE:
            Generator(node->child1);
            printf("    mov     rax, [rax]\n");
            return;
        case ASSIGNMENT_NODE:
            GenerateLeftValue(node->child1);
            printf("    push    rax\n");
            Generator(node->child2);
            printf("    pop     rdi\n");
            printf("    mov     [rdi], rax\n");
            return;
        case BLOCK_NODE:
            for(i = 0; i < node->number1; i++){
                Generator(node->childs[i]);
            }
            return;
        case IF_NODE:
            Generator(node->child1);
            printf("    cmp     rax, 0\n");
            printf("    je      .LElse%d\n", i);
            Generator(node->child2);
            printf("    jmp     .LEnd%d\n", i);
            printf(".LElse%d:\n", i);
            Generator(node->child3);
            printf(".LEnd%d:\n", i);
            return;
        case FOR_WHILE_NODE:
            Generator(node->child1);
            printf(".LBegin%d:\n", i);
            if(node->child2){
                Generator(node->child2);
                printf("    cmp     rax, 0\n");
                printf("    je      .LEnd%d\n", i);
            }
            Generator(node->child4);
            Generator(node->child3);
            printf("    jmp     .LBegin%d\n", i);
            printf(".LEnd%d:\n", i);
            return;
        case RETURN_NODE:
            Generator(node->child1);
            printf("    mov     rsp, rbp\n");
            printf("    pop     rbp\n");
            printf("    ret\n");
            return;
        case PROGRAM_NODE:
            localVariableCounter = node->number1;
            printf(".intel_syntax noprefix\n");
            printf(".global main\n\n");
            printf("main:\n");
            printf("    push    rbp\n");
            printf("    mov     rbp, rsp\n");
            printf("    sub     rsp, %d\n", LOCAL_VARIABLE_SIZE * localVariableCounter);
            Generator(node->child1);
            printf("    mov     rsp, rbp\n");
            printf("    pop     rbp\n");
            printf("    ret\n");
            return;
        default:
            Generator(node->child2);
            printf("    push    rax\n");
            Generator(node->child1);
            printf("    pop     rdi\n");
    }
    switch(node->kind){
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
            printf("    cmp     rax, rdi\n");
    }
    switch(node->kind){
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
