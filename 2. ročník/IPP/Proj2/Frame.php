<?php

namespace IPP\Student;



class Program_frames{
    public $global_frame;
    public $local_frame;
    public $temporal_frame;
    public $data_frame;

    


    public function __construct(){
        $this->global_frame   = new Global_Frame;
        $this->local_frame    = new Local_Frame;
        $this->temporal_frame = new Temporal_Frame;
        $this->data_frame     = new Data_Frame;
    }

    public function store_to_frame($name, $type, $value, $target_frame){
        // print "Storing " .$name. " to " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                $this->global_frame->add_variable_to_frame($name, $type, $value);
                break;
            case 'LF':
                $this->local_frame->add_variable_to_frame($name, $type, $value);
                break;
            case 'TF':
                $this->temporal_frame->check_access();
                $this->temporal_frame->add_variable_to_frame($name, $type, $value);
                break;
        }
        // print_r($this->global_frame);
    }

    public function find_in_frame($name, $target_frame){
        switch ($target_frame){
            case 'GF':
                return $this->global_frame->find_variable_in_frame($name);
            case 'LF':
                return $this->local_frame->find_variable_in_frame($name);
            case 'TF':
                return $this->temporal_frame->find_variable_in_frame($name);
        }
    }

    //SET
    public function update_variable_value($name, $new_value,$new_type, $target_frame){
        // print "Updating " .$name. " to new value of " .$new_value. " at frame " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                $this->global_frame->change_variable_value_at_frame($name, $new_value,$new_type);
                break;
            case 'LF':
                $this->local_frame->change_variable_value_at_frame($name, $new_value,$new_type);
                break;
            case 'TF':
                $this->temporal_frame->check_access();
                $this->temporal_frame->change_variable_value_at_frame($name, $new_value,$new_type);
                break;
        }
    }

    public function update_variable_value_pops($name, $new_value,$new_type, $target_frame){
        // print "Updating " .$name. " to new value of " .$new_value. " at frame " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                $this->global_frame->change_variable_value_at_frame_by_pops($name, $new_value,$new_type);
                break;
            case 'LF':
                $this->local_frame->change_variable_value_at_frame_by_pops($name, $new_value,$new_type);
                break;
            case 'TF':
                $this->temporal_frame->check_access();
                $this->temporal_frame->change_variable_value_at_frame_by_pops($name, $new_value,$new_type);
                break;
        }
    }

    

    public function update_variable_type($name, $new_value, $target_frame){
        // print "Updating " .$name. " to new type of " .$new_value. " at frame " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                $this->global_frame->change_variable_type_at_frame($name, $new_value);
                break;
            case 'LF':
                $this->local_frame->change_variable_type_at_frame($name, $new_value);
                break;
            case 'TF':
                $this->temporal_frame->check_access();
                $this->temporal_frame->change_variable_type_at_frame($name, $new_value);
                break;
        }
    }


    //GET
    public function get_variable_value($name, $target_frame){
        // print "Getting value of " .$name. " from from " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                return $this->global_frame->get_variable_value_from_frame($name);
            case 'LF':
                return $this->local_frame->get_variable_value_from_frame($name);
            case 'TF':
                $this->temporal_frame->check_access();
                return $this->temporal_frame->get_variable_value_from_frame($name);
        }
    }

    public function get_variable_type($name, $target_frame){
        // print "Getting value of " .$name. " from from " .$target_frame. "\n";
        switch ($target_frame){
            case 'GF':
                return $this->global_frame->get_variable_type_from_frame($name);
            case 'LF':
                return $this->local_frame->get_variable_type_from_frame($name);
            case 'TF':
                $this->temporal_frame->check_access();
                return $this->temporal_frame->get_variable_type_from_frame($name);
        }
    }

    //Temporal frame
    // public function create_new_temporal_frame(){
    //     $this->temporal_frame->define_temporal_frame();
    // }

    //Local frame
    //Push temporal frame to Local Frame
    public function push_temporal_frame_to_LF($frames){
        $this->local_frame->add_new_local_frame($frames);
        // print "Pushing TF to LF\n";
        // print_r($this->local_frame);
    }

    public function pop_local_frame($frames){
        $this->local_frame->delete_top_local_frame($frames);
        // print "POPing LF\n";
    }

}

class Global_Frame{

     //Array for storing global variables
     protected $frame_array = []; 

    public function __construct()
    {
        
    }

    public function number_of_variables(){
        return count($this->frame_array);
    }

    public function get_variable_value_from_frame($name){
        $variable_node = $this->find_variable_in_frame($name);
        //First we have to check, if variable is defined
        if($variable_node == null){
            throw new SemanticErrorNotDefinedException;   //Variable is not defined
        }
        
        // print_r($variable_node);
        return $variable_node ["value"];
    }

    public function get_variable_type_from_frame($name){
        $variable_node = $this->find_variable_in_frame($name);
        //First we have to check, if variable is defined
        if($variable_node == null){
            throw new SemanticErrorNotDefinedException;   //Variable is not defined
        }
        return $variable_node ["type"];
    }

