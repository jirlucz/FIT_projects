<?php

namespace IPP\Student;


use IPP\Core\StreamWriter;
use IPP\Core\FileInputReader;



//Add argument check here
//And other functions here
class Instruction
{
    public mixed $program_frames;

    public string $arg1_type;  
    public string $arg1_frame;
    public string $arg1_variable_name;
    public string $arg1_value;

    public string $arg2_type;  
    public string $arg2_frame;
    public string $arg2_variable_name;
    public string $arg2_value;

    public string $arg3_type;  
    public string $arg3_frame;
    public string $arg3_variable_name;
    public string $arg3_value;
    

    public function __construct(){

    }


    //Method fom getting frame and name of the variable
    protected function parse_var($raw_var){
        list($frame_name, $variable_name) = explode('@', $raw_var);

        //Check if @ was found
        if($frame_name == null || $variable_name == null){
            throw new XMLBadStructException;
        }

        //Check data frame
        if( $frame_name != "GF" && $frame_name != "LF" && $frame_name != "TF"){
            throw new XMLBadStructException;
        }

        //Debug
        // print "Frame: " .$frame_name. "\n";
        // print "Variable name: " .$variable_name. "\n";

        $return_arr [] = $frame_name;
        $return_arr [] = $variable_name;

        return $return_arr;
    }

    //Method tests if type is var
    protected function check_var($var_test){
        if($var_test != "var"){
            throw new XMLBadStructException;
        }
    }

    
    //Method tests if type is symbol (can be constant or variable)
    protected function check_symbol($symbol_test){
        if($symbol_test == "var"){
            
        }
        elseif ($symbol_test == "int"){

        }
        elseif ($symbol_test == "string"){

        }
        elseif ($symbol_test == "bool"){

        }
        elseif ($symbol_test == "nil"){

        }
        else{
            throw new XMLBadStructException;
        }
    }

    protected function check_label($label_to_check) : void{
        if($label_to_check != "label"){
            throw new SemanticErrorWrongOperandException;
        }
    }

    protected function check_int_value($number_to_check) : void{
        if(is_numeric(($number_to_check)) != true){
            throw new SemanticErrorWrongOperandException;
        }
    }

    protected function check_string_value($string_to_check) : void{
        if(is_string($string_to_check) != true){
            throw new SemanticErrorWrongOperandException;
        }
    }

    protected function check_int_type($type_to_check) : void{
        if($type_to_check != "int"){
            throw new SemanticErrorWrongOperandException;
        }
    }

    protected function check_bool_type($type_to_check) : void{
        if($type_to_check != "bool"){
            throw new SemanticErrorWrongOperandException;
        }
    }

    protected function check_string_type($type_to_check) : void{
        if($type_to_check != "string"){
            throw new SemanticErrorWrongOperandException;
        }
    }

    //Method used in JUMP_N/EQ  //nil???
    protected function check_same_type($arg1_type, $arg2_type) : void {
        if($arg1_type == "nil" && $arg2_type != "nil"){
            return;
        }
        elseif($arg1_type != "nil" && $arg2_type == "nil"){
            return;
        }elseif($arg1_type == "nil" && $arg2_type == "nil"){
            throw new CantCompareException;
        }
        if ($arg1_type != $arg2_type ){
            throw new CantCompareException;
        }
    }

    //Method used in JUMP_N/EQ
    protected function are_same_value($arg1_value, $arg2_value) : bool {
        if ($arg1_value == $arg2_value ){
            return true;
        }
        return false;
    }

    protected function check_type ($type) : string {
        if($type == "string"){
            return "string";
        }
        elseif ($type == "int"){
            return "int";
        }
        elseif ($type == "bool"){
            return "bool";
        }
        else{
            throw new UnknownTypeException;
        }
    }

    protected function check_bool_value($value) : void {
        if($value != "true" && $value != "false" ){
            throw new SemanticErrorIncompatibleException;
        }
    }

