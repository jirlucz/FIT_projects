#include "scanner.h"
#include "error.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/**
 * @file scanner.c
 * 
 * @brief Implementation of scanner based on FSM
 * 
 * Project: IFJ 2023
 * 
 * @authors Jiri Kotek <xkotek09@stud.fit.vutbr.cz>

*/

FILE*sourceFile = NULL;


void SetSourceFile(FILE *file){
    if (file != NULL)
    {
        sourceFile = file;
    }
    
}



bool InitToken(Token *token){
    //Alocating memory for Fluids structure
    token->fluidPayload = (Fluids*) malloc (sizeof (Fluids));
    if (token->fluidPayload == NULL)
    {
       return false;
    }

    if ((FluidInit(token->fluidPayload)) == NULL)
    {
        return false;
    }
    
    token->value = 0;
    token->floatingValue = 0.0;
    token->type = START; //Default
    token->followingEOL = false;
    return true;   
}


bool FlushToken(Token *token){
    if (token == NULL)
    {
        return false;   //Nothing to do
    }
    if (FluidFlush(token->fluidPayload) == false)
    {
        return false;
    }
    
    free(token->fluidPayload);
    token->fluidPayload = NULL;
    return true;
}


bool AddCharToToken(Token *token,char c){
    if (token == NULL)
    {
        return false;   //Nothing to do
    }

     return (FluidAddChar((token->fluidPayload),c));
}


bool AddTypeToToken(Token *token,Token_Type type){
    if (token == NULL)
    {
        return false;
    }
    
    token->type = type;
    return true;
}


void Return2Begg (){
    if (sourceFile == NULL)
    {
        return;
    }
    
    fseek(sourceFile,0,SEEK_SET);
}


bool bingo (char *arr, int size){
    if (arr == NULL || size <= 0)
    {
        return false;
    }
    
    char cmp = '"';
    for (int i = 0; i < size; i++)
    {
        if (cmp != arr[i])
        {
            return false;
        }
    }
    return true;
}


int MultiLineOffset (int *resArr){

    if (sourceFile == NULL)
    {
        return INTERNAL_ERR;
    }

    //Save current file position
    long originalPosition = ftell(sourceFile);
    
    int offset = 0;
    int NoLine = 0;
    char arrChar [3];

    //Now we want to locate ending """ secvention
    //bingo function returns true if arrChar[0,1,2] contains "
      while (bingo(arrChar, 3) == false)
      {
        for (int i = 0; i < 3; i++)
        {
            arrChar[i] = getc(sourceFile);
                if (arrChar[i] == EOF)
                {
                    return LEX_ERR;
                }                
        }
        ungetc(arrChar[2], sourceFile);
        ungetc(arrChar[1], sourceFile);
        if (arrChar[0] == ' ')
        {
            offset++;
        }
        
        if (arrChar[0] == '\n')
        {
            NoLine++;
            offset = 0;
        }
    }

    //restore original position in a file
    fseek (sourceFile, originalPosition, SEEK_SET);
    resArr[0] = offset;
    resArr[1] = NoLine;
    return LEX_OK;
    
}


int HexNumToChar(const char *HexaString){
        if (HexaString == NULL)
        {
            return '0';
        }
        
    int decimalVal = strtol (HexaString, NULL, 16);

    return decimalVal;
}


