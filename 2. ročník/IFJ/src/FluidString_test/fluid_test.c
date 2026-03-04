#include "../fluid_string.c"
#include "../fluid_string.h"
#include <stdio.h>

int main (){

    FLUIDS testString;
    bool bool_return;
    if (FluidInit(&testString) == false)
    {
            printf ("Chyba Inicializace\n");
            return -1;
    }

    else
    {    
        printf("Inicializace uspesna, alokovana delka %d, aktualnd delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
    }

   for (int i = 0; i < 20; i++)
   {
     bool_return = FluidAddChar(&testString, '_');
        if (bool_return != false)
        {
            printf("Inicializace uspesna, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        
        else
        {
            printf ("Chyba pridani charu\n");
            return -1;
        }
   }
   

    bool flush = FluidFlush(&testString);
        if (flush == true)
        {
            printf("smazani bylo uspesne\n");
        }
        else
        {
            printf("Neuspech pri uvolnovani pameti FluidFlush\n");
            return -1;
        }

    printf("\033[1;32m");  // 1;32m zelena
    printf("Test probehl uspesne\n");
    printf("\033[0m");     // reset na normalni
    return 0;
}



