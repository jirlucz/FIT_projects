#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"


ParserDataT *globalData;

#define START_TEST do{                                  \
    ParserDataT _globalData;                            \
    globalData = &_globalData;             \
    Token _token;                                        \
    globalData->token = &_token;                         \
    globalData->pushBack = false;                       \
    }while(0);

void loadTokens(Token *dest, int *eols, Token_Type *types, int cnt)
{
    for (int i = 0; i < cnt; ++i)
    {
        Token newTok;
        newTok.type = types[i];
        newTok.followingEOL = eols[i];
    
        dest[i] = newTok;
    }

    SetTokenArray(dest, cnt);
}

void test01()
{
    printf("test01 # LET, IDENTIFICATOR, DOUBLEDOT, INTKEY, EOFILE\n");

    START_TEST;

    const int cnt = 5;
    Token tokens[5];
    int followingEol[5] = {0, 0, 0, 1, 0};
    Token_Type types[5] = {LET, IDENTIFICATOR, DOUBLEDOT, INTKEY, EOFILE};

    loadTokens(tokens, followingEol, types, cnt);

    int result = parse(globalData);
    printf("RESULT: %d\n\n", result);
}

void test02()
{
    printf("test01 # LET, IDENTIFICATOR, EQ, IDENTIFICATOR, OPBR, CLBR, EOL, EOFILE\n");

    START_TEST;

    const int cnt = 7;
    Token tokens[7];
    int followingEol[7] = {0, 0, 0, 0, 0, 1, 0};
    Token_Type types[7] = {LET, IDENTIFICATOR, EQ, IDENTIFICATOR, OPBR, CLBR, EOFILE};

    loadTokens(tokens, followingEol, types, cnt);

    int result = parse(globalData);
    printf("RESULT: %d\n\n", result);
}

int main()
{
    printf("PARSER TESTS\n");
    test01();
    test02();
}
