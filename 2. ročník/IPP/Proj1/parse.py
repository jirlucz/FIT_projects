# Analyzator IPP 2024
# Jiri Kotek xkotek09
# 2BIT 2024


import sys
import re
import xml.etree.ElementTree as ET
from xml.dom import minidom 


#Function checks arguments recived from terminal
def argcheck():
    if (len(sys.argv) >= 1):
        #print(len(sys.argv))
        if (len(sys.argv) == 2 and (sys.argv[1] == "--help")):
            print("Usege:   \"$python3.10 parse.py <input\"")
            print(" Script will parse data from input into XML format on STDOUT")
            sys.exit(0)
        elif (len(sys.argv) == 1):
            pass                #Just a script itself
        else:
            sys.exit(10)        #Script is called with illegal arg combination
            
#Function delete all comments in input string (all comments starts with '#' and end at nearest '\n')
def deleteComments (input):
    return re.sub(r'#.*?(\n|$)', r'\1',input)

#Function checks if input file starts with header ".IPPcode2024" (case-insensitive)
#Header must be on the first position of the document (Whitespaces, tabs, \n are leading to 21 err)
def IPPheadCheck (input: str):
    input = input.casefold()
    
    splittedInput = input.splitlines()
    
    input = re.sub(r'\s+(?=\S)', "", input, 1)     #REG to delete all whitespace to the nearest char
    

    noHeaders = 0
    
    
    for line in splittedInput:
        
        if(line.find(".ippcode24") != -1):
            #print(line)
            noHeaders = noHeaders + 1
            
    #print(noHeaders)
        
    if ((input.find(".ippcode24")) != 0):
        sys.exit(21)
    else:
        if((input.index(".ippcode24")) != 0):
            sys.exit(21)
            
    if (noHeaders != 1):
        #print("Chyba hlavicky")
        sys.exit(23)
            
#Function deletes header and whitespaces
def removeHeader(input):
    pattern = re.compile(re.escape(".ippcode24"), re.IGNORECASE)
    return pattern.sub("",input)

#Function removes all empty lines
def removeEmptyLines(input):
    return re.sub('^[ \t]*$\r?\n', "", input, flags=re.MULTILINE)

#Help function for general processing user input
def processInput(input):
    
    input = deleteComments(input)
    
    findheader = input
    
    IPPheadCheck(findheader)
    
    input = removeHeader(input)
    
    return removeEmptyLines(input)

def standardiseSpaces(input):
    string = re.sub(r'\s{2,}', " ", input)  #Replaces multiple whitespaces with single space
    return re.sub(r'\s+$', "", string)      #Removes whitespaces after last char

def createXmlFile(input):
    pass
    


def noParam(instructionLine: str, root, iteration: int):
    #print (len(instructionLine))
    if (len(instructionLine) != 1):
        #print(instructionLine)
        sys.exit(23)
    else:
        instruction = ET.SubElement(root,'instruction')
        instruction.set('order', str(iteration))
        instruction.set('opcode', instructionLine[0].upper())

#If typecarriage is empty it means we are not working with syml and type will be always var
#If we are working with symb, type is in type parameter and in typecarriage is name
def SingleParam(instructionLine: str, root, iteration: int, type: str, typecarriage = ''):
    xmlInstruction = ET.SubElement(root, 'instruction' )
    xmlInstruction.set('order', str(iteration))
    xmlInstruction.set('opcode', instructionLine[0].upper())
    
    xmlArg1 = ET.SubElement(xmlInstruction,'arg1')
    if (typecarriage == ''):
        xmlArg1.set('type', type)
        xmlArg1.text = instructionLine[1]
    else:
        xmlArg1.set('type', type)
        xmlArg1.text = typecarriage
       
# READ  GF@a  |int
#       1.arg |2.arg     
#   1.arg is always type = <var> && typecarriage = instructionLine[1] (Here: 'GF@a')
#   2.arg can be:
#                   -> <type>: type = 'var'   &&  typecarriage = 'typecarriage'
#                   -> <symb>: type = 'type'  &&  typecarriage = 'typecarriage'        
#                                            type (f.g. type,..)|| (int, ...)
#                                                             |            |    
def TwoParam(instructionLine: str, root, iteration: int, type: str, typecarriage = ''):
    xmlInstruction = ET.SubElement(root, 'instruction' )
    xmlInstruction.set('order', str(iteration))
    xmlInstruction.set('opcode', instructionLine[0].upper())
    
    #Arg1 is always VAR
    xmlArg1 = ET.SubElement(xmlInstruction,'arg1')
    xmlArg1.set('type', 'var')
    xmlArg1.text = instructionLine[1]
    
    if (typecarriage == '' and type != 'string'):
        xmlArg2 = ET.SubElement(xmlInstruction,'arg2')
        xmlArg2.set('type', 'type')
        xmlArg2.text = instructionLine[2]
    else:
        xmlArg2 = ET.SubElement(xmlInstruction,'arg2')
        xmlArg2.set('type', type)
        xmlArg2.text = typecarriage
        
