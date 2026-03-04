 /**
 * Project: IFJ23.
 *
 * @brief Code generator implementation.
 *        Some fucntions are multiplied for better navigation through code and possibility to dynamicaly change it
 *
 * @author Dominik Honza xhonza04
 */

#include "generator.h"
#include <stdbool.h>
#include <stdio.h>
#include "fluid_string.c"
#include "expr.h"

void generator_compare_types(Fluids*code){

     FluidInsertString(code,"\n# Compare types - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $comparetypes");

     FluidInsertString(code,"\nMOVE GF@_env_op3 GF@_exp");

     FluidInsertString(code,"\nPOPS GF@_env_op2"); // 
     FluidInsertString(code,"\nPOPS GF@_env_op1");

     FluidInsertString(code,"\nTYPE GF@_env_type1 GF@_env_op1");
     FluidInsertString(code,"\nTYPE GF@_env_type2 GF@_env_op2");

     FluidInsertString(code,"\nPUSHS GF@_env_op1");
     FluidInsertString(code,"\nPUSHS GF@_env_op2");

     // Prvni je string (Ten na top)
     FluidInsertString(code,"\nJUMPIFNEQ $skip_con GF@_env_op3 bool@true");
     FluidInsertString(code,"\nJUMPIFEQ $compare_concat GF@_env_type1 string@string");
     FluidInsertString(code,"\nLABEL $skip_con");

     // Rovnaji se
     FluidInsertString(code,"\nMOVE GF@_return int@0");
     FluidInsertString(code,"\nEQ GF@_exp GF@_env_type1 GF@_env_type2");
     FluidInsertString(code,"\nJUMPIFEQ $adds_test GF@_exp bool@true");
     
     // Prvni je float (Ten na top)
     FluidInsertString(code,"\nMOVE GF@_return int@1");
     FluidInsertString(code,"\nEQ GF@_exp GF@_env_type1 string@float");
     FluidInsertString(code,"\nJUMPIFEQ $convert_first_to_double GF@_exp bool@true");

      // Druhy je float (Top -1 )
     FluidInsertString(code,"\nMOVE GF@_return int@2");
     FluidInsertString(code,"\nEQ GF@_exp GF@_env_type2 string@float");
     FluidInsertString(code,"\nJUMPIFEQ $convert_second_to_double GF@_exp bool@true");

     FluidInsertString(code,"\nLABEL $convert_first_to_double");
     FluidInsertString(code,"\nINT2FLOATS");
     FluidInsertString(code,"\nJUMP $adds_test");

     FluidInsertString(code,"\nLABEL $convert_second_to_double");
     FluidInsertString(code,"\nPOPS GF@_env_op2");
     FluidInsertString(code,"\nINT2FLOATS");
     FluidInsertString(code,"\nPUSHS GF@_env_op2");
     FluidInsertString(code,"\nJUMP $adds_test");

     FluidInsertString(code,"\nLABEL $compare_concat");
     FluidInsertString(code,"\nMOVE GF@_return int@4\n");
     generator_concat_on_stack(code);
     FluidInsertString(code,"\nJUMP $skip_compare");

     FluidInsertString(code,"\nLABEL $adds_test");
     FluidInsertString(code,"\nJUMPIFNEQ $skip_compare GF@_env_op3 bool@true");
     FluidInsertString(code,"\nADDS");

     // Pridej jump zavolej danou korekci typu a return
     // Pro string budes comaprovat uz o funkci driv 
     FluidInsertString(code,"\nLABEL $skip_compare");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Compare types- FUNCTION"); // DEBUG
}

void generator_read_string(Fluids*code){

     FluidInsertString(code,"\n# Read string - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $readString");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nWRITE string@\\032");
     FluidInsertString(code,"\nREAD GF@_return string");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Read string- FUNCTION"); // DEBUG
}

void generator_read_int(Fluids*code){

     FluidInsertString(code,"\n# Read int - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $readInt");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nWRITE string@\\032");
     FluidInsertString(code,"\nREAD GF@_exp int");
     FluidInsertString(code,"\nMOVE GF@_return int@0");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Read int- FUNCTION"); // DEBUG
}

