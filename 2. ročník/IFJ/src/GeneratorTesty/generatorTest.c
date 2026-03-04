#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include "../generator.h"
#include "../scanner.h"
#include "../scanner.c"

bool Test_Complete_InBuilt(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Main(){
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_header(code);
    generator_main_start(code);
    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_StrLen(){
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    FluidInsertString(code,"\nDEFVAR GF@_test_string");
    FluidInsertString(code,"\nDEFVAR GF@_output");
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcde");
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $strlen");
    FluidInsertString(code,"\nWRITE GF@_return");
    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Read(){
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    FluidInsertString(code,"\nDEFVAR GF@_test_string");
    FluidInsertString(code,"\nDEFVAR GF@_output");
    FluidInsertString(code,"\nCREATEFRAME");

    FluidInsertString(code,"\nCALL $readString");
    FluidInsertString(code,"\nWRITE GF@_return");

    FluidInsertString(code,"\nCALL $readInt");
    FluidInsertString(code,"\nWRITE GF@_return");

    FluidInsertString(code,"\nCALL $readDouble");
    FluidInsertString(code,"\nWRITE GF@_return");

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Ord(){
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    FluidInsertString(code,"\nDEFVAR GF@_test_string");
    FluidInsertString(code,"\nDEFVAR GF@_output");
    // Non empty string
    FluidInsertString(code,"\nMOVE GF@_test_string string@ABcde");
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $ord");
    FluidInsertString(code,"\nWRITE GF@_return");
    // Empty string
    FluidInsertString(code,"\nMOVE GF@_test_string string@");
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $ord");
    FluidInsertString(code,"\nWRITE GF@_return");

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Chr(){

    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    FluidInsertString(code,"\nDEFVAR GF@_test_int");
    FluidInsertString(code,"\nMOVE GF@_test_int int@65");
    FluidInsertString(code,"\nPUSHS GF@_test_int");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $chr");
    FluidInsertString(code,"\nWRITE GF@_return");
    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Print(){
  
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);

    FluidInsertString(code,"\nMOVE GF@_exp int@65");
    generator_print(code);

    FluidInsertString(code,"\nMOVE GF@_exp string@abc");
    generator_print(code);

    FluidInsertString(code,"\nMOVE GF@_exp float@0x1.26666p+5");
    generator_print(code);

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Conversions(){
 
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    // Int 2 double
    FluidInsertString(code,"\nDEFVAR GF@_test_int");
    FluidInsertString(code,"\nMOVE GF@_test_int int@65");
    FluidInsertString(code,"\nPUSHS GF@_test_int");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $int2double");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");
    // Double2int
    FluidInsertString(code,"\nDEFVAR GF@_test_double");
    FluidInsertString(code,"\nMOVE GF@_test_double float@0x1.26666p+5");
    FluidInsertString(code,"\nPUSHS GF@_test_double");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $double2int");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Substring(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);
    // Init vars
    FluidInsertString(code,"\nDEFVAR GF@_test_string");
    FluidInsertString(code,"\nDEFVAR GF@_test_i");
    FluidInsertString(code,"\nDEFVAR GF@_test_j");

    //TEST 1 - Correct input
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@1");
    FluidInsertString(code,"\nMOVE GF@_test_j int@3");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 2 - InCorrect input - i > j
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@3");
    FluidInsertString(code,"\nMOVE GF@_test_j int@1");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 3 - InCorrect input i<0
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@-1");
    FluidInsertString(code,"\nMOVE GF@_test_j int@3");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 4 - InCorrect input j<0
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@1");
    FluidInsertString(code,"\nMOVE GF@_test_j int@-3");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 5 - InCorrect input i > len
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@15");
    FluidInsertString(code,"\nMOVE GF@_test_j int@3");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 6 - InCorrect input i = len
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abc");
    FluidInsertString(code,"\nMOVE GF@_test_i int@3");
    FluidInsertString(code,"\nMOVE GF@_test_j int@3");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    //TEST 7 - InCorrect input j>len
    // Fill vars
    FluidInsertString(code,"\nMOVE GF@_test_string string@abcdedfg");
    FluidInsertString(code,"\nMOVE GF@_test_i int@1");
    FluidInsertString(code,"\nMOVE GF@_test_j int@30");
    // Push vars
    FluidInsertString(code,"\nPUSHS GF@_test_string");
    FluidInsertString(code,"\nPUSHS GF@_test_i");
    FluidInsertString(code,"\nPUSHS GF@_test_j");
    //Call func
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nCALL $substr");
    FluidInsertString(code,"\nWRITE string@Vysledek:");
    FluidInsertString(code,"\nWRITE GF@_return");

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Structure_If(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    int cnt = 1;
    FluidInit(code);
    generator_start(code);

    FluidInsertString(code,"#EXPRESSIONS\n");
    FluidInsertString(code,"EQ GF@_exp int@1 int@0\n");
    generator_if_header(code);
    generator_if_jump_to_else(code,cnt);
    FluidInsertString(code,"#BODY TRUE\n");
    FluidInsertString(code,"WRITE string@true\n");
    generator_if_jump_to_skip(code,cnt);
    generator_if_label_else(code,cnt);
    FluidInsertString(code,"#BODY FALSE\n");
    FluidInsertString(code,"WRITE string@false\n");
    generator_if_label_skip(code,cnt);

    cnt++;
    FluidInsertString(code,"#EXPRESSIONS\n");
    FluidInsertString(code,"EQ GF@_exp int@1 int@1\n");
    generator_if_header(code);
    generator_if_jump_to_else(code,cnt);
    FluidInsertString(code,"#BODY TRUE\n");
    FluidInsertString(code,"WRITE string@true\n");
    generator_if_jump_to_skip(code,cnt);
    generator_if_label_else(code,cnt);
    FluidInsertString(code,"#BODY FALSE\n");
    FluidInsertString(code,"WRITE string@false\n");
    generator_if_label_skip(code,cnt);

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Structure_While(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    int cnt = 1;
    FluidInit(code);
    generator_start(code);

    // While co vykona 5 tocek
    FluidInsertString(code,"MOVE GF@_exp int@5\n");
    generator_while_header(code);
    generator_while_start(code,cnt); //WHILE ()
    FluidInsertString(code,"#EXPRESSIONS\n"); 
    FluidInsertString(code,"GT GF@_eif_while_result GF@_exp int@1\n");  // {
    generator_while_jump_to_skip(code,cnt); // Cond
    FluidInsertString(code,"SUB GF@_exp GF@_exp int@1\n"); 
    FluidInsertString(code,"#BODY TRUE\n");    
    FluidInsertString(code,"WRITE GF@_exp\n");      // BODY
    generator_while_jump_to_start(code,cnt); 
    generator_while_label_skip(code,cnt);
    
    // }

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Eval_Expression_On_Stack(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code);
    generator_start(code);

    Token*token = malloc(sizeof(Token));
    InitToken(token);
    Fluids*codeT = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(codeT);
    Fluids*code2 = (Fluids*) malloc (sizeof (Fluids));
    FluidInit(code2);
    token->fluidPayload = codeT;


    FluidInsertString(token->fluidPayload,"XDD");
    token->type = STRINGVALUE;
    generator_push_something_to_stack(code,token);

    token->fluidPayload = code2;
    FluidInsertString(token->fluidPayload,"retezec s lomitkem \\ a\n novym#radkem");
    token->type = STRINGVALUE;
    generator_push_something_to_stack(code,token);

    generator_concat_on_stack(code);

    generator_save_exp(code);
    generator_print(code);


    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}

bool Test_Compare_type(){
    
    Fluids*code = (Fluids*) malloc (sizeof (Fluids));
    PrecSym sym = 10;
    FluidInit(code);
    generator_start(code);

    FluidInsertString(code,"WRITE string@---\n");
    FluidInsertString(code,"PUSHS string@abdsd5\n");
    FluidInsertString(code,"PUSHS string@41616\n");

    generator_expr(code,sym);
    generator_save_exp(code);
    generator_print(code);

    for( sym = 10 ; sym < 20;sym++){
    if(sym == 13) continue;
    FluidInsertString(code,"WRITE string@---\n");
    FluidInsertString(code,"PUSHS int@5\n");
    FluidInsertString(code,"PUSHS int@6\n");

    generator_expr(code,sym);
    generator_save_exp(code);
    generator_print(code);

    FluidInsertString(code,"WRITE string@---\n");
    FluidInsertString(code,"PUSHS float@0x1.26666p+5\n");
    FluidInsertString(code,"PUSHS float@0x1.26666p+8\n");

    generator_expr(code,sym);
    generator_save_exp(code);
    generator_print(code);

    FluidInsertString(code,"WRITE string@---\n");
    FluidInsertString(code,"PUSHS int@6\n");
    FluidInsertString(code,"PUSHS float@0x1.26666p+8\n");

    generator_expr(code,sym);
    generator_save_exp(code);
    generator_print(code);

    FluidInsertString(code,"WRITE string@---\n");
    FluidInsertString(code,"PUSHS float@0x1.26666p+8\n");
    FluidInsertString(code,"PUSHS int@6\n");

    generator_expr(code,sym);
    generator_save_exp(code);
    generator_print(code);
    }

    generator_main_end(code);
    FluidCreateOutput(code);
    free(code);
    return true;
}
int main(int argc, char *argv[])
{
    printf("----------TEST GENERATORU-------- %d %s\n",argc,argv[0]);

    //Test_Complete_InBuilt();
    //Test_Main();
    //Test_StrLen();
    //Test_Ord();
    //Test_Chr();
    //Test_Conversions();
    //Test_Substring();
    //Test_Read();
    //Test_Print();
    //Test_Structure_If();
    //Test_Structure_While();
    //Test_Compare_type();
    Test_Eval_Expression_On_Stack();
}