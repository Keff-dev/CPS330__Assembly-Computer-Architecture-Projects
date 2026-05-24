# CPS330-Assembly-Computer-Architecture-Projects
In Spring 2026, I took an assembly class called "Assembly Language and Computer Architecture", this repository holds the projects done during that semester.

There were 3 main projects, used to simulate what a language processor does. The i1.c is the interpreter, the a1.c is the assembler, and the l.c file holds the assembly file "linker".


### i1 File
The interpreter converts the machine code into the different functions of assembly by using specialised codes to match operations. In this file, registers and a memory array can be actively seen to simulate the architecture of a CPU in C. This was the first project due in the course.

### a1 File
The assembler converts an assembly file to machine code, using the same codes as the interpreter for different operations. This file makes use of various bitwise functions (AND/ORing binary numbers, bit-shifting, etc.) to create the machine code that the interpreter could read. This was the second project of the course.

### l File
The linker allows different assembly files to be compiled into a larger file in the similar way that one might compartmentalize tasks to separate files and reference them in a main file for languages like Java or C. In other words, it is essentially the compiler. This program takes in different object files created by individual assembly files and compiles them into one large machine code file. This was the final project done in the course.

### Labs Folder
Essentially how it sounds. Each week the students were assigned a lab with various tasks to complete, relating to learning C and Assembly. The Labs folder contain a few examples of the assignments done over the semester.