void generator_read_double(Fluids*code){

     FluidInsertString(code,"\n# Read double - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $readDouble");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nWRITE string@\\032");
     FluidInsertString(code,"\nREAD GF@_return float");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Read double- FUNCTION"); // DEBUG
}

void generator_length(Fluids*code){ // DONE

     FluidInsertString(code,"\n# STRLEN - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $length");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nDEFVAR LF@_first_arg");
     FluidInsertString(code,"\nPOPS LF@_first_arg");
     FluidInsertString(code,"\nDEFVAR LF@_length");
     FluidInsertString(code,"\nSTRLEN LF@_length LF@_first_arg");
     FluidInsertString(code,"\nMOVE GF@_return LF@_length");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END STRLEN - FUNCTION"); // DEBUG
}

void generator_Int2Double(Fluids*code){

     FluidInsertString(code,"\n# Int2Double - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $Int2Double");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nDEFVAR LF@_param");
     FluidInsertString(code,"\nPOPS LF@_param");
     FluidInsertString(code,"\nINT2FLOAT GF@_return LF@_param");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END INT2DOUBLE- FUNCTION"); // DEBUG
}


void generator_Double2Int(Fluids*code){
     FluidInsertString(code,"\n# Double2Int - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $Double2Int");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nDEFVAR LF@_param");
     FluidInsertString(code,"\nPOPS LF@_param");
     FluidInsertString(code,"\nFLOAT2INT GF@_return LF@_param");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Double2Int - FUNCTION"); // DEBUG
}

void generator_Substring(Fluids*code){

     FluidInsertString(code,"\n# substring - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $substring");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nMOVE GF@_return nil@nil");
     FluidInsertString(code,"\nDEFVAR LF@_expr");
     FluidInsertString(code,"\nDEFVAR LF@_len");

     //DDEF OF PARAMS
     FluidInsertString(code,"\nDEFVAR LF@_param_string");
     FluidInsertString(code,"\nDEFVAR LF@_param_i");
     FluidInsertString(code,"\nDEFVAR LF@_param_j");
     //POP to params
     FluidInsertString(code,"\nPOPS LF@_param_j");
     FluidInsertString(code,"\nPOPS LF@_param_i");
     FluidInsertString(code,"\nPOPS LF@_param_string");
     // INVALID PARAMS
     
     //i<0
     FluidInsertString(code,"\nLT LF@_expr LF@_param_i int@0");
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_expr bool@true");

     //j<0
     FluidInsertString(code,"\nLT LF@_expr LF@_param_j int@0");
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_expr bool@true");

     //i>j
     FluidInsertString(code,"\nGT LF@_expr LF@_param_i LF@_param_j");
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_expr bool@true");

     FluidInsertString(code,"\nSTRLEN LF@_len LF@_param_string");
     //i>=length
     FluidInsertString(code,"\nGT LF@_expr LF@_param_i LF@_len");
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_expr bool@true"); // i > len
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_param_i LF@_len"); // i = len

     //j>length
     FluidInsertString(code,"\nGT LF@_expr LF@_param_j LF@_len");
     FluidInsertString(code,"\nJUMPIFEQ $skipsubstr LF@_expr bool@true");

     // BODY
     FluidInsertString(code,"\nDEFVAR LF@_char");
     FluidInsertString(code,"\nMOVE GF@_return string@");

     FluidInsertString(code,"\nLABEL $do");
     FluidInsertString(code,"\nGETCHAR LF@_char LF@_param_string LF@_param_i");
     FluidInsertString(code,"\nCONCAT GF@_return GF@_return LF@_char");
     FluidInsertString(code,"\nADD LF@_param_i LF@_param_i int@1");
     FluidInsertString(code,"\nJUMPIFNEQ $do LF@_param_i LF@_param_j");

     //Skip
     FluidInsertString(code,"\nLABEL $skipsubstr");
     //CLEAN UP
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Substtring - FUNCTION"); // DEBUG
}

