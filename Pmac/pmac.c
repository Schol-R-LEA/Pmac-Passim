/* pmac.c - a simple p-machine demonstrator in C. The program
 * simulates a simple stack machine.
 * version 00.01.00
 * Copyright (C) 2011  Joseph Osako
 * This file is part of Pmac-Passim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXMEM 65536

int TRACE = false;  /* tracing toggle */

typedef enum {SUCCEED, FAIL} EXITTYPE;

typedef uint16_t WORD;


/* program and disk image files */
FILE* program = NULL;
FILE* diskimg = NULL;


/* simulated memory and registers */
WORD memory[MAXMEM];

WORD ip = 0;           // instruction pointer
WORD sp = MAXMEM - 1;  // stack pointer, initialized to the top of memory
WORD fp = MAXMEM - 1;  // frame pointer, initially matches the stack pointer


/* instruction set opcode values */
enum {
    HALT = 0,
    PUSH, PUSHI, PUSHR, PUSHA, PUSHO, PUSHF, PUSHS, PUSHP, PUSHZ, DUP,
    POPA = 0x0100, POPI, POPR, POPO, POPF,  POPS, DROP, SWAP,
    BRA = 0x0200, BRI,
    BRZ = 0x0300, BNZ,
    BSR = 0x0400, RTS,
    EQL = 0x0500, NEQ, LES, LEQ, GRE, GEQ,
    ADD = 0x0600, INC = 0x06F0, SUB = 0x0700, DEC = 0x07F0,
    MUL = 0x0800, DIV = 0x0900, MOD = 0x09F0,
    SHL = 0x0A00, SHR = 0x0B00,
    IOR = 0x0C00, XOR = 0x0D00, AND = 0x0E00, NOT = 0x0F00,
    IN  = 0x1000, OUT = 0x2000
} OPCODES;

/* simulated I/O ports */
enum {TTY = 0, FDD = 1} PORTS;

/* function prototypes */
void finish(char* description, EXITTYPE result);
void dumpregs(void);
void pause(void);
void read_program(void);
void interp(void);
void push(WORD val);
WORD pop(void);
WORD argument(void);
WORD index_arg(void);
void input(void);
void output(void);
void display_program(void);


/* main()
The pmac program takes two arguments, a program file name
and a disk image file name. An optional fourth argument, '-t',
enables the tracing mode. The program indicates when
the program begins and ends.
*/
int main (int argc, char *argv[])
{
    if (3 > argc)
        finish("Usage: <program> <diskimg>", FAIL);

    /* open the two working files */

    if (NULL == (program = fopen(argv[1], "r")))
        finish("Program file not found", FAIL);

    if (NULL == (diskimg = fopen(argv[2], "rw+")))
    {
        finish("Could not create disk image file", FAIL);
    }

    if (4 == argc && 0 == strcmp(argv[3], "-t"))
    {
        TRACE = true;
        puts("tracing mode ON");
    }
    else
        TRACE = false;

    printf("\nLoading Program...");
    memset(memory, 0, MAXMEM); // clear the memory
    read_program();
    printf("done.");
    if (TRACE)
    {
        display_program();
    }
    puts("Beginning run:");
    interp();
    return 0;
}

void finish(char* description, EXITTYPE result)
{
    puts("\n");
    puts(description);
    puts("\n");
    dumpregs();
    if (program != NULL) fclose(program);
    if (diskimg != NULL) fclose(diskimg);
    exit(result);
}


// dumpregs() - display the registers as a line of text
void dumpregs()
{
    printf("Registers: IP:%4x   SP:%4x   FP:%4x   TOS:%4x\n\n", ip, sp, fp, memory[sp]);
}

void read_program()
{
    int i;

    for (i = 0; !feof(program) && !ferror(program) && i < MAXMEM; i++)
        fscanf(program, "%4x", (unsigned int *) &memory[i]);
}

void trace(char *inst, WORD op)
{
    if (TRACE)
    {
        printf("Inst: %s  Opcode: %4x\n", inst, op);
        dumpregs();
        getchar();
    }

}

