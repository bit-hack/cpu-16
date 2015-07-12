# CPU-16

CPU-16 is a fake 16 bit processor written as part of my desire to better learn assembly language, CPU design, and VM design.
Since CPU-16 is not a real architecture I was able to make the instruction set as simple to learn as possible.
A RISC load/store architecture is followed, so allowing memory to be accessed only via specific instructions.
This processor also follows a Von-Neumann architecture; with program instructions occupying the same memory as the working data of a program.

## status

* An assembler has been written in python
* An very (very) crude emulator has been written in c++
* Serial out byte at address 0x8000 writes to the console

## todo

* Emulator doesn't currently calculate arithmetic overflow
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
R03 (OF)     arithmetic overflow
R04
...          general purpose
R16
```

All arithmetic is calculated at 32 bits internally and the high word is written into OF.

## Memory map

Since CPU-16 has a 16bit address bus, 0xffff is the highest location that can be addressed.
words are stored in memory as little endian.

```
0x0000
...         - general SRAM
0xfffd
0xfffe      - reset vector

0x8000      - Serial out port
```

## Power on

At power-on the program counter is set to the address written placed in the reset vector 0xFFFE

## Assembly directives

*operand types:
** #xxxx           - hex literal
** %name           - symbolic address

*symbolic address:
** FUNCTION name   - declare function 'name'
** .label          - mark address with label
** END             - mark end of function

*address control:
** AT #1234        - set assembler output address

*encode data:
** DATA #ff        - output byte 0xff
** DATA $HELLO     - output ascii string 'HELLO'
** DATA %label     - output address of label

## Instructions

Instruction Format: 0xCCXY, [0xIIII]

Instructions are either 16bits in size or 32bits if it has an immediate value.

```
| 0xCC   | 0x0_             | 0x1_           | 0x2_      | 0x3_       | 0x4_    | 0x5_          | 0x6_   | 0x7_     | 0x8_   |
| ------ | ---------------- | -------------- | --------- | ---------- | ------- | ------------- | ------ | -------- | ------ |
| 0x_0   | LOAD.W   RY  RX  | LOAD.W IMM RX  | ADD RX RY | ADD IMM RX | PUSH RX | JMP       IMM | JRO RX | CALL IMM | RETI   |
| 0x_1   | LOAD.B   RY  RX  | LOAD.B IMM RX  | MUL RX RY | MUL IMM RX | POP  RX | JNE RX RY IMM |        | INT  IMM | CLI    |
| 0x_2   | LOAD.W+  RY  RX  |                | SHL RX RY | SHL IMM RX |         | JEQ RX RY IMM |        |          | STI    |
| 0x_3   | LOAD.B+  RY  RX  |                | SHR RX RY | SHR IMM RX |         | JL  RX RY IMM |        |          |        |
| 0x_4   | STORE.W  RY  RX  | STORE.W RX IMM | SUB RX RY | SUB IMM RX |         | JG  RX RY IMM |        |          |        |
| 0x_5   | STORE.B  RY  RX  | STORE.B RX IMM | DIV RX RY | DIV IMM RX |         | JLE RX RY IMM |        |          |        |
| 0x_6   | STORE.W+ RY  RX  |                | MOD RX RY | MOD IMM RX |         | JGE RX RY IMM |        |          |        |
| 0x_7   | STORE.B+ RY  RX  |                | AND RX RY | AND IMM RX |         |               |        |          |        |
| 0x_8   |                  |                | OR  RX RY | OR  IMM RX |         |               |        |          |        |
| 0x_9   |                  |                | XOR RX RY | XOR IMM RX |         |               |        |          |        |
| 0x_A   |                  |                | MOV RX RY | MOV IMM RX |         |               |        |          |        |
```

IMM is a 16 bit immediate value following the instruction
RX and RY are specified in the (X,Y) register field of the instruction