void generator_ord(Fluids*code){

     FluidInsertString(code,"\n# Ord - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $ord");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nMOVE GF@_return int@0");
     FluidInsertString(code,"\nDEFVAR LF@_param");
     FluidInsertString(code,"\nPOPS LF@_param");
     FluidInsertString(code,"\nSTRLEN GF@_return LF@_param");
     FluidInsertString(code,"\nJUMPIFEQ $skipORD GF@_return int@0");
     FluidInsertString(code,"\nSTRI2INT GF@_return LF@_param int@0");
     FluidInsertString(code,"\nLABEL $skipORD");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Ord - FUNCTION"); // DEBUG
}

void generator_chr(Fluids*code){

     FluidInsertString(code,"\n# Char - FUNCTION"); // DEBUG
     FluidInsertString(code,"\nLABEL $chr");
     FluidInsertString(code,"\nPUSHFRAME");
     FluidInsertString(code,"\nDEFVAR LF@_param");
     FluidInsertString(code,"\nPOPS LF@_param");
     FluidInsertString(code,"\nINT2CHAR GF@_return LF@_param");
     FluidInsertString(code,"\nPOPFRAME");
     FluidInsertString(code,"\nRETURN");
     FluidInsertString(code,"\n# END Char - FUNCTION"); // DEBUG
}


void generator_header(Fluids*code){

    FluidInsertString(code,".IFJcode23");

    FluidInsertString(code,"\nDEFVAR GF@_env_op1");
    FluidInsertString(code,"\nDEFVAR GF@_env_op2");
    FluidInsertString(code,"\nDEFVAR GF@_env_op3");

    FluidInsertString(code,"\nDEFVAR GF@_env_type1");
    FluidInsertString(code,"\nDEFVAR GF@_env_type2");
    
    FluidInsertString(code,"\nDEFVAR GF@_exp");
    FluidInsertString(code,"\nDEFVAR GF@_return");
    FluidInsertString(code,"\nDEFVAR GF@_eif_while_result");
    FluidInsertString(code,"\nJUMP $$main");
}

// Starting point of generation
void generator_start(Fluids*code){
   
    // Header
    generator_header(code);

    // In built functions
    generator_compare_types(code);
    generator_read_string(code);
    generator_read_int(code);
    generator_read_double(code);
    generator_length(code);
    generator_Double2Int(code);
    generator_Int2Double(code);
    generator_Substring(code);
    generator_ord(code);
    generator_chr(code);
    generator_main_start(code);
}

// Prints generated code to file
void generator_print_to_file(Fluids*code,FILE *file){
    fputs(code->payload,file);
}

/*
* Generates main scope 
*/
void generator_main_start(Fluids*code){

    FluidInsertString(code,"\n#MAIN START");
    FluidInsertString(code,"\nLABEL $$main");
    FluidInsertString(code,"\nCREATEFRAME");
    FluidInsertString(code,"\nPUSHFRAME\n");

}

/*
* Generates end of main scope
*/
void generator_main_end(Fluids*code){

    FluidInsertString(code,"#MAIN END\n");
    FluidInsertString(code,"POPFRAME\n");
    FluidInsertString(code,"CLEARS\n");
}

/*
* Generates call of label
*/
void generator_func_call(Fluids*code,char*id){
     FluidInsertString(code,"CREATEFRAME\n");
     FluidInsertString(code,"CALL $");FluidInsertString(code,id);FluidInsertString(code,"\n");   
}

/*
* Generates prep for passing args to function
*/
void generator_pass_arguments_to_func(Fluids*code,char* id){
      FluidInsertString(code,"PUSHS LF@_");FluidInsertString(code,id);FluidInsertString(code,"\n"); 
}

/*
* Generates header with name fro func
*/
void generator_func_header(Fluids*code,char* id){
     FluidInsertString(code,"JUMP $_SKIP_");FluidInsertString(code,id);FluidInsertString(code,"\n");
     FluidInsertString(code,"LABEL $");FluidInsertString(code,id);FluidInsertString(code,"\n");
     FluidInsertString(code,"PUSHFRAME\n");
}

/*
* Generates parameter with name for func
*/
void generator_func_params(Fluids*code,char* param_id){
     FluidInsertString(code,"DEFVAR LF@_");FluidInsertString(code,param_id);FluidInsertString(code,"\n");
}