    protected function compare_two_values($val1, $type1, $val2, $type2, $operation) : bool{
        //Basic type check
        if($type1 != $type2){
            throw new CantCompareException;
        }
        if(($type1 == "nil" || $type2 == "nil") && ($operation != "EQ")){   //check
            throw new CantCompareException;
        }
        if ($type1 == "bool" || $type2 == "bool"){
            $this->check_bool_value($val1);
            $this-> check_bool_value($val2);
        }
        elseif($type1 == "int" || $type2 == "int"){
            $this->check_int_value($val1);
            $this->check_int_value($val2);
        }
        elseif($type1 == "string" || $type2 == "string"){
            $this->check_string_value($type1);
            $this->check_string_value($type2);
        }
        else{
            throw new SemanticErrorIncompatibleException;
        }
        
        if($operation == "LT"){
            if($val1 < $val2 ){
                return true;
            }
            return false;
        }
        elseif($operation == "GT"){
            if($val1 > $val2 ){
                return true;
            }
            return false;
        }
        elseif($operation == "EQ"){
            if($val1 == $val2 ){
                return true;
            }
            return false;
        }
        else{
            return false;
        }
    }

    protected function  argument_variable($parsed_instruction, $frame) : void {
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
    }

    protected function argument_symbol($parsed_instruction, $frame, $argument_position) : void {
        $this->program_frames = $frame; 
        switch($argument_position){
            case "1":
                if ($parsed_instruction["arg1_type"] == "var"){
                    $var1_parts = [];
                    $var1_parts = $this->parse_var($parsed_instruction["arg1_value"]);
                    $this->arg1_type = $parsed_instruction["arg1_type"];
                    $this->arg1_frame = $var1_parts[0]; 
                    $this->arg1_variable_name = $var1_parts[1];
                    $this->check_var($this->arg1_type);
                    // print "Working with variable, which names " .$this->arg1_variable_name. ", type is ".$this->arg1_type. " on frame " .$this->arg1_frame. "\n";
                    $this->arg1_value = $this->program_frames->get_variable_value($this->arg1_variable_name, $this->arg1_frame);
                    $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
                }
                else{
                    $this->arg1_type = $parsed_instruction["arg1_type"];
                    $this->arg1_value = $parsed_instruction["arg1_value"];
                    $this->check_symbol($this->arg1_type);
                    // print "Working with constant " .$this->arg1_value. " of type ".$this->arg1_type. "\n";
                }
                break;
            case "2":
                if ($parsed_instruction["arg2_type"] == "var"){
                    $var2_parts = [];
                    $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
                    $this->arg2_type = $parsed_instruction["arg2_type"];
                    $this->arg2_frame = $var2_parts[0]; 
                    $this->arg2_variable_name = $var2_parts[1];
                    $this->check_var($this->arg2_type);
                    // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
                    $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
                    $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
                }
                else{
                    $this->arg2_type = $parsed_instruction["arg2_type"];
                    $this->arg2_value = $parsed_instruction["arg2_value"];
                    $this->check_symbol($this->arg2_type);
                    // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
                }
                break;
            case "3":
                break;
            default:
                //internal error
                break;
        }
        
    }

}

//Memory frames & function call

//TODO CHECK DATA TYPE IF CORRESPOND WITH INSERTING DATA TYPE!!! e.g. int -> 42, string-. "hello" NOT int->"Hello"
class Move extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $this->argument_variable($parsed_instruction, $frame);

        //{symbol} position 2
       $this->argument_symbol($parsed_instruction, $frame , "2");
    }

    public function execute(){
        $this->program_frames->update_variable_value($this->arg1_variable_name, $this->arg2_value, $this->arg2_type, $this->arg1_frame);
        // $this->program_frames->update_variable_type($this->arg1_variable_name, $this->arg2_type, $this->arg1_frame);
        
    }


}

class Createframe_ins extends  Instruction
{
    public function __construct($frame) {
        $this->program_frames = $frame; 
    }

