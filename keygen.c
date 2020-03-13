#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


int validate(char* s){
    int i=0;
    for (i=0; i < strlen(s);i++){
        //if it's not a number
        if (s[i] < 48 || s[i] > 57){
            return -1;
        }
    }
    return atoi(s);
}

void keygen(char* out, int n){
    int i = 0;
    int r = 0;
    for (i=0; i < n; i++){
        r = rand() % 27;
        if (r == 26){
            out[i] = ' ';
        } else {
            out[i] = r + 'A';
        }
    }
}

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc < 2){
        printf("ERROR: requires keylength\n");
        return 1;
    }
    //validate the key length
    int keylength = validate(argv[1]);
    if (keylength < 1){
        printf("ERROR: invalid keylength\n");
    }
    char* key = malloc(sizeof(char)*keylength);
    keygen(key, keylength);
    printf("%s\n", key);
    free(key);



    return 0;
}