#
#
#
#
#
# 
# 
# 
def ThreeParam(instructionLine: str, root, iteration: int, firstType: str, firstTypecarriage: str, secondType: str, secondTypecarriage: str):
    xmlInstruction = ET.SubElement(root, 'instruction' )
    xmlInstruction.set('order', str(iteration))
    xmlInstruction.set('opcode', instructionLine[0].upper())
    
    if( instructionLine[0].upper() == 'JUMPIFEQ' or instructionLine[0].upper() == 'JUMPIFNEQ'):
        xmlArg1 = ET.SubElement(xmlInstruction,'arg1')
        xmlArg1.set('type', 'label')
        xmlArg1.text = instructionLine[1]
    else:
        xmlArg1 = ET.SubElement(xmlInstruction,'arg1')
        xmlArg1.set('type', 'var')
        xmlArg1.text = instructionLine[1]
    
    xmlArg2 = ET.SubElement(xmlInstruction,'arg2')
    xmlArg2.set('type', firstType)
    xmlArg2.text = firstTypecarriage
    
    xmlArg3 = ET.SubElement(xmlInstruction,'arg3')
    xmlArg3.set('type', secondType)
    xmlArg3.text = secondTypecarriage
        

def processVar(instructionLine: str, index: int):
    if (bool(re.fullmatch(r'(?:LF|TF|GF)@[a-zA-Z_\-$&%*!?][\w$&%*!?\-]*', instructionLine[index]))):
        #instructionLine[1] = xmlCharReplacement(instructionLine[index]) #Replacement of '&' -> '&amp;'
        pass
    else:
        #print("chyba Var")
        #print(instructionLine)
        sys.exit(23)


def processLabel(instructionLine: str, index: int):
    if (bool(re.fullmatch(r'[a-zA-Z_\-$&%*!?][\w_$\-&%*!?-]*', instructionLine[index]))):
        #instructionLine[1] = xmlCharReplacement(instructionLine[index]) #Replacement of '&' -> '&amp;'
        pass
    else:
        #print("chyba Label")
        #print(instructionLine)
        sys.exit(23)


def processSymb(instructionLine: str, index: int):
    if (bool(re.fullmatch(r'(?:LF|TF|GF|nil|bool|int|string|label|type|var)@.*', instructionLine[index]))):     #At first we check first half
        splittedInstr = instructionLine[index].split("@", 1)
        #print('ydeeeeeeeeeydeeeeeeeeeeeeeeeeeeeeeeeeeee')
        #print(splittedInstr)
        if (splittedInstr[0] == "GF" or splittedInstr[0] == "TF" or splittedInstr[0] == "LF"):
            splittedInstr[1] = splittedInstr[0]+'@'+splittedInstr[1]
            splittedInstr[0] = 'var'
        #splittedInstr[0] = LEFT side (GF, string, ...)
        #splittedInstr[1] = RIGHT side (identificators, string...)
        if splittedInstr[0] == "string":
            #Lex check for escape secventions
            escapeSecCheck(splittedInstr[1])
            #splittedInstr[1] = xmlCharReplacement(splittedInstr[1])
            #print(splittedInstr[1])
        return splittedInstr
            
    else:
        #print(instructionLine)
        sys.exit(23)
        
def processType(instructionLine: str, index: int):
    if (bool(re.fullmatch(r'(nil|bool|int|string|label|type|var)', instructionLine[index]))):
       # print("shoda Typu")
       pass
    else:
        #print("chyba Typu")
        sys.exit(23)
        

def escapeSecCheck(input: str):
    i = 0
    while i < len (input):
        if input[i] == '\\':
            if i+4 > len(input):
                #print("Chyba delky escape sekvence")
                #print(input)
                sys.exit(23)
            else:
                digitPart = input[i+1:i+4]
                #print(digitPart)
                if digitPart.isdigit() :
                        i = i+1
                        continue
                else:
                    #print("Chyba escape sekvence neobsahuje pouze cisla")
                    #print(input)
                    sys.exit(23)
        else:
            i = i+1
         
"""""   
def xmlCharReplacement(input: str):
    output = input.replace('&','&amp;')
    output = output.replace('<', '&al;')
    output = output.replace('>', '&gt;')
    return output
"""

def lenCheck (instructionLine, length: int):
    if (len(instructionLine) != length):
        #print(instructionLine)
        sys.exit(23)
                    


