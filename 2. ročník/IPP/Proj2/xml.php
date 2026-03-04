<?php

namespace IPP\Student;


use IPP\Core\Exception\XMLException;

require 'XMLException2.php';


class xml
{
    public function __construct()
    {
    }

    //Method for checking XML head
    private function xml_header_check(mixed $dom): int 
    {
        $targetLanguage = "IPPcode24";
        $line = $dom->getElementsByTagName('program');
        $headerFound = false;

        foreach ($line as $instruction) {
            if ($instruction->getAttribute('language') == $targetLanguage) {
                // Extract and print the opcode of the target instruction
                $headerFound = true;
                break;
            }
        }

       if ($headerFound == false){
            // echo "Header not found or in wrong format";
            throw new XMLBadStructException;
       }

        return 0;
    }


    //Method for converting input to and array of associative arrays sorted by order
    private function xml_instruction_order(mixed $dom)
    {
        $instructionNodes = $dom->getElementsByTagName('instruction');  //Get all instructions
        $orderNumbers = [];                                             //Array for storing loaded instruction orders
        $instructionAssociativeArray = [];                              //Array of associative arrays with parsed instructions

        foreach ($instructionNodes as $instructionNode) {
            $instructionDetails = $this->create_associative_array();    

            //Try to find order attribute
            if ($instructionNode->hasAttribute('order')) {
                $order = $instructionNode->getAttribute('order');
                //Duplicate order number or negative number
                if (in_array($order, $orderNumbers) || intval($order) <= 0) {
                    throw new XMLBadStructException; //Err 32
                }
                
                $orderNumbers[] = $order;   //Add number of order to help array

                //Fill the associative array 
                //Fill order
                $instructionDetails["order"] = $order;
                //Try to fill opcode
                if ($instructionNode->hasAttribute('opcode')){
                    $instructionDetails["opcode"] = $instructionNode->getAttribute('opcode');
                }
                else{
                    throw new XMLBadStructException;
                }
                 // Check and fill arg1
                if ($instructionNode->hasChildNodes()) {
                    $arg1Node = $instructionNode->getElementsByTagName('arg1')->item(0);
                    if ($arg1Node) {
                        $instructionDetails["arg1_type"] = $arg1Node->getAttribute('type');
                        $instructionDetails["arg1_value"] = $arg1Node->nodeValue;
                    }
                }

                // Check and fill arg2
                if ($instructionNode->hasChildNodes()) {
                    $arg2Node = $instructionNode->getElementsByTagName('arg2')->item(0);
                    if ($arg2Node) {
                        $instructionDetails["arg2_type"] = $arg2Node->getAttribute('type');
                        $instructionDetails["arg2_value"] = $arg2Node->nodeValue;
                    }
                }

                // Check and fill arg3
                if ($instructionNode->hasChildNodes()) {
                    $arg3Node = $instructionNode->getElementsByTagName('arg3')->item(0);
                    if ($arg3Node) {
                        $instructionDetails["arg3_type"] = $arg3Node->getAttribute('type');
                        $instructionDetails["arg3_value"] = $arg3Node->nodeValue;
                    }
                }
            }
            else
            {
                throw new XMLBadStructException;
            }

            $instructionAssociativeArray[] = $instructionDetails;
        }

        //Sort by order
        usort($instructionAssociativeArray, function($a, $b) {
            return $a['order'] <=> $b['order'];
        });

    
        return $instructionAssociativeArray;
    }

    
    //Method for creating associative array
    private function create_associative_array(){
        // Define an empty associative array
        $assoc_array = array();
    
        $assoc_array["order"]       = null;
        $assoc_array["opcode"]      = null;
        $assoc_array["arg1_type"]   = null;
        $assoc_array["arg1_value"]  = null;
        $assoc_array["arg2_type"]   = null;
        $assoc_array["arg2_value"]  = null;
        $assoc_array["arg3_type"]   = null;
        $assoc_array["arg3_value"]  = null;
    
        return $assoc_array;
    }


    //Method for converting input XML format to an array
    public function get_instruction_array($dom)
    {
        $parsedInstructions = [];

        $this->xml_header_check($dom);
        $parsedInstructions = $this->xml_instruction_order($dom);

        return  $parsedInstructions;
            
    }
}
