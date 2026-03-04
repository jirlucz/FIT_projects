/**
 * @file main.c
 *
 * @brief
 *
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 * @author Dominik Honza <xhonza04@stud.fit.vutbr.cz>
 * @author David Nepraš <xnepra02@stud.fit.vutbr.cz>
 * @author Jiří Kotek <xkotek09@stud.fit.vutbr.cz>
 */

#include "main.h"

#ifdef DEBUG
#define PRINT_ERR_CODE(errCode) fprintf(stderr, "Parsing error code: %d\n", errCode)
#else
#define PRINT_ERR_CODE(errCode)
#endif

int main()
{
    ParserDataT gData;
    InitParserData(&gData);
    generator_start(gData.code);

    FILE *fileInput = stdin;

    SetSourceFile(fileInput);

    int result = loadFuns(&gData);      // Načtení funkcí do globální tabulky symbolů
    {
        PRINT_ERR_CODE(result);
        DisposeParserData(&gData);
        return result;
    }

    int syn_result = parse(&gData);        //první průchod - syntaktická analýza, naplnění tabulek symbolů
    if (syn_result != 0)
    {
        PRINT_ERR_CODE(syn_result);
        DisposeParserData(&gData);
        return syn_result;
    }
    
    int sem_result = sem_anal(&gData);    //druhý průchod - sémantické akce
    if (sem_result != 0)
    {
        PRINT_ERR_CODE(sem_result);
        DisposeParserData(&gData);
        return sem_result;
    }

    generator_main_end(gData.code);
    FluidCreateOutput(gData.code, stdout);
    DisposeParserData(&gData);

    return 0;
}
