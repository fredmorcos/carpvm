# CarpVM

Based on Carp by Maxwell Bernstein. The goal of the project is to try
and build a small (and decently reliable) VM from the ground up. Right
now there are instructions, registers, a stack, data memory, and
calls. Recursion should work. Please consider this unstable and
non-ready software. Things do and will change more often than not.

Contributions are most welcome.

## Differences between Carp (from Maxwell Bernstein) and CarpVM

* C11 with -pedantic -Wall -Wextra and a few more
* Aims for zero compiler warnings in library, interpreter and tests
* Completely removed use of the C preprocessor (except for `#includes`
  and few `#defined` constants)
* OpenBSD-like KNF coding style
* Uses libok with asserts instead of libtap for tests (simpler)
* Cleaned-up project directory hierarchy
* Less focus on portability or compiler backwards compatibility
* Hooks to build, test and run the interpreter before every commit

## Further goals of CarpVM

* Cleanup the code and focus on correctness and simplicity
* Focus on memory safety and memory management correctness (talloc)
* Redesign the instruction set to offer a smaller core to build upon
* Improve documentation and rationale behind the instruction set
* Separate parsing into a compiler and build an intermediate bytecode
* Improve error handling and exit conditions/states
* Faster symbol lookup
* Small main loop and focus on cache coherence
* Exception mechanism

## Goals from Carp's old TODO file

* `loadstr` to read a string into memory and push address onto stack
* Bounds-checking `load` and `store` instructions

## Installation

### Getting Carp

**If you already have a local copy**

* `git fetch`
* `git rebase`

**If you don't already have a local copy**

* `git clone`

### Building Carp

1. `make`
2. `make test` (optional)
3. `make install`
4. `make clean` (optional)

## Use

See `carp -h` for help with command-line arguments.

### Interpreter

Run `carp your_file.carp`

### API

1. Include `carp/carp_machine.h` in your program
2. Run `gcc program.c /usr/local/lib/libcarp.a -o program`

## Syntax

The syntax looks like most ASM with instructions, arguments and
whatnot:

* Function/label definitions are defined with a colon at the end of
the name, like so: `add:`
* Function calls are simple: `call add, 2`
* Registers are not prefixed with %, like so: `ax`
* Instruction arguments are separated by commas (though actually they
  don't have to be)
* All words must be shorter than 10 characters long
* All Carp files must end with a blank line

Example:

```asm
add:
  load -4
  load -3
  add
  pop ax
  ret

main:
  push 7
  push 9
  call add
  preg ax
  halt 0
```

This should print `16`.

## Instruction set

Opcode | Arguments | Description
------ | --------- | -----------
HALT   | exit code | Sets `ext` to given code, halts, and attempts to clean up stack, data memory, and label memory.
NOP    |           | Does nothing. Seriously.
LOADR  | reg, val  | Loads given integer value into given register.
LOAD   | diff      | Loads value at location `fp` + diff in the stack.
STORE  | diff, val | Stores value at location `fp` + diff.
MOV    | dst, src  | Copies contents of src register into dst register.
ADD    |           | Pops the top two integers from the stack and pushes their sum.
SUB    |           | Pops the top two integers from the stack and pushes the difference (lower minus upper).
MUL    |           | Pops the top two integers from the stack and pushes their product.
MOD    |           | Pops the top two integers from the stack and pushes lower % upper.
SHR    |           | Pops the top two integers from the stack and pushes lower >> upper.
SHL    |           | Pops the top two integers from the stack and pushes lower << upper.
NOT    |           | Pops top integer from stack and pushes bitwise NOT of that integer.
XOR    |           | Pops the top two integers from the stack and pushes bitwise XOR..
OR     |           | Pops the top two integers from the stack and pushes bitwise OR.
AND    |           | Pops the top two integers from the stack and pushes bitwise AND.
INCR   | reg       | Increments value in given register.
DECR   | reg       | Decrements value in given register.
INC    |           | Increments the value at the top of the stack.
DEC    |           | Decrements the value at the top of the stack.
PUSHR  | reg       | Pushes value in given register.
PUSH   | val       | Pushes given value.
POP    | reg       | Pops an integer from the stack and dumps it into given register.
CMP    |           | Pops the top two integers from the stack and checks if equal. 0 means equal. Pushes result.
LT     |           | Pops the top two integers from the stack and checks if lower < upper. Pushes result.
GT     |           | Pops the top two integers from the stack and checks if lower > upper. Pushes result.
JZ     | addr      | Jumps to given absolute address if top of the stack is 0.
RJZ    | diff      | Adds differential to `ip` (relative jump) if top of the stack is 0.
JNZ    | addr      | Jumps to given absolute address if top of the stack is not 0.
RJNZ   | diff      | Adds differential to `ip` (relative jump) if top of the stack is not 0.
JMP    | addr      | Jumps to given absolute address unconditionally.
RJMP   | diff      | Adds differential to `ip` (relative jump) unconditionally.
CALL   | key/addr  | Save state and set IP to value in data memory at key. Function may return value in `ax`.
RET    |           | Put top of the stack into `ax` and load previous state.
PREG   | reg       | Prints contents of given register.
PTOP   |           | Peeks top of stack and prints top value.

## Registers

Name | Purpose
---- | -------
`r0` ... `r9` | General purpose.
`ax` | Return value for user-defined function.
`bx`, `cx`, `dx`, `rx` | ... something in the future. Just taking up space for now.
`ip` | Instruction pointer. Used for keeping place in code, gotos, calling, etc.
`sp` | Stack pointer.
`fp` | Frame pointer. Used to keep state for function calls.
`gbg` | Garbage register mainly used for popping.
`run` | Boolean - is machine running?
`ext` | Exit code.

## License

GPLv3 - See [LICENSE](http://github.com/fredmorcos/carp/tree/master/LICENSE)