### Body of main function ###
def main():
    
    argcheck()
    
    input_string = sys.stdin.read()
    
    if (input_string == ''):
        #print("prazdny vstup")
        sys.exit(21)
    
    processedInput = processInput(input_string)
    
    #print(processedInput)
    iteration = 1
    #Create XML 
    root = ET.Element('program', language='IPPcode24')
    

    tree = ET.ElementTree(root)
    
    
    
    
    
    #Split input into each lines
    input_lines = processedInput.splitlines()
    #Calculates instruction order
    order = 0
    
    for line in input_lines:
        
        standardisedLine = standardiseSpaces(line)
        instructionPart = standardisedLine.split(" ")
        order = order+1

        #print(instructionPart)
        #print(instructionPart[0])
        
        #   Instruction participation:
        #   [INSTRUCTION NAME] [1ST ARGUMENT] [2ND ARGUMENT] [3RD ARGUMENT]
        #          |                 |               |             |
        #         [0]               [1]             [2]           [3]
        

        match (instructionPart[0].casefold()):
            #memory frames, function call
            case "move":
                lenCheck(instructionPart, 3)
                processVar(instructionPart, 1)
                splittedInstruction = processSymb(instructionPart, 2)
                #print(splittedInstruction)
                TwoParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            case "createframe":
                noParam(instructionPart, root, order)
                 
            case "pushframe":
                noParam(instructionPart, root, order)
            case "popframe":
                noParam(instructionPart, root, order)
            case "defvar":
                lenCheck(instructionPart, 2)
                processVar(instructionPart, 1)
                SingleParam(instructionPart, root, order, 'var')
            case "call":
                lenCheck(instructionPart, 2)
                processLabel(instructionPart, 1)
                SingleParam(instructionPart, root, order, 'label')
            case "return":
                noParam(instructionPart, root, order)
            
            #data stack
            case "pushs":
                lenCheck(instructionPart, 2)
                splittedInstruction = processSymb(instructionPart, 1)
                SingleParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            case "pops":
                lenCheck(instructionPart, 2)
                processVar(instructionPart, 1)
                SingleParam(instructionPart, root, order, 'var')
            
            #arithmetic, relation, bool and conversion
            case "add":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "sub":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "mul":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "idiv":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "lt":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "gt":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "eq":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "and":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "or":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "not":
                lenCheck(instructionPart, 3)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                TwoParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1])
            case "int2char":
                 lenCheck(instructionPart, 3)
                 processVar(instructionPart, 1)
                 splittedInstruction = processSymb(instructionPart, 2)
                 TwoParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            case "stri2int":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            
            #IO instructions
            case "read":
                 lenCheck(instructionPart, 3)
                 processVar(instructionPart, 1)
                 processType(instructionPart, 2)
                 TwoParam(instructionPart, root, order, 'type')
            case "write":
                lenCheck(instructionPart, 2)
                splittedInstruction = processSymb(instructionPart, 1)
                SingleParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            
            #String instructions
            case "concat":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "strlen":
                 lenCheck(instructionPart, 3)
                 processVar(instructionPart, 1)
                 splittedInstruction = processSymb(instructionPart, 2)
                 TwoParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            case "getchat":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "setchat":
                lenCheck(instructionPart, 4)
                processVar(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            
            #type instructions
            case "type":
                 lenCheck(instructionPart, 3)
                 processVar(instructionPart, 1)
                 splittedInstruction = processSymb(instructionPart, 2)
                 TwoParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            
            #flow control instructions
            case "label":
                lenCheck(instructionPart, 2)
                processLabel(instructionPart, 1)
                SingleParam(instructionPart, root, order, 'label')
            case "jump":
                lenCheck(instructionPart, 2)
                processLabel(instructionPart, 1)
                SingleParam(instructionPart, root, order, 'label')
            case "jumpifeq":
                lenCheck(instructionPart, 4)
                processLabel(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "jumpifneq":
                lenCheck(instructionPart, 4)
                processLabel(instructionPart, 1)
                splittedInstruction1 = processSymb(instructionPart, 2)
                splittedInstruction2 = processSymb(instructionPart, 3)
                ThreeParam(instructionPart, root, order, splittedInstruction1[0], splittedInstruction1[1], splittedInstruction2[0], splittedInstruction2[1])
            case "exit":
                lenCheck(instructionPart, 2)
                splittedInstruction = processSymb(instructionPart, 1)
                SingleParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            
            #debug
            case "dprint":
                lenCheck(instructionPart, 2)
                splittedInstruction = processSymb(instructionPart, 1)
                SingleParam(instructionPart, root, order, splittedInstruction[0], splittedInstruction[1])
            case "break":
                noParam(instructionPart, root, order)
            
            case _:
                sys.exit(22)
                
     # Write XML to stdout
    ET.indent(tree)
    print('<?xml version="1.0" encoding="UTF-8"?>')
    print(ET.tostring(tree.getroot(), encoding='UTF-8', xml_declaration=False).decode())
         

    sys.exit(0)

if __name__ == "__main__":
    main()






