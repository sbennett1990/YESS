YESS
====

A y86 (Assembly Code) Simulator

Simulates a "Y86" processor. It will run y86 assembly code, which is more or
less a subset of x86. Y86 embodies elements of RISC and CISC archetectures.


###Use:

unix> ./yess \<filename\>.yo

###Assembly:

y86 is a theoretical assembly language. However, the YESS program simulates
y86 machine code. In the bin folder there is a tool that "assembles" y86
source code into the required machine code, which has a .yo extension. This 
is the y86 assembler: yas.

###References:

See the CS:APP2e website for more information on Y86 and running the simulator.
It includes a lot of useful information on the design of a y86 processor. There 
are also reference tools and programs, including an assembler.

http://csapp.cs.cmu.edu/public/students.html

The PDF simulator guide also has great information, although not all of it
is relavent to YESS. YESS is more closely related to the PIPE machine that 
is mentioned.

http://csapp.cs.cmu.edu/public/simguide.pdf