    public function execute(){
        $this->program_frames->create_new_temporal_frame();
    }
}

class Pushframe extends  Instruction
{
    public function __construct($frame){
        $this->program_frames = $frame; 
    }
    public function execute(){
        $this->program_frames->push_temporal_frame_to_LF($this->program_frames);
    }
}

class Popframe extends  Instruction
{
    public function __construct($frame){
        $this->program_frames = $frame; 
    }
    public function execute(){
        $this->program_frames->pop_local_frame($this->program_frames);
    }
}

class Defvar extends  Instruction
{
    

    public function __construct($parsed_instruction, $frame){
        $this->argument_variable($parsed_instruction, $frame);
    }

    public function execute(){
        $this->check_var($this->arg1_type);
        $this->program_frames->store_to_frame($this->arg1_variable_name, $this->arg1_type, null,$this->arg1_frame );
        //Debug
        // $returnTest = $this->program_frames->find_in_frame($this->arg1_variable_name, $this->arg1_frame );
        // if($returnTest == null){
        //     print "Not found\n";
        // }
        // else{
        //     print "Found\n";
        // }
    }
}

class Call extends  Instruction
{
    protected $instruction_index_backup;
    protected $label_list; 
    protected $target_label_name;
    
    public function __construct($parsed_instruction, $given_label_list, $actual_instruction_index){
        $this->label_list = $given_label_list;
        $this->target_label_name = $parsed_instruction["arg1_value"];
        $this->check_label($parsed_instruction["arg1_type"]);
        $this->instruction_index_backup = $actual_instruction_index;
    }

    
    public function execute() : int {
        // print_r($this->label_list);
        // print "Index of label is " .$this->label_list->find_label($this->target_label_name). "\n";
        return $this->label_list->jump_to_label_with_backup($this->target_label_name, $this->instruction_index_backup);
    }
}

class ReturnInstruction extends  Instruction
{
    protected $label_list; 

    public function __construct($given_label_list){
        $this->label_list = $given_label_list;

    }
    
    public function execute() : int {
       
        return $this->label_list->return_instruction_index_from_call();
    }
}

//Data stack
class Pushs extends  Instruction
{
    public function __construct($parsed_instruction, $frame) {
        $this->program_frames = $frame; 
        $this->argument_symbol($parsed_instruction, $frame, "1");
    }

    public function execute(){
        $this->program_frames->data_frame->push_to_data_frame($this->arg1_value, $this->arg1_type );
    }
}

class Pops extends  Instruction
{
    protected $symbol;

    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        
    }

    public function execute(){
        //Value [0] = value, Value[1] = type 
        $this->symbol = $this->program_frames->data_frame->pop_from_data_frame();
        // print "Get val " . $this->symbol["value"]. " with value " .$this->symbol["type"]. "\n";
        $this->program_frames->update_variable_value_pops($this->arg1_variable_name, $this->symbol["value"], $this->symbol["type"], $this->arg1_frame );
    }
}