/*
* Generates parameter pops
*/
void generator_func_pops(Fluids*code,char* param_id){
     FluidInsertString(code,"POPS LF@_");FluidInsertString(code,param_id);FluidInsertString(code,"\n");
}

// /* 
// * Generates return type
// */
void generator_func_return_type(Fluids*code,Token_Type*type){ // PARAM JE TYP TOKENU ()

     FluidInsertString(code,"MOVE GF@_return ");

     if(*type == WHOLENUMBER){ // Int
          FluidInsertString(code,"int@0");
     }
     
     if(*type == DECIMALNUMBER){ //Float
          FluidInsertString(code,"float@0x0.00000p+0");
     }

     if(*type == STRINGVALUE) { // String
          FluidInsertString(code,"string@");
     }
     
     FluidInsertString(code,"\n");
}

/*
* Generates end of func
*/
void generator_func_end(Fluids*code,char*id){
     FluidInsertString(code,"MOVE GF@_return");FluidInsertString(code," GF@_exp");FluidInsertString(code,"\n"); // ASign to return vall
     FluidInsertString(code,"POPFRAME\n");
     FluidInsertString(code,"RETURN\n");
     FluidInsertString(code,"LABEL $_SKIP_");FluidInsertString(code,id);FluidInsertString(code,"\n");
}

/*
* Generates header of if 
*/
void generator_if_header(Fluids*code){
     FluidInsertString(code,"#IF STATEMENT\n");
}

/*
* Generates jump to else
*/
void generator_if_jump_to_else(Fluids*code,int id){
     FluidInsertString(code,"JUMPIFNEQ $_else");FluidInsertInt(code,id);FluidInsertString(code," GF@_exp bool@true\n");
}

/*
* Generates jump to skip
*/
void generator_if_jump_to_skip(Fluids*code,int id){
     FluidInsertString(code,"JUMP $_skip");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}

/*
* Generates label else
*/
void generator_if_label_else(Fluids*code,int id){
     FluidInsertString(code,"LABEL $_else");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}

/*
* Generates label skip
*/
void generator_if_label_skip(Fluids*code,int id){
     FluidInsertString(code,"LABEL $_skip");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}

/*
* Generates while header
*/
void generator_while_header(Fluids*code){
     FluidInsertString(code,"#WHILE STATEMENT\n");}

/*
* Generates begining of while
*/
void generator_while_start(Fluids*code,int id){
     FluidInsertString(code,"LABEL $_while");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}

/*
* Generates save while exp
*/
void generator_while_save(Fluids*code){
     FluidInsertString(code,"MOVE GF@_eif_while_result GF@_exp");FluidInsertString(code,"\n");
}

/*
* Generates end of while
*/
void generator_while_label_skip(Fluids*code,int id){
     FluidInsertString(code,"LABEL $_skip");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}


/*
* Generates jump to while
*/
void generator_while_jump_to_start(Fluids*code,int id){
     FluidInsertString(code,"JUMP $_while");FluidInsertInt(code,id);FluidInsertString(code,"\n");
}

/*
* Generates jump to while
*/
void generator_while_jump_to_skip(Fluids*code,int id){
     FluidInsertString(code,"JUMPIFNEQ $_skip");FluidInsertInt(code,id);FluidInsertString(code," GF@_eif_while_result bool@true\n");
}

/*
* Generates definition of variable
*/
void generator_var_define(Fluids*code,avl_Data*node){
     FluidInsertString(code,"DEFVAR LF@_");FluidInsertArray(code,node);FluidInsertString(code,"\n");
}

/*
* Generates asignemnt of variable, goes in stack till expression is calculated then write
*/
void generator_var_asign(Fluids*code,avl_Data*node){
     FluidInsertString(code,"MOVE LF@_");FluidInsertArray(code,node);FluidInsertString(code," GF@_exp");FluidInsertString(code,"\n");
}

/*
* Generates asignemnt of variable, goes in stack till expression is calculated then write
*/
void generator_var_asign_fluid(Fluids*code,char*id){
     FluidInsertString(code,"MOVE LF@_");FluidInsertString(code,id);FluidInsertString(code," GF@_exp");FluidInsertString(code,"\n");
}

