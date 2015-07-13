# CPU-16
## Description

CPU-16 is a fake 16 bit processor written as part of my desire to better learn assembly language, CPU design, and VM design.
Since CPU-16 is not a real architecture I was able to make the instruction set as simple to learn as possible.
A RISC load/store architecture is followed, so allowing memory to be accessed only via specific instructions.
This processor also follows a Von-Neumann architecture; with program instructions occupy the same memory as the working data of a program.

## Status

* An assembler has been written in python
* The emulator is still very much a work in progress
* A address/data bus abstraction has been added to the emulator
* Bytes written to 0x8000 to 0x8400 will be output to the console (crappy serial)

## Todo

* Loads of scope to improve the emulator
* Interrupts not implemented
* Only tested with hello world example
* More interrupt vectors can be implemented
* Add timer peripheral
* Add basic VGA graphics peripheral
* Add sound synth peripheral such as (YM2612, SN76489)
* Add keyboard controller peripheral
* Add disk controller
* Write basic OS

## Usage

The Assembler:
```
python cpu16asm.py <input.asm> <output.img>
```

The emulator:
```
cpu16 <output.img>
```

## Registers

All registers are 16 bits wide.

```
R00 (ZR)     Zero (always 0)
R01 (PC)     PC
R02 (SP)     stack pointer
R03
...          general purpose
R16
```

## Memory map

Since CPU-16 has a 16bit address bus, 0xffff is the highest location that can be addressed.
words are stored in memory as little endian.

```
{ 0x0000 .. 0x7fff }  - SRAM
{ 0x8000 .. 0x83ff }  - serial interface
{ 0x8400 .. 0xfffd }  - SRAM
{ 0xfffe }            - reset vector
```

## Power on

At power-on the program counter is set to the address of the reset vector; 0xFFFE.

## Assembly directives

* Operand types:
  * A symbolic address can be either a function name or a label.
```
#xxxx           - hexadecimal literal
%name           - symbolic address
```

* Symbolic address:
  * Functions will impose their scope on any labels declared within them.
```
FUNCTION name   - declare function 'name'
.label          - mark address with label
END             - mark end of function
```

* Address control:
  * All following output will follow from address #XXXX.
```
AT #XXXX        - set assembler output address
```

* Encode data:
  * String data is not zero terminated and may not contain spaces
  * Spaces must be encoded as the ascii #20 char
  * Data can contain many types on one line
```
DATA #ff        - output byte 0xff
DATA $HELLO     - output ascii string 'HELLO'
DATA %label     - output address of label
DATA $HI #20 #1234
```

## Instructions

* Instruction format
  * Instructions are either 16bits in size or 32bits if it has an immediate value.
  * The CC byte encodes the specific opcode to execute.
  * RX and RY are specified in the XY register byte of the instruction.
  * IMM is a little endian 16 bit immediate value following the instruction.
```
0xCCXY, [0xIIII]
```

* Instruction map
  * <opcode>.W denotes a memory operation for WORD values.
  * <opcode>.B denotes a memory operator for BYTE values.
  * <opcode>+ denotes that the address register will be incremented by the data size after execution.

```
| 0xCC   | 0x0_             | 0x1_           | 0x2_      | 0x3_       | 0x4_    | 0x5_          | 0x7_     | 0x8_   |
| ------ | ---------------- | -------------- | --------- | ---------- | ------- | ------------- | -------- | ------ |
| 0x_0   | LOAD.W   RY  RX  | LOAD.W IMM RX  | ADD RX RY | ADD IMM RX | PUSH RX | JMP       IMM | CALL IMM | RETI   |
| 0x_1   | LOAD.B   RY  RX  | LOAD.B IMM RX  | MUL RX RY | MUL IMM RX | POP  RX | JNE RX RY IMM | INT  IMM | CLI    |
| 0x_2   | LOAD.W+  RY  RX  |                | SHL RX RY | SHL IMM RX |         | JEQ RX RY IMM |          | STI    |
| 0x_3   | LOAD.B+  RY  RX  |                | SHR RX RY | SHR IMM RX |         | JL  RX RY IMM |          |        |
| 0x_4   | STORE.W  RY  RX  | STORE.W RX IMM | SUB RX RY | SUB IMM RX |         | JG  RX RY IMM |          |        |
| 0x_5   | STORE.B  RY  RX  | STORE.B RX IMM | DIV RX RY | DIV IMM RX |         | JLE RX RY IMM |          |        |
| 0x_6   | STORE.W+ RY  RX  |                | MOD RX RY | MOD IMM RX |         | JGE RX RY IMM |          |        |
| 0x_7   | STORE.B+ RY  RX  |                | AND RX RY | AND IMM RX |         |               |          |        |
| 0x_8   |                  |                | OR  RX RY | OR  IMM RX |         |               |          |        |
| 0x_9   |                  |                | XOR RX RY | XOR IMM RX |         |               |          |        |
| 0x_A   |                  |                | MOV RX RY | MOV IMM RX |         |               |          |        |
| 0x_B   |                  |                | MLH RX RY | MLH IMM RX |         |               |          |        |
```

  * CALL - call a subroutine (PUSH PC+4)
  * INT  - raise a specific interrupt
  * MLH  - multiply, keep high word (good for fixed point)
  * RETI - return from interrupt
  * CLI  - disable interrupts
  * STI  - enable interrupts

## Pseudo instructions

  * RET  (POP PC)
  * NOP  (MOV ZR ZR)
  * NOT  (XOR RX #FFFF)
  * SWAP (XOR RX RY, XOR RY RX, XOR RX RY)