//Abstract_arithmetical, relate, bool and converse
abstract class Abstract_arithmetical extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        // print "Var at Add has type ".$this->arg1_type. "\n";

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }

        //{symbol2} can be var || constant
        if ($parsed_instruction["arg3_type"] == "var"){
            $var3_parts = [];
            $var3_parts = $this->parse_var($parsed_instruction["arg3_value"]);
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_frame = $var3_parts[0]; 
            $this->arg3_variable_name = $var3_parts[1];
            $this->check_var($this->arg3_type);
            // print "Working with variable, which names " .$this->arg3_variable_name. ", type is ".$this->arg3_type. " on frame " .$this->arg3_frame. "\n";
            $this->arg3_value = $this->program_frames->get_variable_value($this->arg3_variable_name, $this->arg3_frame);
            $this->arg3_type = $this->program_frames->get_variable_type($this->arg3_variable_name, $this->arg3_frame);
            // print "I got a value " .$this->arg3_value. "\n";
        }
        else{
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_value = $parsed_instruction["arg3_value"];
            $this->check_symbol($this->arg3_type);
            // print "Working with constant " .$this->arg3_value. " of type ".$this->arg3_type. "\n";
        }
    }

    public function execute(){
        $this->check_int_type($this->arg2_type);    //Type of both symbols must be INT
        $this->check_int_type($this->arg3_type);
        $this->check_int_value($this->arg2_value);  //Value of both symbols must be INT
        $this->check_int_value($this->arg3_value);
        $output = $this->do_math_operation($this->arg2_value, $this->arg3_value);
        // print "calculate " .$this->arg2_value. " operand " .$this->arg3_value. " is " .$output. " and store it inside " .$this->arg1_variable_name. " of type " .$this->arg1_type. "\n";
        $this->program_frames->update_variable_value($this->arg1_variable_name, $output, "int",$this->arg1_frame);
        // $this->program_frames->update_variable_type($this->arg1_variable_name, $this->arg2_type, $this->arg1_frame);
    }

    protected function do_math_operation($operand1, $operand2) {
        return;
    }
}

class Add extends  Abstract_arithmetical
{
    //Override 
    protected function do_math_operation($operand1, $operand2) {
        $operand1_int = intval($operand1);
        $operand2_int = intval($operand2);

        return $operand1_int + $operand2_int;
    }
}

class Sub extends  Abstract_arithmetical
{
    //Override 
    protected function do_math_operation($operand1, $operand2) {
        $operand1_int = intval($operand1);
        $operand2_int = intval($operand2);

        return $operand1_int - $operand2_int;
    }
}

class Mul extends  Abstract_arithmetical
{
    //Override 
    protected function do_math_operation($operand1, $operand2) {
        $operand1_int = intval($operand1);
        $operand2_int = intval($operand2);

        return $operand1_int * $operand2_int;
    }
}

class Idiv extends  Abstract_arithmetical 
{
    //Override 
    protected function do_math_operation($operand1, $operand2) {
        $operand1_int = intval($operand1);
        $operand2_int = intval($operand2);

        //0 div
        if ($operand2_int == 0){
            throw new ZeroDivException;
        }

        return $operand1_int / $operand2_int;
    }
}

class Lower extends  Abstract_arithmetical
{
    public function execute() : void {
        $this->check_same_type($this->arg2_type, $this->arg3_type);
        $return_value = $this->compare_two_values($this->arg2_value,$this->arg2_type,$this->arg3_value, $this->arg3_type, "LT" );
        $return_type = $this->arg2_type;
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, $return_type, $this->arg1_frame);

    }
}

class Greater extends  Abstract_arithmetical
{
    public function execute() : void {
        $this->check_same_type($this->arg2_type, $this->arg3_type);
        $return_value = $this->compare_two_values($this->arg2_value,$this->arg2_type,$this->arg3_value, $this->arg3_type, "GT" );
        $return_type = $this->arg2_type;
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, $return_type, $this->arg1_frame);
    }
}

class Equal extends  Move 
{
    public function execute() : void {
        $this->check_same_type($this->arg2_type, $this->arg3_type);
        $return_value = $this->compare_two_values($this->arg2_value,$this->arg2_type,$this->arg3_value, $this->arg3_type, "EQ" );
        $return_type = $this->arg2_type;
        if($return_type == "nil"){
            $return_type = $this->arg3_type;
        }
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, $return_type, $this->arg1_frame);
    }
}

class AND_class extends Abstract_arithmetical
{
    public function execute(){
        $this->check_bool_type($this->arg2_type);
        $this->check_bool_type($this->arg3_type);
        if ($this->arg2_value == "true" && $this->arg3_value == "true"){
            $return_value = "true";
        }
        else{
            $return_value = "false";
        }
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, "bool", $this->arg1_frame);
  
    }
}