void interp()
{
    WORD op;
    WORD temp;

    do
    {
        op = memory[ip];

        switch(op)
        {
            case HALT:
                trace("HALT", op);
                finish("Execution halted.", SUCCEED);
                break;
            case PUSH:      /* push immediate */
                temp = argument();
                push(temp);
                if (TRACE)
                {
                    printf("Inst: PUSH #%4x  Opcode: %4x\n",
                           temp, op);
                    dumpregs();
                    getchar();
                }
                break;
            case PUSHI:     /* push indexed */
                temp = index_arg();
                push(memory[temp]);
                if(TRACE)
                {
                    printf("Inst: PUSH %4x[%4x]  Opcode: %4x Index: %4x\n I",
                           memory[ip-1], memory[ip], op, memory[memory[ip]]);
                    dumpregs();
                    getchar();
                }
                break;
            case PUSHR:     /* push indirect from stack */
                temp = pop();
                push(memory[temp]);
                trace("PUSHR", op);
                break;
            case PUSHA:     /* push indirect from argument */
                temp = argument();
                push(memory[temp]);
                if(TRACE)
                {
                    printf("Inst: PUSHA %4x  Opcode: %4x\n", temp, op);
                    dumpregs();
                    getchar();
                }
                break;
             case PUSHO:     /* push from frame pointer offset */
                temp = pop();
                push(memory[fp + temp]);
                trace("PUSHO", op);
            case PUSHF:     /* push frame pointer */
                push(fp);
                trace("PUSHF", op);
                break;
            case PUSHS:     /* push stack pointer */
                push(sp);
                trace("PUSHS", op);
                break;
            case PUSHP:     /* push instruction pointer */
                push(ip);
                trace("PUSHP", op);
                break;
            case PUSHZ:     /* push zero */
                push(0);
                trace("PUSHZ", op);
                break;
            case DUP:     /* push zero */
                push(memory[sp]);
                trace("DUP", op);
                break;
            case POPA:
                memory[argument()] = pop();
                if(TRACE)
                {   temp = memory[ip];
                    printf("Inst: POP %4x Opcode: %4x Target: %4x\n",
                            temp, op, memory[temp]);
                    dumpregs();
                    getchar();
                }
                break;
            case POPI:
                memory[index_arg()] = pop();
                if(TRACE)
                {
                    temp = memory[ip] + memory[ip + 1];
                    printf("Inst: POP %4x[%4x]  Opcode: %4x Target: %4x\n",
                            memory[ip], memory[ip-1], op, temp);
                    dumpregs();
                    getchar();
                }
                break;
            case POPO:
                temp = pop();
                memory[fp + temp] = pop();
                trace("POPO", op);
                break;
            case POPR:
                temp = pop();
                memory[temp] = pop();
                trace("POPR", op);
                break;
            case POPF:
                fp = pop();
                trace("POPF", op);
                break;
            case POPS:
                sp = pop();
                trace("POPS", op);
                break;
            case DROP:
                pop();            /*ignore result */
                trace("DROP", op);
                break;
            case SWAP:
                temp = memory[sp];
                memory[sp] = memory[sp-1];
                memory[sp-1] = temp;
                trace("SWAP", op);
                break;

            /* branch */
            case BRA:
                ip = argument();
                if(TRACE)
                {
                    printf("Inst: BRA %4x Opcode: %4x\n", memory[ip], op);
                    dumpregs();
                    getchar();
                }
                break;
            case BRI:
                ip = index_arg();
                if(TRACE)
                {
                    printf("Inst: BRI %4x[%4x]  Opcode: %4x\n", memory[ip], memory[ip-1], op);
                    dumpregs();
                    getchar();
                }
                break;
            /* conditional branch */
            case BRZ:
                temp = pop();
                if (0 == temp)
                    ip = argument();
                else
                   ip += 2;
                if(TRACE)
                {
                    printf("Inst: BRZ %4x Opcode: %4x\n", memory[ip], op);
                    dumpregs();
                    getchar();
                }
                break;
            case BNZ:
                temp = pop();
                if (0 != temp)
                    ip = argument();
                else
                   ip += 2;
                if(TRACE)
                {
                    printf("Inst: BNZ %4x Opcode: %4x\n", memory[ip], op);
                    dumpregs();
                    getchar();
                }
                break;
            /* call and return */
            case BSR:
                push(ip);
                ip = argument();
                if(TRACE)
                {
                    printf("Inst: BSR %4x Opcode: %4x\n", memory[ip], op);
                    dumpregs();
                    getchar();
                }
                break;
            case RTS:
                ip = pop();
                trace("RTS", op);
                break;
            /* comparisons */
                break;
            case EQL:
                temp = pop();
                push(pop() == temp ? 1 : 0);
                trace("EQL", op);
                break;
            case NEQ:
                temp = pop();
                push(pop() == temp ? 0 : 1);
                trace("NEQ", op);
                break;
            case LES:
                temp = pop();
                push(temp < pop() ? 0 : 1);
                trace("NEQ", op);
                break;
            case LEQ:
                temp = pop();
                push(temp <= pop() ? 0 : 1);
                trace("LEQ", op);
                break;
            case GRE:
                temp = pop();
                push(temp > pop() ? 0 : 1);
                trace("GRE", op);
                break;
            case GEQ:
                temp = pop();
                push(temp >= pop() ? 0 : 1);
                trace("GEQ", op);
                break;
            case ADD:
                push(pop() + pop());
                trace("ADD", op);
                break;
            case INC:
                (memory[sp])++;
                trace("INC", op);
                break;
            case SUB:
                push(pop() - pop());
                trace("SUB", op);
                break;
            case DEC:
                (memory[sp])--;
                trace("DEC", op);
                break;
            case MUL:
                push(pop() * pop());
                trace("MUL", op);
                break;
            case DIV:
                temp = pop();
                if (0 == temp)
                    finish("Divide by Zero Error", FAIL);
                else
                    push(pop() / temp);
                trace("DIV", op);
                break;
            case MOD:
                temp = pop();
                push(pop() % temp);
                trace("MOD", op);
                break;
             /* shift operators */
            case SHL:
                memory[sp] <<= pop();
                trace("SHL", op);
                break;
            case SHR:
                memory[sp] >>= pop();
                trace("SHR", op);
                break;
            case IOR:
                memory[sp] |= pop();
                trace("IOR", op);
                break;
            case XOR:
                memory[sp] ^= pop();
                trace("XOR", op);
                break;
            case AND:
                memory[sp] &= pop();
                trace("AND", op);
                break;
            case NOT:
                memory[sp] = ~memory[sp];
                trace("NOT", op);
                break;
            case IN:
                input();
                break;
            case OUT:
                output();
                break;
            default:
                break;    /* do nothing */
        }
        if (op != BRA && op != BRZ && op != BNZ && op != BSR && op != RTS && op != HALT)
        {
            ip++;
        }
    } while (1);

}