/*
* Generates print of current val of expressions
*/
void generator_print(Fluids*code){
     FluidInsertString(code,"\nWRITE GF@_exp\n");
}

/*
* If conversion is necessary it is used
*    
*|---------------|
*|               | <- STACK TO
*|---------------|
*|   OPERAND 1   |
*|---------------|
*|   OPERAND 2   |
*|---------------|
* 
*
* TYPE RESTRICTIONS
* ADDS - SAME TYPES ( INT or FLOAT)
* SUBS - SAME TYPES ( INT or FLOAT)
* MULS - SAME TYPES ( INT or FLOAT)
* DIVS - SAME TYPES ( INT )
* IDIVS - SAME TYPES ( INT )
* LTS/GTS/EQS - SAME TYPE ( BOOL, INT, FLOAT, STRING)
*/

void generator_expr(Fluids*code,PrecSym sym){
     
     FluidInsertString(code,"MOVE GF@_exp bool@false\n");
     switch (sym)
     {
      case PLUS_E:
          FluidInsertString(code,"\nMOVE GF@_exp bool@true\n");
          FluidInsertString(code,"\nCALL $comparetypes\n");
          break;
        case MIN_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"SUBS\n");
          break;
        case MUL_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"MULS\n");
          break;
        case DIV_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"DIVS\n");
          break;
        case LESS_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"LTS\n");
          break;
        case MORE_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"GTS\n");
          break;
        case EQ_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"EQS\n");
          break;
        case NEQ_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"EQS\n");
          FluidInsertString(code,"NOTS\n");
          break;
        case LEQ_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"POPS GF@_env_op1\n");
          FluidInsertString(code,"POPS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op1\n");
          FluidInsertString(code,"EQS\n");
          FluidInsertString(code,"PUSHS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op1\n");
          FluidInsertString(code,"LTS\n");
          FluidInsertString(code,"ORS\n");
          break;
        case MEQ_E:
          FluidInsertString(code,"\nCALL $comparetypes\n");
          FluidInsertString(code,"POPS GF@_env_op1\n");
          FluidInsertString(code,"POPS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op1\n");
          FluidInsertString(code,"EQS\n");
          FluidInsertString(code,"PUSHS GF@_env_op2\n");
          FluidInsertString(code,"PUSHS GF@_env_op1\n");
          FluidInsertString(code,"GTS\n");
          FluidInsertString(code,"ORS\n");
          break;
     default:
          break;
     }
}

/*
* Generates save exp from stack to GF@_exp
*/
void generator_save_exp(Fluids*code){

     FluidInsertString(code,"POPS GF@_exp\n");
}

/*
* Generates push of any value or id to stack
* Divided into sections by type of token
*/
void generator_push_something_to_stack(Fluids*code,Token token){

     
     char floatStr[40];        // Size for conversion of float do string

	switch (token.type)
     {
     case WHOLENUMBER:
      FluidInsertString(code,"PUSHS int@");FluidInsertInt(code,token.value);FluidInsertString(code,"\n");
          break;
     case DECIMALNUMBER:
      sprintf(floatStr, "%a", token.floatingValue);
      FluidInsertString(code,"PUSHS float@");FluidInsertString(code,floatStr);FluidInsertString(code,"\n");
          break;
     case STRINGVALUE:
     FluidInsertString(code,"PUSHS string@");

     for(unsigned int i =0; i<token.fluidPayload->actualLength;i++){
        if(token.fluidPayload->payload[i] == '#' || token.fluidPayload->payload[i] == '\\' || token.fluidPayload->payload[i] <= 32 || !isprint(token.fluidPayload->payload[i]))
        {
            FluidAddChar(code,92);
            FluidAddChar(code,'0');
            char c = token.fluidPayload->payload[i];
            FluidInsertInt(code,c);
        }
        else
        {
            FluidAddChar(code,token.fluidPayload->payload[i]);
        }
        
    }FluidInsertString(code,"\n");
          break;
     case IDENTIFICATOR: //TODO Mozny problem s GF
      FluidInsertString(code,"PUSHS LF@_");FluidInsertString(code,token.fluidPayload->payload);FluidInsertString(code,"\n");
          break;
     default:
     break;
     }
}