class OR_class extends Abstract_arithmetical
{
    public function execute(){
        $this->check_bool_type($this->arg2_type);
        $this->check_bool_type($this->arg3_type);
        if ($this->arg2_value == "true" || $this->arg3_value == "true"){
            $return_value = "true";
        }
        else{
            $return_value = "false";
        }
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, "bool", $this->arg1_frame);
  
    }
}

class NOT_class extends Move
{
    public function execute(){
        $this->check_bool_type($this->arg2_type);
        if ($this->arg2_value == "true" ){
            $return_value = "false";
        }
        else{
            $return_value = "true";
        }
        $this->program_frames->update_variable_value($this->arg1_variable_name, $return_value, "bool", $this->arg1_frame);
  
    }
}

class Int_to_char extends  Move //Has only 2 operands
{
    //Override
    public function execute(){
        $char = $this->convert_int_to_char($this->arg2_value);
        $this->program_frames->update_variable_value($this->arg1_variable_name, $char, "string", $this->arg1_frame);
        
    }

    private function convert_int_to_char ($string) : string {
        if(!is_numeric($string)){
            throw new StringOperationNotUnicodeValException;
        }

        $char = mb_chr($string, 'UTF8');

        if ($char == false){
            throw new StringOperationNotUnicodeValException;
        }

        return $char;
    }
}

class String_to_int extends  Instruction //Has only 2 operands
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        // print "Var at Add has type ".$this->arg1_type. "\n";

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }

        //{symbol2} can be var || constant
        if ($parsed_instruction["arg3_type"] == "var"){
            $var3_parts = [];
            $var3_parts = $this->parse_var($parsed_instruction["arg3_value"]);
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_frame = $var3_parts[0]; 
            $this->arg3_variable_name = $var3_parts[1];
            $this->check_var($this->arg3_type);
            // print "Working with variable, which names " .$this->arg3_variable_name. ", type is ".$this->arg3_type. " on frame " .$this->arg3_frame. "\n";
            $this->arg3_value = $this->program_frames->get_variable_value($this->arg3_variable_name, $this->arg3_frame);
            $this->arg3_type = $this->program_frames->get_variable_type($this->arg3_variable_name, $this->arg3_frame);
            // print "I got a value " .$this->arg3_value. "\n";
        }
        else{
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_value = $parsed_instruction["arg3_value"];
            $this->check_symbol($this->arg3_type);
            // print "Working with constant " .$this->arg3_value. " of type ".$this->arg3_type. "\n";
        }
    }

    public function execute(){
        $char = $this->return_int_from_string_with_index($this->arg2_value, $this->arg3_value);
        $this->program_frames->update_variable_value($this->arg1_variable_name, $char, "string", $this->arg1_frame);
        
    }

    private function return_int_from_string_with_index ($string, $position) : int {
        //Position must be numeric
        if(!is_numeric($position)){
            throw new StrToCharNotNumException;
        }

        if(!is_string($string)){
            throw new StrToCharNotStringException;
        }

        $position_index = intval($position);
        $string_length = strlen($string);

        if ($position_index < 0 || $position_index >= $string_length){
            throw new StrToCharOutOfIndexException;
        }

        $int_val_of_char = mb_ord($string[$position_index]);

        if ($int_val_of_char == false){
            throw new StrToCharFunctionErrorException;
        }
            
        return $int_val_of_char;
    }
}

// Input/Output

class Read extends  Instruction //Has only 2 operands
{
    private $type_to_read;

    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        //{type}    
        $this->arg2_value   = $parsed_instruction["arg2_value"];
        $this->arg2_type    = $parsed_instruction["arg2_type"];
        if($this->arg2_type != "type"){
            throw new XMLBadStructException;
        }
        $this->type_to_read = $this->check_type($this->arg2_value);

    }

    public function execute($interpreter_input){
        // $this->file_input_reader = new FileInputReader;
        $value = ($interpreter_input->readString());
        if ($value == null){
            $this->program_frames->update_variable_value($this->arg1_variable_name, "nil", "nil", $this->arg1_frame);
        }
        else{
            $this->program_frames->update_variable_value($this->arg1_variable_name, $value, $this->type_to_read, $this->arg1_frame);
        }
    }
}