int processNumber (Token *token){
    char inputChar;
    inputChar = getc(sourceFile);

    bool decimalPointLoad   = false;
    bool exponentLoaded     = false;
    bool sign               = false;
    bool decimalPointLValueLoad     = false;
    bool exponentValueLoaded        = false;

    while (inputChar != EOF)
    {
        
        //Loading WHOLE number
        if (isdigit(inputChar) != 0)
        {
            AddCharToToken(token, inputChar);

            if (decimalPointLoad == true && decimalPointLValueLoad == false)
            {
                decimalPointLValueLoad = true;
            }

            if (exponentLoaded == true && exponentValueLoaded == false)
            {
                exponentValueLoaded = true;
            }            
        }

        //Loading DECIMAL number
        else if (inputChar == '.')
        {
            AddCharToToken(token, inputChar);
            //Error decimal point was already loaded
            if (decimalPointLoad == true)
            {
                return LEX_ERR;
            }
            //Error exponent cant be loaded before decimal point
            if (exponentLoaded == true)
            {
                return LEX_ERR;
            }
            

            
            decimalPointLoad = true;
            inputChar = getc(sourceFile);
            //Checks for EOF and decimal point must be followed by a number
            if (inputChar == EOF || isdigit(inputChar) == 0)
            {
                return LEX_ERR;
            }
            decimalPointLValueLoad = true;
            AddCharToToken(token, inputChar);
        }

        //Loading DECIMAL number with exponent
        else if (inputChar == 'e' || inputChar == 'E')
        {
            //Exponent loaded multiple times
            if (exponentLoaded == true)
            {
                return LEX_ERR;
            }

            exponentLoaded = true;

            //E -> e convertion
            if (inputChar == 'E')
            {
                inputChar = inputChar+32;
            }

            AddCharToToken (token, inputChar);

                inputChar = getc(sourceFile);
                if (inputChar == '+' || inputChar == '-')
                {
                    sign = true;
                    AddCharToToken(token,inputChar);
                }
                else
                {
                    ungetc(inputChar, sourceFile);
                }
                
        }
        else if ((inputChar == '+' && exponentLoaded == true) || (inputChar == '-' && exponentLoaded == true))
        {
            if (sign == true)
            {
                return LEX_ERR;
            }
            else
            {
                sign = true;
                AddCharToToken(token,inputChar);
            }
            
        }
        else if ( isspace(inputChar) != 0 )
        {
            break;
        }
        else if (inputChar == '_' || isalpha(inputChar) != 0)
        {
            return LEX_ERR;
        }
        
        else
        {
            break;
        }
        inputChar = getc(sourceFile);
                   
    }

    ungetc(inputChar, sourceFile);
            if (decimalPointLoad == false && exponentLoaded == false)
            {
                token->value = atoi(token->fluidPayload->payload);
                AddTypeToToken(token, WHOLENUMBER);
                return LEX_OK;
            }
            else //if (decimalPointLoad == true || exponentLoaded != true)
            {
                char *ptr = NULL;
                token->floatingValue = strtod (token->fluidPayload->payload, &ptr);
                //We loaded 6e (without number after e)
                if (    (exponentValueLoaded == false && exponentLoaded == true) || (decimalPointLValueLoad == false && decimalPointLoad == true) )
                {
                    return LEX_ERR;
                }
                
                AddTypeToToken(token, DECIMALNUMBER);
                return LEX_OK;
            }

    return LEX_OK;  

}


bool FollowingEOL (){
    char inputChar = getc(sourceFile);

    while (inputChar == ' ')
    {
        inputChar = getc(sourceFile);
    }
    
    if (inputChar == '\n' || inputChar == EOF)
    {
        return true;
    }
    
    ungetc(inputChar, sourceFile);
    return false;
}


bool FollowingEOF (){
    char inputChar = getc(sourceFile);

    while (inputChar == ' ')
    {
        inputChar = getc(sourceFile);
    }
    
    if (inputChar == EOF)
    {
        return true;
    }
    

    ungetc(inputChar, sourceFile);
    return false;
}

