<?php

namespace IPP\Student;

class Load_label{

    private mixed $list_of_labels;
    private mixed $instruction_lane;
    private mixed $call_stack;
    private int $call_stack_size;

    public function __construct($instructions)
    {
        $this->list_of_labels = [];
        $this->call_stack = [];
        $this->call_stack_size = -1;
        $this->load_labels($instructions);
        
    }

    private function  load_labels($instruction_array) : void {

        $this->instruction_lane = 0;
        foreach ($instruction_array as $instruction) {
            // print_r($instruction);
            if(strtoupper($instruction["opcode"]) == "LABEL"){
                if($instruction["arg1_type"] != "label"){
                    throw new SemanticErrorWrongOperandException;
                }
                $label["label_name"]         = $instruction["arg1_value"];
                $label["line_number"]   = $this->instruction_lane;
                $this->load_single_label($label);
            }
            $this->instruction_lane++;
        }
        // print_r($this->list_of_labels);
    }

    private function load_single_label ($label_to_add){
        foreach ($this->list_of_labels as $label_record){
            if($label_record["label_name"] == $label_to_add["label_name"]){
                throw new LabelNameException;
            }
        }
        $this->list_of_labels[] = $label_to_add;
    }

    //Method returns index of label
    public function find_label ($target_label) : int{
        foreach ($this->list_of_labels as $single_label){
            if ($single_label["label_name"] == $target_label){
                return $single_label["line_number"];
            }
        }

        throw new LabelNotFoundException;
    }

    public function jump_to_label_with_backup($target_label, $index_backup){
        
        foreach ($this->list_of_labels as $single_label){
            if ($single_label["label_name"] == $target_label){
                //Store next instruction index
                $this->call_stack[] = ($index_backup+1);
                $this->call_stack_size++;
                return $single_label["line_number"];
            }
        }

        throw new LabelNotFoundException;
    }

    public function return_instruction_index_from_call (): int {
        if ($this->call_stack_size < 0){
            throw new EmptyCallStackException;
        }
        $return_index = $this->call_stack[$this->call_stack_size];
        $this->call_stack_size--;

        return $return_index;
    }

    
}