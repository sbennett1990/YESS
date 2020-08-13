YESS(1) - General Commands Manual

# NAME

**yess** - y86 simulator

# SYNOPSIS

**yess**
\[**-duv**]
**-f**&nbsp;*prog.yo*

# DESCRIPTION

The
**yess**
program simulates execution of the y86 instruction set.
y86 is a theoretical teaching assembly language, which embodies elements
of RISC and CISC archetectures.

**yess**
executes y86 object
("machine")
code.
Object code must be placed in a file ending with a
*.yo*
extension.
This program file is a regular ASCII file, not a binary.
The y86 assembler can produce ASCII object code required by
**yess**.

The options are as follows:

**-d**

> Produce debug messages.
> **yess**
> will write debug information to stderr.

**-f** *prog.yo*

> The program file to execute.

**-u**

> Print usage information and exit.

**-v**

> Verbose mode.
> **yess**
> will write more information to stderr (but not as much as
> **-d**
> mode).

# BUILD

Making the obj directory first will keep the
*src*
directory clean:

	$ cd src
	$ make obj && make

The
**yess**
executable is now at
*obj/yess*.

Optionally install to
*~/bin*:

	$ make install

# SEE ALSO

y86\_obj\_code(7)

Randal E. Bryant,
David R. O'Hallaron,
*CS:APP2e Guide to Y86 Processor Simulators*,
[https://web.archive.org/web/20150330120944/http://csapp.cs.cmu.edu/public/simguide.pdf](https://web.archive.org/web/20150330120944/http://csapp.cs.cmu.edu/public/simguide.pdf),  
July 29, 2013.

# AUTHORS

This program was written by
Scott Bennett
and
Alex Svarda.

# CAVEATS

**yess**
only builds and works on
OpenBSD 6.6
or newer because it relies on the built-in
make(1)
infrastructure.
Portability could be achieved with a little effort, but that is outside the
scope of my current goals.

OpenBSD 6.7 - August 13, 2020