class Write extends  Instruction //Has only 2 operands
{
    public function __construct($parsed_instruction, $frame){
        //{symbol} can be var || constant
        
        if($this->arg1_type == "string"){
            $this->arg1_value = $this->convertEscapeSequences($this->arg1_value);
        }
    }

    private function convertEscapeSequences($string) {
        //Regular expression
        $pattern = '/\\\\(\d{3})/';
        if($string == null){
            return "";
        }
        //Callback function to convert the matched number to a character representation
        $callback = function($matches) {
            $number = intval($matches[1]); 
            return chr($number); // Convert the integer to character
        };

        //Replace occurrences of escape sec. with the char
        return preg_replace_callback($pattern, $callback, $string);
    }

    
    public function execute($source){
        $this->check_symbol($this->arg1_type);    
        switch ($this->arg1_type){
            case 'int':
                $source->writeInt($this->arg1_value);
                break;
            case 'string':
                $source->writeString($this->arg1_value);
                break;
            case 'bool':
                $source->writeBool($this->arg1_value);
            break;
            case 'nil':
                $source->writeString("");    //Empty string?
                break;
        }
        // print "Value printed: " .$this->arg1_value. "\n";
        
    }
}

// Functions for working with strings
class Concat extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        // print "Var at Add has type ".$this->arg1_type. "\n";

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }

        //{symbol2} can be var || constant
        if ($parsed_instruction["arg3_type"] == "var"){
            $var3_parts = [];
            $var3_parts = $this->parse_var($parsed_instruction["arg3_value"]);
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_frame = $var3_parts[0]; 
            $this->arg3_variable_name = $var3_parts[1];
            $this->check_var($this->arg3_type);
            // print "Working with variable, which names " .$this->arg3_variable_name. ", type is ".$this->arg3_type. " on frame " .$this->arg3_frame. "\n";
            $this->arg3_value = $this->program_frames->get_variable_value($this->arg3_variable_name, $this->arg3_frame);
            $this->arg3_type = $this->program_frames->get_variable_type($this->arg3_variable_name, $this->arg3_frame);
            // print "I got a value " .$this->arg3_value. "\n";
        }
        else{
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_value = $parsed_instruction["arg3_value"];
            $this->check_symbol($this->arg3_type);
            // print "Working with constant " .$this->arg3_value. " of type ".$this->arg3_type. "\n";
        }
    }

    public function execute(){
        $this->check_string_type($this->arg2_type);
        $this->check_string_type($this->arg3_type);

        $concat_string = $this->arg2_value . $this->arg3_value;

        $this->program_frames->update_variable_value($this->arg1_variable_name, $concat_string, "string", $this->arg1_frame );
    }
}

class Strlen extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        // print "Var at Add has type ".$this->arg1_type. "\n";

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }
    }

    public function execute(){
        $string_length = $this->count_string_length($this->arg2_value);
        $this->program_frames->update_variable_value($this->arg1_variable_name, $string_length, "int", $this->arg1_frame );
    }

    private function count_string_length($string) : int {
        if(!is_string($string)){
            throw new StrToCharNotStringException;
        }
        return strlen($string);
    }
}

class Getchar extends  Concat
{
    //Override
    public function execute(){
        $this->check_string_type($this->arg2_type);
        $this->check_int_type($this->arg3_type);

        $char = $this->return_char_from_string_with_index($this->arg2_type, $this->arg3_type);

        $this->program_frames->update_variable_value($this->arg1_variable_name, $char, "string", $this->arg1_frame );
    }

