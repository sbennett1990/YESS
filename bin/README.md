####This folder contains two utility programs.

YAS
===

The Y86 assembler. This takes a Y86 assembly code file with extension .ys and generates a file with
extension .yo. The generated file contains an ASCII version of the object code.

YIS
===

The Y86 instruction simulator. This program executes the instructions in a Y86 machine-level program according to the instrcution set definition. For example, suppose you want to run the program asum.yo. Simply run:

unix> ./yis asum.yo

YIS simulates the execution of the program and then prints changes to any registers or memory locations on the terminal. This is very similar to running the YESS program and is provided to verify that YESS is working correctly.
