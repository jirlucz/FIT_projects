#include "../fluid_string.c"
#include "../fluid_string.h"
#include <stdio.h>

int main (){

    FLUIDS testString;
    bool bool_return;
    int NoErr = 0;
    if (FluidInit(&testString) == false)
    {
            printf ("Chyba Inicializace\n");
            NoErr ++;
    }

    else
    {    
        printf("Inicializace uspesna, alokovana delka %d, aktualnd delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
    }

  
     bool_return = FluidAddChar(&testString, 'h');
        if (bool_return != false)
        {
            printf("Pridani uspesne, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        else
        {
            printf ("Chyba pridani charu\n");
            NoErr++;
        }
    bool_return = FluidAddChar(&testString, 'e');
        if (bool_return != false)
        {
            printf("Pridani uspesne, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        else
        {
            printf ("Chyba pridani charu\n");
            NoErr++;
        }
    bool_return = FluidAddChar(&testString, 'l');
        if (bool_return != false)
        {
            printf("Pridani uspesne, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        else
        {
            printf ("Chyba pridani charu\n");
            NoErr++;
        }
    bool_return = FluidAddChar(&testString, 'l');
        if (bool_return != false)
        {
            printf("Pridani uspesne, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        else
        {
            printf ("Chyba pridani charu\n");
            NoErr++;
        }
    bool_return = FluidAddChar(&testString, 'o');
        if (bool_return != false)
        {
            printf("Inicializace uspesna, alokovana delka %u, aktualni delka %u, obsah retezce:%sEND\n", testString.length, testString.actualLength, testString.payload );
        }
        else
        {
            printf ("Chyba pridani charu\n");
            NoErr++;
        }

    char string2cmp [] = "hello";
    bool_return =CmpFluidS(&testString, string2cmp);
        if (bool_return == true)
        {
            printf("Retezce jsou stejne\n");
        }
        else
        {
            printf("Retezce jsou rozdilne\n");
            NoErr++;
        }
        

    bool flush = FluidFlush(&testString);
        if (flush == true)
        {
            printf("smazani bylo uspesne\n");
        }
        else
        {
            printf("Neuspech pri uvolnovani pameti FluidFlush\n");
            NoErr++;
        }

    if (NoErr == 0)
    {
        printf("\033[1;32m");  // 1;32m zelena
        printf("Test probehl uspesne\n");
        printf("\033[0m");     // reset na normalni
        return 0;
    }
    else
    {
        printf("\033[1;31m");  // 1;32m zelena
        printf("Testy byly neuspesne\n");
        printf("\033[0m");     // reset na normalni
        return 0;
    }
    
}