void push(WORD value)
{
    memory[--sp] = value;
}

WORD pop()
{
    return memory[sp++];
}

WORD argument()
{
    ip++;
    return memory[ip];
}

WORD index_arg()
{
    WORD base, index;

    base = argument();
    index = argument();
    return (base + memory[index]);
}


void output()
{
    WORD port, seek, value;

    port = seek = value = 0;

    port = pop();

    switch(port)
    {
        case TTY:
            value = pop();
            putchar((char) value);
            break;
        case FDD:
            seek = pop();
            seek *= sizeof(WORD) + strlen("\n") + 2;
            value = pop();
            fseek(diskimg, seek, 0);
            fprintf(diskimg, "%4x\n", value);
            break;
        default:
            if (TRACE)
            {
                printf("Inst: OUT  Opcode: %4x  Port: %4x  Seek: %4x  Value: %4x\n",
                       OUT, port, seek * sizeof(WORD), value);
                       dumpregs();
                       getchar();
            }
            finish("Invalid output port", FAIL);
            break;
    }
    if (TRACE)
    {
        printf("Inst: OUT  Opcode: %4x  Port: %4x  Seek: %4x  Value: %4x\n",
               OUT, port, seek, value);
        dumpregs();
        getchar();
    }

}

void input()
{
    WORD port, seek = 0, value = 0;

    port = pop();
    switch (port)
    {
        case TTY:
            push(getchar());
            break;
        case FDD:
            seek = pop();
            seek *= sizeof(WORD);
            fseek(diskimg, seek, 0);
            fscanf(diskimg, "%4x", (unsigned int *) &value);
            push(value);
            break;
        default:
            if (TRACE)
            {
                printf("Inst: OUT  Opcode: %4x  Port: %4x  Seek: %4x  Value: %4x\n",
                       OUT, port, seek, value);
                dumpregs();
                getchar();
            }
            finish("Invalid input port", FAIL);
            break;
    }
    if (TRACE)
    {
        printf("Inst: OUT  Opcode: %4x  Port: %4x  Seek: %4x  Value: %4x\n",
               OUT, port, seek, value);
        dumpregs();
        getchar();
    }

}