    private function return_char_from_string_with_index ($string, $position) : string {
        //Position must be numeric
        if(!is_numeric($position)){
            throw new StrToCharNotNumException;
        }

        if(!is_string($string)){
            throw new StrToCharNotStringException;
        }

        $position_index = intval($position);
        $string_length = strlen($string);

        if ($position_index < 0 || $position_index >= $string_length){
            throw new StrToCharOutOfIndexException;
        }

        $char_val_of_char = $string[$position_index];

        if ($char_val_of_char == false){
            throw new StrToCharFunctionErrorException;
        }
            
        return $char_val_of_char;
    }
}

class Setchar extends  Concat
{
    //Override
    public function execute(){
        $this->check_string_type($this->arg1_type); //Default string
        $this->check_int_type($this->arg2_type);    //Index
        $this->check_string_type($this->arg3_type); //New char

        $result_string = $this->replace_char_in_string_at_index($this->arg1_type,$this->arg3_type, $this->arg2_type );

        $this->program_frames->update_variable_value($this->arg1_variable_name, $result_string, "string", $this->arg1_frame );
    }

    private function replace_char_in_string_at_index ($string,$new_char ,$position) : string {
        //Position must be numeric
        if(!is_numeric($position)){
            throw new StrToCharNotNumException;
        }

        if(!is_string($string)){
            throw new StrToCharNotStringException;
        }

        if(!is_string($new_char)){
            throw new StrToCharNotStringException;
        }

        $position_index = intval($position);
        $string_length = strlen($string);

        if ($position_index < 0 || $position_index >= $string_length){
            throw new StrToCharOutOfIndexException;
        }

        $string_arr = str_split($string);
        $new_char_arr = str_split($new_char);
        $string_arr[$position_index] = $new_char_arr[0];
        $result = implode($string_arr);
            
        return $result;
    }
}

//Type
class Type extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        //{var}
        $var_parts = [];
        $var_parts = $this->parse_var($parsed_instruction["arg1_value"]);
        $this->arg1_type = $parsed_instruction["arg1_type"];
        $this->arg1_frame = $var_parts[0]; 
        $this->arg1_variable_name = $var_parts[1];
        $this->program_frames = $frame; 
        $this->check_var($this->arg1_type);
        //Check type of variable
        $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        // print "Var at Add has type ".$this->arg1_type. "\n";

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }
    }

    public function execute(){
        $this->program_frames->update_variable_value($this->arg1_variable_name, $this->arg2_value, "string", $this->arg1_frame );
    }
}

//Functions for flow control of the program

class Label extends  Instruction
{
}

class Jump extends  Instruction
{
    protected $label_list; 
    protected $target_label_name;

    public function __construct($parsed_instruction, $given_label_list){
        $this->label_list = $given_label_list;
        $this->target_label_name = $parsed_instruction["arg1_value"];
        $this->check_label($parsed_instruction["arg1_type"]);
        
    }

    public function execute() : int {
        // print "Index of label is " .$this->label_list->find_label($this->target_label_name). "\n";
        return $this->label_list->find_label($this->target_label_name);
    }

}