    public function change_variable_value_at_frame($name, $new_value, $new_type){
        //First we have to check, if variable is defined
        if($this->find_variable_in_frame($name) == null){
            throw new SemanticErrorNotDefinedException;   //Variable is not defined
        }
        

        // Update the variable in the frame_array
        foreach ($this->frame_array as &$array_record) {
            if ($array_record["name"] == $name) {
                //We have to check if types are compatible
                //Variable can be without a type, then type == var or must have the same type
                if($array_record["type"] != "var" && $array_record["type"] != $new_type){
                    // print "Type variable is: " .$array_record["type"]. " type to store is " .$new_type. "\n";
                    throw new SemanticErrorWrongOperandException;
                }
                $array_record["type"] = $new_type;
                $array_record["value"] = $new_value;
                break;
            }
        }
    }

    //Simplified change_variable_value_at_frame without type control
    public function change_variable_value_at_frame_by_pops($name, $new_value, $new_type){
        //First we have to check, if variable is defined
        if($this->find_variable_in_frame($name) == null){
            throw new SemanticErrorNotDefinedException;   //Variable is not defined
        }
        // Update the variable in the frame_array
        foreach ($this->frame_array as &$array_record) {
            if ($array_record["name"] == $name) {
                $array_record["type"] = $new_type;
                $array_record["value"] = $new_value;
                break;
            }
        }
    }

    //Not using
    public function change_variable_type_at_frame($name, $new_type){
        //First we have to check, if variable is defined
        if($this->find_variable_in_frame($name) == null){
            throw new SemanticErrorNotDefinedException;   //Variable is not defined
        }
        // Update the variable in the frame_array
        foreach ($this->frame_array as &$array_record) {
            if ($array_record["name"] == $name) {
                //Variable can be without a type, then type == var or must have the same type
                if($array_record["type"] != "var" && $array_record["type"] != $new_type){
                    throw new SemanticErrorWrongOperandException;
                }
                $array_record["type"] = $new_type;
                break;
            }
        }
    }

    public function add_variable_to_frame($name, $type, $value)
    {
        //Fist check if the variable is not defined
        $is_defined = $this->find_variable_in_frame($name);
        if ($is_defined != null){
            throw new SemanticErrorRedefinitionException;    //Variable is already defined, exit code 52
        }
        
        $variable = $this->create_associative_array();

        $variable ["name"] = $name;
        $variable ["type"] = $type;
        $variable ["value"] = $value;

        $this->frame_array[] = $variable;
    }

    //Method returns array node PRIVATE?
    public function find_variable_in_frame($name)
    {
        
        foreach ($this->frame_array as $variable) {
            if ($variable["name"] == $name) {
                return $variable; // Return the variable if found
            }
        }
    
        return null;      
    }


    //Method for creating associative array 
    protected function create_associative_array() {
        // Define an empty associative array
        $var_array = array();
    
        $var_array["name"]      = null;
        $var_array["type"]      = null;
        $var_array["value"]     = null;
    
        return $var_array;
    }
}

class Local_Frame extends Global_Frame{
    public $pointer_to_actual_local_frame;
    public $array_of_local_frames;
    //Override
    public function __construct()
    {
        $this->array_of_local_frames = array();
        $this->pointer_to_actual_local_frame = -1;
    }

    //OVerride
    public function find_variable_in_frame($name)
    {
        
        foreach ($this->array_of_local_frames[$this->pointer_to_actual_local_frame] as $variable) {
            if ($variable["name"] == $name) {
                return $variable; // Return the variable if found
            }
        }
    
        return null;      
    }

    public function add_new_local_frame($frames){
        $this->pointer_to_actual_local_frame++;   //Increment number of local frames

        array_push($this->array_of_local_frames, $frames->temporal_frame->get_temporal_frame() );

        // print_r($this->array_of_local_frames);

    }

    
    public function delete_top_local_frame($frames){
        // print $this->pointer_to_actual_local_frame;
        if($this->pointer_to_actual_local_frame < 0){
            throw new FrameAccessErrorException;
        }
        $frames->temporal_frame->set_temporal_frame($this->array_of_local_frames[ $this->pointer_to_actual_local_frame]);

        $this->pointer_to_actual_local_frame--;   //Decrement number of local frames
    }

}

class Temporal_Frame extends Global_Frame{
    public $is_defined;

    //Override
    public function __construct()
    {
        $this->is_defined = false;
    }



    public function check_access() : void {
        //Fist check if TF defined
        if($this->is_defined == false){
            throw new FrameAccessErrorException;
        }
    }
   
    //
    public function define_temporal_frame(){
        // print "Defining TMP frame \n";
       $this->frame_array = [];
       $this->is_defined = true;
    }

    //This method will set $is_defined to false, used when pushing to LF
    public function get_temporal_frame(){
        //Check if TF is defines
        if ($this->is_defined == false){
            throw new FrameAccessErrorException;
        }
        $this->is_defined = false;
        return $this->frame_array;
    }

    public function set_temporal_frame($given_local_frame){
        $this->is_defined = true;
        $this->frame_array = $given_local_frame;
    }
        
        
}

class Data_Frame {
    protected $data_frame;
    protected $data_frame_index;
    protected $value;


    public function __construct(){
        $this->data_frame = [];
        $this->value = [];
        $this->data_frame_index = -1;
    }

    public function push_to_data_frame ($value_to_push, $type_to_push) : void{
        $this->value["value"] = $value_to_push;
        $this->value["type"] = $type_to_push;
        $this->data_frame[] = $this->value; 
        $this->data_frame_index++;
    }

    public function pop_from_data_frame (){
        if ($this->data_frame_index < 0){
            throw new EmptyDataStackException;
        }

        $return_value =  $this->data_frame[$this->data_frame_index];
        $this->data_frame_index--;
        return $return_value;
    }
}