# The ASM16 System Reference Manual

----------
# Contents

- Overview
- Registers
- Interrupts
- Memory map
- Peripherals
    - High precision Timers
    - Video Processing Unit (VPU)
    - Audio Processing Unit (APU)
    - Input controller
- Instruction Format
- Instruction Set Reference
    - Memory access
    - Arithmetic and logic
    - Single operand instructions
    - Zero operand instructions

----------
# Overview

The ASM16 System is an imaginary **16 bit** computer system that could have existed in the **late 1980s**.  Since the ASM16 system does not have to execute on real hardware a system could be designed that is highly accessable to programmers.  Many of the non fun details of real hardware were also able to be skipped, such as PAL/NTSC timings, etc.

ASM16 stands for **'A Simple Machine'**, and the designator 16 signifies that it has a 16 bits address and data bus.  The system follows the 'load store' aproach to RISC processor design, allowing memory access only via specific instructions.  ASM16 also uses a 'Von Neumann' style of computer architecture as RAM may contain data or code at any location and makes no distinctions between them.

The ASM16 processor is clocked at a **nippy 1Mhz**, and has a whopping **64kb of RAM** for program and data.
A dedicated external video processing unit handles the complexity of video output and has a farther 64kb of **fast video RAM** at its desposal and can output outputing a staggering **256 colours**.  The ASM16 system comes complete with **high fidelity audio** courtesy of a 3 channel synth chip with onboard noise generator.

Like the 8086, ASM16 is a little endian machine and thus all WORD values should be stored with the most significant byte first.

----------
# Registers

The ASM16 CPU has 16 internal registers, with each register being 16 bits in size (WORD).  Registers R0 to R3 have specific roles during the operation of the chip and their use is somewhat restricted.  Registers R04 to R15 have no specific purpose and can be used freely for general computation.

| Register | Alias | Description      |
|----------|-------|------------------|
| R0       | ZR    | Zero Register    |
| R1       | PC    | Program Counter  |
| R2       | SP    | Stack Pointer    |
| R3       | CR    | Control Register |
| R4       |       | General Purpose  |
| R5       |       | "                |
| R6       |       | "                |
| R7       |       | "                |
| R8       |       | "                |
| R9       |       | "                |
| R10      |       | "                |
| R11      |       | "                |
| R12      |       | "                |
| R13      |       | "                |
| R14      |       | "                |
| R15      |       | "                |

#### Zero Register (ZR):
All reads from register R0 (ZR) will return the value 0.  Writes to this register will have no effect.

#### Program Counter (PC):
Every clock cycle the ASM16 CPU will fetch an instruction from main memory at the address contained in R1 (PC). After the execution of each instruction the PC register will be incremented by the instructions size.  The PC will not advance however if the instruction branches (JMP, CALL, RET, etc).

#### Stack Pointer (SP):
The ASM16 CPU has hardware support for a stack via register R2 (SP).  The stack operates with 16 bit values only.  The stack is said to grow downward, as stack push operations decrement SP by two.  Pop operations increment the stack by two.

#### Control Register (CR):
The R03 (CR) is the control register which can enable or disable certain modes of operation and on chip peripherals. 
    
    Layout: 0000 0000 0000 000i
    
    i   0: interrupts disabled
        1: interrupts enabled

----------
# Interrupts

TODO
----------
# Memory Map

    [General RAM]
        0x0000
        0x....
        0x7fff
    
    [VPU]
        0x8000    - layer 1
        0x....
        0x9000	- layer 2
        0x....
        0xa000	- layer 3
        0x....
        0xb000	- sprites
        
    [APU]
    	0xf000	- Tone 0 Freq
    	0xf002	- Tone 0 Attenuation
    	0xf004	- Tone 1 Freq
    	0xf008	- Tone 1 Attenuation
    	0xf004	- Tone 2 Freq
    	0xf006	- Tone2 Attenuation
    
    [Timers]
        0xf100  - Timer control register
    	0xf102	- Timer 0 threshold
    	0xf104  - Timer 1 threshold
    
    [Input Controller]
        0xf200  - Keyboard scancode
    
    [Interrupts]
        0xff80	- Reset
        0xff82	- Input
        0xff84	- Horizontal Blank
        0xff86	- Vertical Blank
        0xff88	- Timer 0
        0xff8a	- Timer 1

----------
# Peripherals

TODO

### High precision Timers

TODO

### The Video Processing Unit (VPU)

TODO

### The Audio Processing Unit (APU)

TODO

### Input Controller

TODO

----------
# Instruction Format

All ASM16 instructions are either two bytes or four bytes in length. The Immediate field, if present is stored in little endian format.

    2 byte encoding:
    	[cccc cccc] [yyyy xxxx]

	4 byte encoding:
		[cccc cccc] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Where:
	c = Opcode specifier
	x = Rx register index
	y = Ry register index
	I = 16 bit immediate value

The following table shows the derivation of .  Each mnemonic is followed by a listing of the operands used.  The ASM16 CPU has 45 instructions in total.