int DeleteComments(){
    char inputChar = getc(sourceFile);

    //Delete all extra white spaces
    while (inputChar == ' ' && inputChar != EOF)
    {
        inputChar = getc(sourceFile);
    }

    if (inputChar == EOF)
        {   
            ungetc(inputChar, sourceFile);
            return 0;
        }
    

    char nextChar = getc(sourceFile);

    if (inputChar == '/' && nextChar == '/')
    {
        //Delete all chars
        while ((inputChar = getc(sourceFile)) != '\n' && inputChar != EOF)
        {
        }
        ungetc(inputChar, sourceFile);
        return 0;
    }
    else if (inputChar == '/' && nextChar == '*')
    {
        int No_BLOCK_COMMENTS = 1;
        //No_BLOCK_COMMENTS++;        //We increment number of block comments
        while (No_BLOCK_COMMENTS != 0)
        {
            inputChar = getc(sourceFile);
            if (inputChar == EOF)
            {
                return 1; //Error /*(EOF)
            }
            nextChar = getc(sourceFile);
            if (nextChar == EOF)
            {
                return 1; //Error /*(EOF)
            }
                                    
            if (inputChar == '/' && nextChar == '*')
            {
                No_BLOCK_COMMENTS++;
            }
            else if (inputChar == '*' && nextChar == '/')
            {
                No_BLOCK_COMMENTS--;

                if (No_BLOCK_COMMENTS == 0)
                {
                    return 0;
                }
                
            }
            ungetc(nextChar, sourceFile);
        }
    }
    ungetc(nextChar, sourceFile);
    ungetc(inputChar,sourceFile);
    return 0;
        
}

int SetTokenFollow (Token *token){
     if (DeleteComments() != 0)
    {
        return 1;
    }
    token->followingEOL=FollowingEOL();
    return 0;
}


int EscapeSecventionProcessing (Token *token){
    char inputChar;
    char nextchar = getc (sourceFile);
    if (nextchar == EOF || nextchar == '\n' )
    {
        return LEX_ERR;
    }
    // AddCharToToken(token, inputChar);
    switch (nextchar)
    {
        //Loaded (\")
        case '"':
            AddCharToToken(token, nextchar);
            break;
        //Loaded (\n)
        case 'n':
            AddCharToToken(token, '\n');
            break;
        //Loaded (\r)
        case 'r':
            AddCharToToken(token, '\r');
            break;
        //Loaded (/t)
        case 't':
            AddCharToToken(token, '\t');
            break;
        //Loaded (\\)
        case '\\':
            AddCharToToken(token, nextchar);
            break;
        //Loaded (\u) -> expected HAXA num \u{(exp)}
        case 'u':
            inputChar = getc (sourceFile);
            if (inputChar != '{')
            {
                return LEX_ERR;
            }
            int count2 = 0;
            nextchar = getc(sourceFile);
            ///////////////////////////////Loading 2 HEXADEC chars//////////////////////////////////////////////////////////
            //String where we store value inside \u{(exp)}
            char *HexString = (char*)malloc(sizeof(char) * 4);
            if (HexString == NULL)
            {
                return INTERNAL_ERR;
            }
            while ((count2 < 2) && (nextchar != EOF ) && (nextchar != '\n'))
            {
            // nextchat >= '0' && nextchar <= '9' || nextchar >= A && nextchar <= F) || nextchar >= a && nextchar <= f)
            if ((nextchar >= 48 && nextchar <= 57 ) || (nextchar >= 65 && nextchar <= 70) ||(nextchar >= 97 && nextchar <= 102))
            {
                //Small char change to Large char (a->A, b->B,...)
                if (nextchar >= 97 && nextchar <= 102)
                {
                nextchar = nextchar - 32;
                }
                HexString[count2] = nextchar;
            }

            else if (nextchar == '}')
            {
                break; //The end of loading HEXA num
            }

            else
            {
                return LEX_ERR; //error
            }

            nextchar = getc(sourceFile);
            count2++;
            }
            //Error, we didnt load a single char -> "\u{}"
            if (count2 == 0)
            {
                return LEX_ERR;
            }
            if (nextchar != '}')
            {
                return LEX_ERR;
            }
            

            //Hex value is stored in HexString, now we convert it into char
            int HexCharVal = HexNumToChar(HexString);
            free(HexString);
            AddCharToToken(token, HexCharVal);
            break;
        ///////////////////////////////The end of reading HEXA num/////////////////////////////
        //Loaded (\(012)) -> escape secvention
        default:
            //Loaded (\(exp)) -> error
                return LEX_ERR;
        }

        return 0;
}



