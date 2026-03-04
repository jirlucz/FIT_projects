#include"../scanner.c"
#include"../scanner.h"
#include "../fluid_string.c"
#include "../fluid_string.h"
#include <stdio.h>

int main (){

     SetSourceFile(stdin);    
    
    Token token;

       while (1)  
       {
        printf("_______________________________________________________________________\n");
        if (GetToken(&token) != LEX_OK)
        {
            printf("Vyskytla se chyba\n");
            return 1;
        }
        switch (token.type)
        {
        case WHOLENUMBER:
            printf("Cele cislo:%lld#\n", token.value);
            break;

        case DECIMALNUMBER:
            printf("Desetinne cislo:%f#\n", token.floatingValue);
            break;
        
        case STRINGVALUE:
            printf("String:%s#\n", token.fluidPayload->payload);
            break;
        case IDENTIFICATOR:
            printf("ID je:%s#\n", token.fluidPayload->payload);
            break;
        
        case IF:
            printf("IF\n");
            break;
        
        case NIL:
            printf("NIL\n");
            break;

        case ELSE:
            printf("ELSE\n");
            break;
        
        case FUNC:
            printf("FUNC\n");
            break;

        case LET:
            printf("LET\n");
            break;

        case WHILE:
            printf("WHILE\n");
            break;
        
        case INTKEY:
            printf("INTKEY\n");
            break;
        
        case RETURN:
            printf("RETURN\n");
            break;
        
        case DOUBLEKEY:
            printf("DOUBLEKEY\n");
            break;
        
        case STRINGKEY:
            printf("STRINGKEY\n");
            break;
        
        case EQ:
            printf("EQ\n");
            break;
        case EQEQ:
            printf("EQEQ\n");
            break;
        case NEQ:
            printf("NEQ\n");
            break;
        case LESSER:
            printf("LESSER\n");
            break;
        case LESSEREQ:
            printf("LESSEREQ\n");
            break;
        case GREATER:
            printf("GREATER\n");
            break;
        case GREATEREQ:
            printf("GREATEREQ\n");
            break;
        case OPBR:
            printf("OPBR\n");
            break;
        case CLBR:
            printf("CLBR\n");
            break;
        case OPCB:
            printf("OPCB\n");
            break;
        case CLCB:
            printf("CLCB\n");
            break;
        case MUL:
            printf("MUL\n");
            break;
        case DIV:
            printf("DIV\n");
            break;
        case EOFILE:
            printf("EOFILE\n");
            return 0;
            break;
        case COMA:
            printf("COMA\n");
            break;
        case DOUBLEDOT:
            printf("DOUBLEDOT\n");
            break;
        case UNDERSCORE:
            printf("UNDERSCORE\n");
            break;
        case LAMBDA:
            printf("LAMBDA\n");
            break;
        case QUESTIONMARK:
            printf("QUESTIONMARK\n");
            break;
        case DOUBLEQUESTIONMARK:
            printf("DOUBLEQUESTIONMARK\n");
            break;
        case EXCLAMATION:
            printf("EXCLAMATION");
            break;

        case PLUS:
            printf("PLUS\n");
            break;
        case MINUS:
            printf("MINUS\n");
            break;
        
        
        default:
            printf("DEAFAULT CHYBA\n");
            break;
        }
       }
       
    FlushToken(&token);        



    return 0;
}