| 0xcc | 0x0_     | 0x1_   | 0x2_   | 0x3_   | 0x4_     | 0x5_ |
|------|----------|--------|--------|--------|----------|------|
| 0x_1 | LDW  IYX | ADD YX | ADD IX | PUSH X | JMP  I   | RET  |
| 0x_2 | LDB  IYX | SUB YX | SUB IX | POP  X | JNE  YXI |      |
| 0x_2 | LDW+ IYX | MUL YX | MUL IX |        | JEQ  YXI |      |
| 0x_3 | LDB+ IYX | MLH YX | MLH IX |        | JL   YXI |      |
| 0x_4 | STW  XIY | DIV YX | DIV IX |        | JG   YXI |      |
| 0x_5 | STB  XIY | MOD YX | MOD IX |        | JLE  YXI |      |
| 0x_6 | STW+ XIY | SHL YX | SHL IX |        | JGE  YXI |      |
| 0x_7 | STB+ XIY | SHR YX | SHR IX |        | CALL I   |      |
| 0x_8 |          | AND YX | AND IX |        |          |      |
| 0x_9 |          | OR  YX | OR  IX |        |          |      |
| 0x_a |          | XOR YX | XOR IX |        |          |      |
| 0x_b |          | MOV YX | MOV IX |        |          |      |

----------
# Instructuion Reference

## Memory access

#### LDW
Load register Rx with word from memory address Imm+Ry.

	Format:
		LDW IMM RY RX
			 0x00
			[0000 0000] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Rx  = Memory[Imm + Ry]
		PC += 4

#### LDB 
Load register Rx with byte from memory address Imm+Ry.

	Format:
		LDW IMM RY RX
			 0x01
			[0000 0001] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Rx  = Memory[Imm + Ry] & 0x00ff
		PC +=4

#### STW 
Store a word from register Rx to memory address Imm+Ry.

	Format:
		STW RX IMM RY
			 0x04
			[0000 0100] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Memory[Imm + Ry] = Rx
		PC += 4

#### STB 
Store a byte from register to memory.

	Format:
		STB RX IMM RY
			 0x05
			[0000 0101] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Memory[Imm + Ry] = Rx & 0xff
		PC += 4
 
#### STW+ 
Store a word from register Rx to memory Imm+Ry then increment Ry by two.

	Format:
		STB RX IMM RY
			 0x06
			[0000 0110] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Memory[Imm + Ry] = Rx
		Ry += 2
		PC += 4

#### STB+
Store a byte from register to memory and increment the address.

	Format:
		STB RX IMM RY
			 0x07
			[0000 0111] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		Memory[Imm + Ry] = Rx & 0xff
		Ry += 1
		PC += 4

## Arithmetic and logic

#### ADD
Add a source value to destination register Rx.

	Format:
	a)	ADD RY RX
			 0x10
			[0001 0000] [yyyy xxxx]

	b)	ADD IMM RX
			 0x20
			[0010 0000] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx + Ry
		PC += 2
		
	b)	Rx  = Rx + Imm
		PC += 4

#### SUB
Subtract a source value from destination register Rx.

	Format:
	a)	SUB RY RX
			0x11
			[0001 0001] [yyyy xxxx]

	b)	SUB IMM RX
			0x21
			[0010 0001] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx – Ry
		PC += 2
	
	b)	Rx  = Rx – Imm
		PC += 4

#### MUL
Multiply two word values and place the low word product into destination register Rx.

	Format:
	a)	MUL RY RX
			 0x12
		 	[0001 0010] [yyyy xxxx]

	b)	MUL IMM RX
		 	 0x22
		 	[0010 0010] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = (Rx * Ry) & 0xffff
		PC += 2
	
	b)	Rx  = (Rx * Imm) & 0xffff
		PC += 4

#### MLH
Multiply two word values and place high word product into destination register Rx.

	Format:
	a)	MLH RY RX
			 0x13
			[0001 0011] [yyyy xxxx]

	b)	MLH IMM RX
			 0x23
			[0010 0011] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = (Rx * Ry ) >> 16
		PC += 2
	
	b)	Rx  = (Rx * Imm) >> 16
		PC += 4

#### DIV
Divide the destination register Rx by a source value.

	Format:
	a)	DIV RY RX
			 0x14
			[0001 0100] [yyyy xxxx]

	b)	DIV IMM RX
			 0x24
			[0010 0100] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx / Ry
		PC += 2
	
	b)	Rx  = Rx / Imm
		PC += 4

#### MOD
Place the remainder after division, Rx by source value, into destination register Rx.

	Format:
	a)	MOD RY RX
			 0x15
			[0001 0101] [yyyy xxxx]

	b)	MOD IMM RX
			 0x25
			[0010 0101] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx % Ry
		PC += 2
	
	b)	Rx  = Rx % Imm
		PC += 4

#### SHL
Arithmetic shift left the destination Register Rx.

	Format:
	a)	SHL RY RX
			 0x16
			[0001 0110] [yyyy xxxx]

	b)	SHL IMM RX
			 0x26
			[0010 0110] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx << Ry
		PC += 2
	
	b)	Rx  = Rx << Imm
		PC += 4

