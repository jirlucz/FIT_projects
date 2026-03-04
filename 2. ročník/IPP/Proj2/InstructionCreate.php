<?php

namespace IPP\Student;

require 'Instruction.php';

class InstructionCreate 
{
    public $instruction;
    public $program_frames;
    public $program_label_list;
    public $jump_index;
    public $exit_code;
    public $index_instruction;
    public $input;
    public $source;

    public function __construct($given_instruction, $given_program_frames, $given_program_labels, $instruction_index, $interpreter_input, $interpreter_source)
    {
        $this->instruction = $given_instruction;
        $this->program_frames = $given_program_frames;
        $this->program_label_list = $given_program_labels;
        $this->index_instruction = $instruction_index;
        $this->jump_index = -1;
        $this->exit_code = -1;
        $this->input = $interpreter_input;
        $this->source = $interpreter_source;
        $this->execute_instruction();
    }

    private function execute_instruction(){
        
        switch(strtoupper($this->instruction["opcode"])){
            case 'MOVE':
                // print "MOVE\n";
                $execute_instruction = new Move($this->instruction,$this->program_frames);
                $execute_instruction->execute();
                break;
            case 'CREATEFRAME':
                // print "CREATEFRAME\n";
                $execute_instruction = new Createframe_ins($this->program_frames);
                $execute_instruction->execute();
                break;
            case 'PUSHFRAME':
                // print "PUSHFRAME\n";
                $execute_instruction = new Pushframe($this->program_frames);
                $execute_instruction->execute();
                break;
            case 'POPFRAME':
                // print "POPFRAME\n";
                $execute_instruction = new Popframe($this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'DEFVAR':
                // print "DEFVAR\n";
                // print_r($this->instruction);
                $execute_instruction = new Defvar($this->instruction,$this->program_frames ) ;
                $execute_instruction->execute();
                break;
            case 'CALL':
                // print "CALL\n";
                $execute_instruction = new Call($this->instruction,$this->program_label_list, $this->index_instruction ) ;
                $this->jump_index = $execute_instruction->execute();
                break;
            case 'RETURN':
                // print "RETURN\n";
                $execute_instruction = new ReturnInstruction($this->program_label_list) ;
                $this->jump_index = $execute_instruction->execute();
                break;
            //Data stack
            case 'PUSHS':
                // print "PUSHS\n";
                $execute_instruction = new Pushs($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'POPS':
            //Arithmetical, relate, bool and converse
                // print "POPS\n";
                $execute_instruction = new Pops($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'ADD':
                // print "ADD\n";
                $execute_instruction = new Add($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'SUB':
                // print "SUB\n";
                $execute_instruction = new Sub($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'MUL':
                // print "MUL\n";
                $execute_instruction = new Mul($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'IDIV':
                // print "IDIV\n";
                $execute_instruction = new Idiv($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'LT':
                // print "LT\n";
                $execute_instruction = new Lower($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'GT':
                // print "GT\n";
                $execute_instruction = new Greater($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'EQ':
                // print "EQ\n";
                $execute_instruction = new Equal($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'AND':
                // print "AND\n";
                $execute_instruction = new AND_class($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'OR':
                // print "OR\n";
                $execute_instruction = new OR_class($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'NOT':
                // print "NOT\n";
                $execute_instruction = new NOT_class($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'INT2CHAR':
                // print "INT2CHAR\n";
                $execute_instruction = new Int_to_char($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'STRI2INT':
                // print "STRI2INT\n"; NOT TESTED
                $execute_instruction = new String_to_int($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            //I/O
            case 'READ':
                // print "READ\n";
                $execute_instruction = new Read($this->instruction,$this->program_frames) ;
                $execute_instruction->execute($this->input);

                break;
            case 'WRITE':
                // print "WRITE\n";
                $execute_instruction = new Write($this->instruction,$this->program_frames) ;
                $execute_instruction->execute($this->source);
                break;
            //String operation
            case 'CONCAT':
                // print "CONCAT\n";
                $execute_instruction = new Concat($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'STRLEN':
                // print "STRLEN\n";
                $execute_instruction = new Strlen($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'GETCHAR':
                // print "GETCHAR\n";
                $execute_instruction = new Getchar($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            case 'SETCHAR':
                // print "SETCHAR\n";
                $execute_instruction = new Setchar($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();
                break;
            //Type
            case 'TYPE':
                // print "TYPE\n"
                $execute_instruction = new Type($this->instruction,$this->program_frames) ;
                $execute_instruction->execute();;
                break;
            //Flow control
            case 'LABEL':
                // print "LABEL\n";
                //Label is already processed in Load_Label class
                break;
            case 'JUMP':
                // print "JUMP\n";
                $execute_instruction = new Jump($this->instruction,$this->program_label_list) ;
                $this->jump_index = $execute_instruction->execute();
                break;
            case 'JUMPIFEQ':
                // print "JUMPIFEQ\n";
                $execute_instruction = new Jump_If_Equal($this->instruction,$this->program_frames, $this->program_label_list) ;
                $this->jump_index = $execute_instruction->execute();
                break;
            case 'JUMPIFNEQ':
                // print "JUMPIFNEQ\n";
                $execute_instruction = new Jump_If_Notequal($this->instruction,$this->program_frames, $this->program_label_list) ;
                $this->jump_index = $execute_instruction->execute();
                break;
            case 'EXIT':
                // print "EXIT\n";
                $execute_instruction = new Exit_instruction($this->instruction,$this->program_frames);
                $this->exit_code = $execute_instruction->execute();
                break;
            //Debug
            case 'DPRINT':
                // print "DPRINT\n";
                break;
            case 'BREAK':
                // print "BREAK\n";
                break;
            default:
                // print "No instruction matched\n";
                throw new XMLBadStructException;
        }
    }
}