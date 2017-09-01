# YESS

### A y86 (Assembly Code) Simulator

Simulates a "Y86" processor. The program simulates the execution of y86 machine code,
which is more or less a subset of x86. Being a teaching language, y86 embodies elements
of RISC and CISC archetectures.

#### Assembly

y86 is a theoretical assembly language. However, the YESS program simulates
y86 _machine_ code. In the bin folder there is an assembler, `yas`, that "assembles"
y86 source code into the required machine code (just ASCII text), which has the
extension `.yo`.

#### Use

To assemble y86 source:
> $ ./yas \<filename\>.ys

To simulate y86 machine code:
> $ ./yess \<filename\>.yo

#### References

See the CS:APP2e website for more information on Y86 and running the simulator.
It includes a lot of useful information on the design of a y86 processor. There
are also reference tools and programs, including an assembler.

https://web.archive.org/web/20141228221440/http://csapp.cs.cmu.edu/public/students.html

The PDF simulator guide also has great information, although not all of it
is relavent to YESS. YESS is more closely related to the PIPE machine that
is mentioned.

https://web.archive.org/web/20141228221440/http://csapp.cs.cmu.edu/public/simguide.pdf
