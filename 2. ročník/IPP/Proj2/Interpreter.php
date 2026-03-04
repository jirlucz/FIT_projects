<?php

namespace IPP\Student;


use IPP\Core\AbstractInterpreter;

require 'Frame.php';
require 'Label.php';


class Interpreter extends AbstractInterpreter
{
    public function execute(): int
    {
        // TODO: Start your code here
        // Check \IPP\Core\AbstractInterpreter for predefined I/O objects:
        $dom = $this->source->getDOMDocument();
        // $val = $this->input->readString();
        // $this->stdout->writeString("stdout");
        // $this->stderr->writeString("stderr");


        $programInstructions = [];

        $xml_handler = new xml;
        $programInstructions = $xml_handler->get_instruction_array($dom);

        // print_r($programInstructions);

        $program_labels = new Load_label($programInstructions);

        $program_frames= new Program_frames;

        $size_of_array = sizeof($programInstructions);
        $instruction_iterator = 0;

        while ($instruction_iterator < $size_of_array){
            // print_r($programInstructions[$instruction_iterator] );
            $actual_instruction = new InstructionCreate($programInstructions[$instruction_iterator], $program_frames, $program_labels, $instruction_iterator, $this->input, $this->stdout);
            if($actual_instruction->exit_code != -1){
                return $actual_instruction->exit_code;
            }
            if($actual_instruction->jump_index != -1){
                $instruction_iterator = $actual_instruction->jump_index;
            }
            else{
                $instruction_iterator++;
            }
            
        }


        
        return 0;
    }
}
