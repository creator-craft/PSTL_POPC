#include "POP_external/Level.h"
#include <stdio.h>

int main(){
    LEVEL *lvl = createLevel();
    if(lvl == NULL){
        printf("Err 1 : L'allocation mémoire n'a pas pu être effectué\n");
        return 1;
    }
    int r = initiateLevel(lvl, "LEVEL/LEVEL0");
    if (r == 1){
        printf("Err 2 : La récupération des données du niveau n'a pu être effectué\n");
        return 1;
    }
    return 0;
}