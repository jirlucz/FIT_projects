#!/bin/bash

make && 
./TEST &&
python3 "/home/dominik/.vscode/extensions/okrejci.ippc-ifjc-0.2.5/debug/ifjc23interpreter.py" --source "/home/dominik/Desktop/Skola/IFJ/ifj-projekt-repository/GeneratorTesty/IFJCode23.code"