#### SHR
Arithmetic shift right the destination Register Rx.  All bits shifted in from the left match the old most significant bit.

	Format:
	a)	SHR RY RX
			 0x17
			[0001 0111] [yyyy xxxx]

	b)	SHR IMM RX
			 0x27
			[0010 0111] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx >> Ry
		PC += 2
	
	b)	Rx  = Rx >> Imm
		PC += 4

#### AND
Bitwise AND of the destination register Rx and a source value.

	Format:
	a)	AND RY RX
			 0x18
			[0001 1000] [yyyy xxxx]

	b)	AND IMM RX
			 0x28
			[0010 1000] [0000 xxxx]

	Operation:
	a)	Rx  = Rx & Ry
		PC += 2
	
	b)	Rx  = Rx & Imm
		PC += 4

#### OR
Bitwise or of the destination registers Rx and a source value.

	Format:
	a)	DIV RY RX
			 0x19
			[0001 1001] [yyyy xxxx]

	b)	DIV IMM RX
			 0x29
			[0010 1001] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx | Ry
		PC += 2
	
	b)	Rx  = Rx | Imm
		PC += 4

#### XOR
Bitwise exclusive OR of the destination register Rx and a source value.

	Format:
	a)	DIV RY RX
			 0x1a
			[0001 1010] [yyyy xxxx]

	b)	DIV IMM RX
			 0x2a
			[0010 1010] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Rx ^ Ry
		PC += 2
	
	b)	Rx  = Rx ^ Imm
		PC += 4

#### MOV
Move a source value into the destination register Rx.

	Format:
	a)	MOV RY RX
			 0x1b
			[0001 1011] [yyyy xxxx]

	b)	MOV IMM RX
			 0x2b
			[0010 1011] [0000 xxxx] [iiii iiii] [iiii iiii]

	Operation:
	a)	Rx  = Ry
		PC += 2
	
	b)	Rx  = Imm
		PC += 4

## Single operand instructions

#### PUSH
Push a word value from source register Rx onto the processor stack.

	Format:
		PUSH Rx
		 0x30
		[0011 0000] [0000 xxxx]

	Operation:
		SP -= 2
		Rx  = Memory[SP]
		PC += 2

#### POP
Pop a word from the processor stack into destination register Rx.

	Format:
		PUSH Rx
		 0x31
		[0100 0001] [0000 xxxx]

	Operation:
		SP += 2
		Rx  = Memory[SP]
		PC += 2


## Branching

#### JMP
Perform an unconditional absolute jump to immediate address.

	Format:
		JMP IMM
		 0x40
		[0100 0000] [0000 0000] [iiii iiii] [iiii iiii]

	Operation:
		PC = IMM

#### JNE
Perform a conditional absolute jump to immediate address if registers Rx and Ry are not equal.

	Format:
		JNE Ry Rx IMM
		 0x41
		[0100 0001] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry != Rx) Then
			PC  = Imm
		else
			PC += 4

#### JEQ
Perform a conditional absolute jump to immediate address if registers Rx and Ry are equal.

	Format:
		JNE Ry Rx IMM
		 0x42
		[0100 0010] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry == Rx) Then 
			PC  = Imm
		else 
			PC += 4

#### JL
Perform a absolute jump if Ry is less than Rx.

	Format:
		JNE Ry Rx IMM
		 0x43
		[0100 0011] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry < Rx) Then
			PC = Imm
		else
			PC += 4

#### JG
Perform a absolute jump if Ry is greater than Rx.

	Format:
		JNE Ry Rx IMM
		 0x44
		[0100 0100] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry > Rx) Then
			PC = Imm
		else
			PC += 4

#### JLE
Perform a absolute jump if Ry is less than or equal to Rx.

	Format:
		JNE Ry Rx IMM
		 0x45
		[0100 0101] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry <= Rx) Then
			PC = Imm
		else
			PC += 4

#### JGE
Perform a conditional absolute jump if register Ry is greater then or equal to Rx.

	Format:
		JNE Ry Rx IMM
		 0x46
		[0100 0110] [yyyy xxxx] [iiii iiii] [iiii iiii]

	Operation:
		If (Ry >= Rx) Then
			PC = Imm
		else
			PC += 4

#### CALL
Call a program subroutine.  Push the address of the instruction immediate after the call onto the stack and branch to immediate address.

	Format:
		CALL Imm
		 0x50
		[0101 0000] [0000 0000] [iiii iiii] [iiii iiii]

	Operation:
		SP -= 2
		Memory[sp] = PC + 4
		PC = IMM


## Zero operand instructions

#### RET
Return from subroutine by branching to a return address previously pushed on the stack.

	Format:
		RET
		 0x60
		[0110 0000] [0000 0000]

	Operation:
		SP += 2
		PC  = Memory[SP]

#### BRK
Execute the debug breakpoint interrupt. This instruction is equivalent in operation to INT 3.  The 3 in this case is implicit thus reducing this instructions size to a useful two bytes so that it can easily patch any other instruction.

	Format:
		RET
		 0x61
		[0110 0001] [0000 0000]

	Operation:
		Undefined