/*
* Generates store of calculated expression on stack
*/
void generate_store_stack_top(Fluids*code)
{
	FluidInsertString(code,"POPS GF@_exp\n");
}

/*
* Generates concat of two strings on stack
*/
void generator_concat_on_stack(Fluids*code)
{
    FluidInsertString(code,"POPS GF@_env_op1\n");
    FluidInsertString(code,"POPS GF@_env_op2\n");
    FluidInsertString(code,"CONCAT GF@_env_op3 GF@_env_op2 GF@_env_op1\n");
    FluidInsertString(code,"PUSHS GF@_env_op3\n");
}

/*
* STACK TYPE CONVERSIONS
*/

/*
* Generates conversion of first operand on stack to double
*/
void generate_convert_stack_env_to_double(Fluids*code)
{
	FluidInsertString(code,"INT2FLOATS\n");
}

/*
* Generates conversion of second operand on stack to double
*/
void generate_convert_stack_env2_to_double(Fluids*code)
{
	FluidInsertString(code,"POPS GF@_env_op2\n");
     FluidInsertString(code,"INT2FLOATS\n");
     FluidInsertString(code,"PUSHS GF@_env_op2\n");
}

/*
* Generates conversion of first operand on stack to int
*/
void generate_convert_stack_env_to_integer(Fluids*code)
{
	FluidInsertString(code,"FLOAT2INT\n");
}

/*
* Generates conversion of second operand on stack to int
*/
void generate_convert_stack_env2_to_integer(Fluids*code)
{
	FluidInsertString(code,"POPS GF@_env_op2\n");
     FluidInsertString(code,"FLOAT2INT\n");
     FluidInsertString(code,"PUSHS GF@_env_op2\n");
}

/*
* Generates save return
*/
void generator_save_return(Fluids*code){
     FluidInsertString(code,"PUSHS GF@_return");FluidInsertString(code,"\n");
}

/*
* Generates if let
*/
void generator_if_let(Fluids*code,char*id){
    FluidInsertString(code,"TYPE GF@_exp LF@_");FluidInsertString(code,id);FluidInsertString(code,"\n");// TODO
    FluidInsertString(code,"EQ GF@_exp GF@_exp string@nil");FluidInsertString(code,id);FluidInsertString(code,"\n");// TODO
    FluidInsertString(code,"NOT GF@_exp GF@_exp\n");
}

/*
* Generates save return
*/
void generator_write(Fluids*code,Token*token){
     
     
     char floatStr[40];        // Size for conversion of float do string

	switch (token->type)
     {
     case WHOLENUMBER:
      FluidInsertString(code,"WRITE int@");FluidInsertInt(code,token->value);FluidInsertString(code,"\n");
          break;
     case DECIMALNUMBER: // TODO HANDLING 
      sprintf(floatStr, "%a", token->floatingValue);
      FluidInsertString(code,"WRITE float@");FluidInsertString(code,floatStr);FluidInsertString(code,"\n");
          break;
     case STRINGVALUE: //TODO Handling specialnich znaku jako \\032 atd
     FluidInsertString(code,"WRITE string@");

     for(unsigned int i =0; i<token->fluidPayload->actualLength;i++){
        if(token->fluidPayload->payload[i] == '#' || token->fluidPayload->payload[i] == '\\' || token->fluidPayload->payload[i] <= 32 || !isprint(token->fluidPayload->payload[i]))
        {
            FluidAddChar(code,92);
            FluidAddChar(code,'0');
            char c = token->fluidPayload->payload[i];
            FluidInsertInt(code,c);
        }
        else
        {
            FluidAddChar(code,token->fluidPayload->payload[i]);
        }
        
    }FluidInsertString(code,"\n");
          break;
     case IDENTIFICATOR:
           FluidInsertString(code,"WRITE LF@_");FluidInsertString(code,token->fluidPayload->payload);FluidInsertString(code,"\n");
          break;
     default:
     break;
     }
}
