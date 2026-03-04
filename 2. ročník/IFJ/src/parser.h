/**
 * @file parser.h
 *
 * @brief
 *
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */

#ifndef PARSER_H__
#define PARSER_H__

#include "stackChar.h"
#include "scanner.h"
#include "symtable.h"
#include "error.h"
//#include "expr.h"
#include "stack.h"
#include "fluid_string.h"

#define MAX_TSYM 1000
#define MAX_PARAMS 1000

typedef struct
{
    Token *token;
    bool pushBack;                   // Je token pushnutý zpět?
    bool argExpr;                    // Je výraz argument funkce?
    bool followingMlExprEOL;         // Je EOL za multiline výrazem?
    bool inFunc;                     // Je kod ve funkci?
    
    Stack SymStack;
    avl_node_t **LocalST;            // Pole všech tabulek symbolů, globální tabulka symbolů je na indexu 0
    avl_Data symTableData;
    int current_scope[MAX_TSYM];     // Pomocné pole pro uchovávání indexů rozsahů
    int top;                         // Index vrcholu pole
    int max_scope;                   // Celkový počet rozsahů
    int number_params;               // Počet parametrů funkce
    Data_type result_type;           // Vysledny typ vyrazu
    avl_Data *IfLetVar;
    // Flagy:
    bool Return_type;                // Funkce má návrotový typ
    bool type_assigned;              // Proměnná už má přiřazený datový typ
    bool assign;                     // Prirazuje se k promenne
    Fluids*code;                     // Data generovaneho kodu
    int indexif;                     // Index labelu konstrukt
    bool writef;
    bool CanConvertToDouble;
} ParserDataT;

void InitParserData(ParserDataT *);
void DisposeParserData(ParserDataT *);
int loadFuns(ParserDataT *);

int parse(ParserDataT *);

int start(ParserDataT *);
int stat(ParserDataT *);
int funVar(ParserDataT *);
int args(ParserDataT *);
int args2(ParserDataT *);
int statList(ParserDataT *);
int pl(ParserDataT *);
int pl2(ParserDataT *);
int term(ParserDataT *);
int funBody(ParserDataT *, char);
int assign(ParserDataT *);
int ifH(ParserDataT *);
int varDec(ParserDataT *);
int dataType(ParserDataT *);
int funType(ParserDataT *);
int qMark(ParserDataT *);
int value(ParserDataT *);
int pValue(ParserDataT *);
int expr(ParserDataT *, Token[], int);

int add_params(ParserDataT *);
avl_Data *search_symbol(ParserDataT *gData, char* key);
int sem_anal(ParserDataT *);

#endif