int GetToken(Token *token){

    if (sourceFile == NULL)
    {
        //printf("sourceFIle je NULL\n");
        return LEX_ERR;
    }

    if (token->fluidPayload != NULL)
    {
        // FlushToken(token);
    }
    

    if (InitToken(token) == false)
    {
        //printf("Chyba Inicializace tokenu\n");
        return LEX_ERR;
    }
    

    char inputChar, nextchar;
        

    while(true)
    {
        inputChar = getc(sourceFile);
        //Check for empty file
        if (inputChar == EOF)
        {
            token->followingEOF = true;
            AddTypeToToken(token, EOFILE);
            break;
        }
        
            

            //First switch represents individual states of the finite automata
            switch (token->type)
            {

            case START:
                        //Second switch is responding to inputChar a creates tokens
                        switch (inputChar)
                            {
                            case '+':
                                if (AddTypeToToken(token, PLUS) == false)
                                {
                                    return INTERNAL_ERR;
                                }

                                if (SetTokenFollow(token) != 0)
                                {
                                    return LEX_ERR;
                                }                                
                                return LEX_OK;

                            case '-':
                                nextchar = getc(sourceFile);
                                if (nextchar == '>')
                                {
                                    if (AddTypeToToken(token, LAMBDA) == false){
                                        return INTERNAL_ERR;
                                    }
                                    if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    }                                
                                    return LEX_OK;
                                }
                                else
                                {
                                    ungetc(nextchar, sourceFile);
                                    AddTypeToToken(token, MINUS);
                                    if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                    return LEX_OK;
                                }

                            case '*':
                                if (AddTypeToToken(token, MUL) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case '/':
                                if (AddTypeToToken(token, DIV) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            case '=':
                                if (AddTypeToToken(token, EQ) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            case '!':
                                if (AddTypeToToken(token, EXCLAMATION) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            case ':':
                                if (AddTypeToToken(token, DOUBLEDOT) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case ',':
                                if (AddTypeToToken(token, COMA) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case '?':
                                if (AddTypeToToken(token, QUESTIONMARK) == false)
                                {
                                    return INTERNAL_ERR;
                                }
                                break;

                            case '<':
                                if (AddTypeToToken(token, LESSER) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            case '>':
                                if (AddTypeToToken(token, GREATER) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            case '{':
                                if (AddTypeToToken(token, OPCB) == false)
                                {
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case '}':
                                if (AddTypeToToken(token, CLCB) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case '(':
                                if (AddTypeToToken(token, OPBR) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case ')':
                                if (AddTypeToToken(token, CLBR) == false){
                                    return INTERNAL_ERR;
                                }
                                if (SetTokenFollow(token) != 0)
                                    {
                                        return LEX_ERR;
                                    } 
                                return LEX_OK;

                            case '"':
                                if (AddTypeToToken(token, STRINGVALUE) == false){
                                    return INTERNAL_ERR;
                                }
                                
                                break;
                            case '_':   //func f(_x : Int ) -> Int{} !!!

                                //We found UNDERSCORE
                                //Now we must check if it is UNDERSCORE or IDENTIFICATOR
                                nextchar = getc (sourceFile);
                                    //If inputChar is followed by whitespace, we found UNDERSCORE
                                    if (nextchar == ' ')
                                    {
                                        AddTypeToToken(token, UNDERSCORE);
                                        if (SetTokenFollow(token) != 0)
                                        {
                                            return LEX_ERR;
                                        } 
                                        return LEX_OK;
                                    }
                                ungetc(nextchar, sourceFile);

                                if (AddTypeToToken(token, _IDWOALFANUM) == false){
                                    return INTERNAL_ERR;
                                }
                                if (AddCharToToken(token, inputChar) == false){
                                    return INTERNAL_ERR;
                                }
                                break;

                            
                            default:
                                if (isdigit(inputChar) != 0)
                                {
                                    if (AddTypeToToken(token, WHOLENUMBER) == false){
                                        return INTERNAL_ERR;
                                    }
                                    
                                    ungetc(inputChar, sourceFile);
                                }
                                else if (isalpha(inputChar) != 0)
                                {
                                    if (AddTypeToToken(token, IDENTIFICATOR) == false){
                                        return INTERNAL_ERR;
                                    }
                                    
                                    ungetc(inputChar, sourceFile);
                                }
                                else if (inputChar == ' ' || inputChar == '\n')
                                {
                                    ungetc(inputChar, sourceFile);
                                    FollowingEOL(); //With function Following EOL we can also remove whitespaces
                                    //Delete all whitespaces and emplty lines
                                    break;
                                }
                                else if (inputChar == '\t')
                                {
                                    continue;
                                }
                                
                                
                                else
                                {
                                    //printf("Chyba lex analyzy, ve stavu START");
                                    return LEX_ERR;
                                }
                                
                                break;
                            }
                        break;
            
            case DIV:
                //Loaded "/" + (inputChar)
                // inputChar is already loaded!
                        //SINGLE LINE COMMENT
                        //Loaded "//"
                        if (inputChar == '/')
                        {
                            while ((inputChar = getc(sourceFile)) != '\n' && inputChar != EOF)
                            {
                            }
                            AddTypeToToken(token, START);
                            continue;                      
                        }
                        //BLOCK COMMENT
                        //Loaded "/*"
                        else if (inputChar == '*')
                        {
                            int No_BLOCK_COMMENTS = 1;
                            //No_BLOCK_COMMENTS++;        //We increment number of block comments
                            while (No_BLOCK_COMMENTS != 0)
                            {
                                inputChar = getc(sourceFile);
                                    if (inputChar == EOF)
                                    {
                                        return LEX_ERR; //Error /*(EOF)
                                    }
                                nextchar = getc(sourceFile);
                                    if (nextchar == EOF)
                                    {
                                        return LEX_ERR; //Error /*(EOF)
                                    }
                                    
                                if (inputChar == '/' && nextchar == '*')
                                {
                                    No_BLOCK_COMMENTS++;
                                }
                                else if (inputChar == '*' && nextchar == '/')
                                {
                                    No_BLOCK_COMMENTS--;
                                }
                                ungetc(nextchar, sourceFile);
                            }
                            inputChar = getc(sourceFile);
                            AddTypeToToken(token, START);
                            continue;                  //Replace with continue? At First we need to load new char
                        }
                        //DIV
                        //Loaded "/(exp)"
                        else
                        {
                            ungetc(inputChar, sourceFile);
                            AddTypeToToken(token, DIV);
                            if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                            return LEX_OK;
                        }                        
                        break;
                break;

            case EQ:
                //Loaded "=" + inputChar(Unkown)
                if (inputChar == '=')   //Loaded "=="
                {
                    AddTypeToToken(token, EQEQ);
                    if (SetTokenFollow(token) != 0)
                    {
                        return LEX_ERR;
                    } 
                    return LEX_OK;
                }
                else                    //Loaded "=(exp)"
                {
                    ungetc(inputChar, sourceFile);
                    if (SetTokenFollow(token) != 0)
                    {
                        return LEX_ERR;
                    } 
                    return LEX_OK;
                }
                
            case EXCLAMATION:
                //Loaded "!" + inputChar(Unkown)
                if (inputChar == '=')           //Loaded "!="
                {
                    AddTypeToToken(token, NEQ);
                    if (SetTokenFollow(token) != 0)
                    {
                        return LEX_ERR;
                    } 
                    return LEX_OK;
                }
                else                            //Loaded "!(exp)"
                {
                    ungetc (inputChar,sourceFile);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
                
            case QUESTIONMARK:
                //Loaded "?" + inputChar(Unkown)
                if (inputChar == '?')       //Loaded "??"
                {
                    AddTypeToToken(token, DOUBLEQUESTIONMARK);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
                else                        //Loaded "?(exp)"
                {
                    ungetc(inputChar, sourceFile);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }

            case LESSER:
                //Loaded "<" + inputChar(Unkown)
                if (inputChar == '=')           //Loaded "<="
                {
                    AddTypeToToken(token, LESSEREQ);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
                else                            //Loaded "<(exp)"
                {
                    ungetc(inputChar, sourceFile);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
            
            case GREATER:
                //Loaded ">" + inputChar(Unkown)
                if (inputChar == '=')           //Loaded ">="
                {
                    AddTypeToToken(token, GREATEREQ);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
                else                            //Loaded ">(exp)"
                {
                    ungetc(inputChar, sourceFile);
                    if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                    return LEX_OK;
                }
                
            case STRINGVALUE:
                //Loaded {"} + inputChar(Unkown)
                    if (inputChar == EOF)       //Loaded {"(EOF)} LEX ERROR, non-finite state
                    {
                        return LEX_ERR;
                    }
                nextchar = getc(sourceFile);
                    if (nextchar == EOF && inputChar == '"')       //Loaded {""(EOF)} -> Empty string is allowed
                    {
                        AddCharToToken(token, '\0');
                        ungetc(nextchar, sourceFile);
                        return LEX_OK;
                    }
                    else if (inputChar == '"' && nextchar == '"')   //Loaded {"""} -> Multilane string
                    {
                        //Now we must find nearest EOL
                        inputChar = getc(sourceFile);
                        while (inputChar != '\n')
                        {
                            if (inputChar == EOF || inputChar != ' ' )
                            {
                                return LEX_ERR;
                            }
                            inputChar = getc(sourceFile);
                        }
                        //We have to find closing (""") secvention to find out offset from 1st character on a line
                        /* SWIFT DOCUMENTATION:
                            A multiline string can be indented to match the surrounding code. 
                            The whitespace before the closing quotation marks (""") tells Swift what whitespace to ignore before all of the other lines. 
                            However, if you write whitespace at the beginning of a line in addition to what’s before the closing quotation marks, that whitespace is included.
                        */
                       /*
                        resArr detail:
                            resArr[0] = offset;
                            resArr[1] = NoLine;
                       */
                        int offsetANDnoLineArr[2];
                        int retErr = MultiLineOffset(offsetANDnoLineArr);
                        if ( retErr != LEX_OK)
                        {
                            return retErr; //Should return LEX_OK or LEX_ERR
                        }

                        int emptyLine;
                        //String is already checked for EOF
                        //1st "for" is counting lines
                        for (int x = 0; x < offsetANDnoLineArr[1]; x++)
                        {
                            emptyLine = 0;
                            //2nd "for" is counting offset/number of whitespaces to ignore
                            for (int y = 0; y < offsetANDnoLineArr[0]; y++)
                            {
                                emptyLine = y;
                                inputChar = getc(sourceFile);
                                //Checks if we reach EOL
                                if (inputChar == '\n')
                                {
                                    break;
                                }
                                else if (inputChar != ' ')
                                {
                                    return LEX_ERR;
                                }
                                
                                
                            }

                            /* We found: (X means whitespace)
                                1|let x = """\n
                                2|XXHello\n
                                3|\n
                                4|XX"""\n

                                3rd line doesnt match 4th format but bcs of \n is correct
                            */
                            if (emptyLine < (offsetANDnoLineArr[0]-1))
                            {
                                AddCharToToken(token, inputChar);
                                continue;
                            }

                            if (emptyLine == 0 && inputChar == '\n' && (offsetANDnoLineArr[0] !=0))
                            {
                                AddCharToToken(token, inputChar);
                                continue;
                            }
                            
                            

                            inputChar = getc(sourceFile);
                            while (inputChar != '\n')
                            {
                                //Loaded \(exp)
                                if (inputChar == '\\')
                                {
                                    if (EscapeSecventionProcessing(token) == LEX_ERR)
                                    {
                                        return LEX_ERR;
                                    }
                                }
                                else
                                {
                                    AddCharToToken(token, inputChar);
                                }
                                inputChar = getc(sourceFile);
                            }
                            
                            if (x != (offsetANDnoLineArr[1]-1) )
                            {
                                AddCharToToken(token, inputChar);;
                            }
                            
                        }
                        //Remove last three """ (ending multi line string) + \n
                            inputChar =  getc(sourceFile);
                            while (inputChar != '\n' && inputChar != EOF)
                            {
                                if (inputChar == ' ' || inputChar == '"')
                                {
                                    inputChar =  getc(sourceFile);
                                }
                                else
                                {
                                    return LEX_ERR;
                                }
                                
                               
                            }
                            
                        //Sending emptt token string???
                        if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                        return LEX_OK;
                    }
                    else
                    {
                        ungetc(nextchar, sourceFile);               //Put nextchar back
                    }
                
                //Loading standart string into payload
                //EOF cant be loaded, bcs of nextchar check
                while (inputChar != '"')
                {
                    if (inputChar == EOF || inputChar == '\n' || inputChar < 32) //"(exp)" must be placed on a single line, must be larger than ASCII 31
                    {
                        return LEX_ERR;              //Loaded {"(exp)(EOF)} LEX ERROR, non-finite state
                    }

                    /**********************************************Start of reading escape secvention**************************************************************************/
                    if (inputChar == '\\')  //Loaded "\"
                    {
                        if (EscapeSecventionProcessing(token) == LEX_ERR)
                        {
                            return LEX_ERR;
                        }
                    }
                    /**********************************************The end of reading escape secvention**************************************************************************/
                    else
                    {
                        AddCharToToken(token, inputChar);
                    }
                    
                    inputChar = getc (sourceFile);
                }
                if (SetTokenFollow(token) != 0)
                            {
                                return LEX_ERR;
                            } 
                return LEX_OK;                       //Loaded "(exp)" ->  Read of string was successful

            case _IDWOALFANUM:

                //Must continue with AlfaNum char
                    if (isalnum(inputChar) == 0 && inputChar != '_')
                    {
                        return LEX_ERR;
                    }
                    else
                    {
                        ungetc (inputChar, sourceFile);
                        AddTypeToToken(token, IDENTIFICATOR);
                    }
                break;
                
                
            case WHOLENUMBER:
                ungetc(inputChar, sourceFile);
                int err = processNumber(token);
                if (SetTokenFollow(token) != 0)
                {
                    return LEX_ERR;
                } 
                return (err);
            
            case IDENTIFICATOR:
                AddCharToToken(token, inputChar);
                inputChar = getc(sourceFile);
                while (isalnum(inputChar) != 0 || inputChar == '_')
                {
                    AddCharToToken(token, inputChar);
                    inputChar = getc(sourceFile);
                }
                ungetc(inputChar, sourceFile);

                char *keyword[] = {"if", "else", "func", "let", "var", "while", "nil", "return", "Int", "Double", "String"};
                int Nokeywords = 11;

                for (int i = 0; i < Nokeywords; i++)
                {
                    if (strcmp(token->fluidPayload->payload, keyword[i]) == 0)
                    {
                        switch (i)
                        {
                        case 0:
                            AddTypeToToken(token, IF );
                            break;
                        case 1:
                            AddTypeToToken(token, ELSE );
                            break;
                        case 2:
                            AddTypeToToken(token, FUNC );
                            break;
                        case 3:
                            AddTypeToToken(token, LET );
                            break;
                        case 4:
                            AddTypeToToken(token, VAR );
                            break;
                        case 5:
                            AddTypeToToken(token, WHILE );
                            break;
                        case 6:
                            AddTypeToToken(token, NIL );
                            break;
                        case 7:
                            AddTypeToToken(token, RETURN );
                            break;
                        case 8:
                            AddTypeToToken(token, INTKEY );
                            break;
                        case 9:
                            AddTypeToToken(token, DOUBLEKEY );
                            break;
                        case 10:
                            AddTypeToToken(token, STRINGKEY );
                            break;
                        
                        default:
                            return INTERNAL_ERR;
                            break;
                        }
                    }
                    
                }
                if (SetTokenFollow(token) != 0)
                {
                    return LEX_ERR;
                } 
                return LEX_OK;

                    

            default:
                break;
            }
        
            
    }
    if (SetTokenFollow(token) != 0)
            {
                    return LEX_ERR;
            } 
    return LEX_OK;
}
