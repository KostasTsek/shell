#include <stdio.h>
#include "./tokenizer/tokenizer.h"

int main(){
    char in[MAXVAL];

    char c = getc(stdin);
    int i = 0;
    while(c != '\n'){
        in[i]=c;
        c = getc(stdin);
        i++;
    }
    in[i] = '\0';

    Scanner(in, &tokens_arr);

    for(int i = 0; tokens_arr.tokens[i] != NULL; i++){
        printf("Token value: %s,", tokens_arr.tokens[i]->value);
    }

    return 0;
}