void display_program()
{
    WORD op;
   /* WORD temp; */

    puts("\nProgram Listing:");
    do
    {
        op = memory[ip];

        switch(op)
        {
            case HALT:
                puts("HALT");
                break;
            case PUSH:      /* push immediate */
                ip++;
                printf("PUSH #%4x\n", memory[ip]);
                break;
            case PUSHI:     /* push indexed */
                ip += 2;
                printf("PUSH %4x[%4x]\n", memory[ip - 1], memory[ip]);
                break;
            case PUSHA:     /* push indirect */
                ip++;
                printf("PUSH %4x\n", memory[ip]);
                break;
            case PUSHR:     /* push indirect from stack */
                puts("PUSHR");
                break;
            case PUSHF:     /* push frame pointer */
                puts("PUSHF");
                break;
            case PUSHS:     /* push stack pointer */
                puts("PUSHS");
                break;
            case PUSHP:     /* push instruction pointer */
                puts("PUSHP");
                break;
            case PUSHZ:     /* push zero */
                puts("PUSHZ");
                break;
            case DUP:     /* push zero */
                puts("DUP");
                break;
            case POPA:
                ip++;
                printf("POP %4x\n", memory[ip]);
                break;
            case POPI:
                ip += 2;
                printf("POP %4x[%4x]\n", memory[ip - 1], memory[ip]);
                break;
            case POPR:
                puts("POPR");
                break;
            case POPF:
                puts("POPF");
                break;
            case POPS:
                puts("POPS");
                break;
            case DROP:
                puts("DROP");
                break;
            case SWAP:
                puts("SWAP");
                break;
            /* branch */
            case BRA:
                ip++;
                printf("BRA %4x\n", memory[ip]);
                break;
            case BRI:
                ip += 2;
                printf("BRI %4x[%4x]\n", memory[ip - 1], memory[ip]);
                break;
            /* conditional branch */
            case BRZ:
                ip++;
                printf("BRZ %4x\n", memory[ip]);
                break;
            case BNZ:
                ip++;
                printf("BNZ %4x\n", memory[ip]);
                break;
            /* call and return */
            case BSR:
                ip++;
                printf("BSR %4x\n", memory[ip]);
                break;
            case RTS:
                puts("RTS");
                break;
            /* comparisons */
                break;
            case EQL:
                puts("EQL");
                break;
            case NEQ:
                puts("NEQ");
                break;
            case LES:
                puts("NEQ");
                break;
            case LEQ:
                puts("LEQ");
                break;
            case GRE:
                puts("GRE");
                break;
            case GEQ:
                puts("GEQ");
                break;
            case ADD:
                puts("ADD");
                break;
            case INC:
                puts("INC");
                break;
            case SUB:
                puts("SUB");
                break;
            case DEC:
                puts("DEC");
                break;
            case MUL:
                puts("MUL");
                break;
            case DIV:
                puts("DIV");
                break;
            case MOD:
                puts("MOD");
                break;
             /* shift operators */
            case SHL:
                puts("SHL");
                break;
            case SHR:
                puts("SHR");
                break;
            case IOR:
                puts("IOR");
                break;
            case XOR:
                puts("XOR");
                break;
            case AND:
                puts("AND");
                break;
            case NOT:
                puts("NOT");
                break;
            case IN:
                puts("IN");
                break;
            case OUT:
                puts("OUT");
                break;
            default:
                break;    /* do nothing */
        }
        ip++;
    } while (HALT != op);
    /* reset values to start point */
    ip = 0;
    sp = fp = MAXMEM - 1;
    puts("\n");
}