class Jump_If_Equal extends  Jump
{
    //Override
    public function __construct($parsed_instruction,$program_frames, $given_label_list){
        //{label}
        $this->label_list = $given_label_list;
        $this->target_label_name = $parsed_instruction["arg1_value"];
        $this->check_label($parsed_instruction["arg1_type"]);
        $this->program_frames = $program_frames;

        //{symbol1} can be var || constant
        if ($parsed_instruction["arg2_type"] == "var"){
            $var2_parts = [];
            $var2_parts = $this->parse_var($parsed_instruction["arg2_value"]);
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_frame = $var2_parts[0]; 
            $this->arg2_variable_name = $var2_parts[1];
            $this->check_var($this->arg2_type);
            // print "Working with variable, which names " .$this->arg2_variable_name. ", type is ".$this->arg2_type. " on frame " .$this->arg2_frame. "\n";
            $this->arg2_value = $this->program_frames->get_variable_value($this->arg2_variable_name, $this->arg2_frame);
            $this->arg2_type = $this->program_frames->get_variable_type($this->arg2_variable_name, $this->arg2_frame);
        }
        else{
            $this->arg2_type = $parsed_instruction["arg2_type"];
            $this->arg2_value = $parsed_instruction["arg2_value"];
            $this->check_symbol($this->arg2_type);
            // print "Working with constant " .$this->arg2_value. " of type ".$this->arg2_type. "\n";
        }

        //{symbol2} can be var || constant
        if ($parsed_instruction["arg3_type"] == "var"){
            $var3_parts = [];
            $var3_parts = $this->parse_var($parsed_instruction["arg3_value"]);
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_frame = $var3_parts[0]; 
            $this->arg3_variable_name = $var3_parts[1];
            $this->check_var($this->arg3_type);
            // print "Working with variable, which names " .$this->arg3_variable_name. ", type is ".$this->arg3_type. " on frame " .$this->arg3_frame. "\n";
            $this->arg3_value = $this->program_frames->get_variable_value($this->arg3_variable_name, $this->arg3_frame);
            $this->arg3_type = $this->program_frames->get_variable_type($this->arg3_variable_name, $this->arg3_frame);
            // print "I got a value " .$this->arg3_value. "\n";
        }
        else{
            $this->arg3_type = $parsed_instruction["arg3_type"];
            $this->arg3_value = $parsed_instruction["arg3_value"];
            $this->check_symbol($this->arg3_type);
            // print "Working with constant " .$this->arg3_value. " of type ".$this->arg3_type. "\n";
        }
    }

    public function execute(): int
    {
        $this->check_same_type($this->arg2_type, $this->arg3_type);
        //If variables have same values jump
        if($this->are_same_value($this->arg2_value, $this->arg3_value)){
            return $this->label_list->find_label($this->target_label_name);
        }
        //Else continue with next instruction
        return -1;
    }
}

class Jump_If_Notequal extends  Jump_If_Equal
{
    //Override
    public function execute(): int
    {
        $this->check_same_type($this->arg2_type, $this->arg3_type);
        //If variables have same values jump
        if(!($this->are_same_value($this->arg2_value, $this->arg3_value))){
            return $this->label_list->find_label($this->target_label_name);
        }
        //Else continue with next instruction
        return -1;
    }
}

class Exit_instruction extends  Instruction
{
    public function __construct($parsed_instruction, $frame){
        
        $this->program_frames = $frame; 

        //{symbol} can be var || constant
        if ($parsed_instruction["arg1_type"] == "var"){
            $var1_parts = [];
            $var1_parts = $this->parse_var($parsed_instruction["arg1_value"]);
            $this->arg1_type = $parsed_instruction["arg1_type"];
            $this->arg1_frame = $var1_parts[0]; 
            $this->arg1_variable_name = $var1_parts[1];
            $this->check_var($this->arg1_type);
            // print "Working with variable, which names " .$this->arg1_variable_name. ", type is ".$this->arg1_type. " on frame " .$this->arg1_frame. "\n";
            $this->arg1_value = $this->program_frames->get_variable_value($this->arg1_variable_name, $this->arg1_frame);
            $this->arg1_type = $this->program_frames->get_variable_type($this->arg1_variable_name, $this->arg1_frame);
        }
        else{
            $this->arg1_type = $parsed_instruction["arg1_type"];
            $this->arg1_value = $parsed_instruction["arg1_value"];
            $this->check_symbol($this->arg1_type);
            // print "Working with constant " .$this->arg1_value. " of type ".$this->arg1_type. "\n";
        }
    }

    public function execute() : int {
        if (!is_numeric($this->arg1_value)){
            throw new ExitCodeErrorException;
        }

        if($this->arg1_value < 0 || $this->arg1_value > 9){
            throw new ExitCodeErrorException;
        }
        return $this->arg1_value;
    }
}

//Debug 
class Dprint_Debug extends  Instruction
{

}

class Break_Debug extends  Instruction
{
    
}