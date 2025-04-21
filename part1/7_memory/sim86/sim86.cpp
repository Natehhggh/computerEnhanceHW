/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

#include "sim86.h"

#include "sim86_memory.h"
#include "sim86_reg.h"
#include "sim86_text.h"
#include "sim86_decode.h"

#include "sim86_memory.cpp"
#include "sim86_text.cpp"
#include "sim86_decode.cpp"
#include "sim86_sim.cpp"

static void DisAsm8086(memory *Memory, u32 DisAsmByteCount, segmented_access DisAsmStart)
{
    segmented_access At = DisAsmStart;
    
    disasm_context Context = DefaultDisAsmContext();
    
    u32 Count = DisAsmByteCount;
    while(Count)
    {
        instruction Instruction = DecodeInstruction(&Context, Memory, &At);
        if(Instruction.Op)
        {
            if(Count >= Instruction.Size)
            {
                Count -= Instruction.Size;
            }
            else
            {
                fprintf(stderr, "ERROR: Instruction extends outside disassembly region\n");
                break;
            }
            
            UpdateContext(&Context, Instruction);
            if(IsPrintable(Instruction))
            {
                PrintInstruction(Instruction, stdout);
                printf("\n");
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognized binary in instruction stream.\n");
            break;
        }
    }
}

static void Sim8086(memory *Memory, RegisterSet *Registers, u32 DisAsmByteCount, segmented_access DisAsmStart)
{
    segmented_access At = DisAsmStart;
    
    disasm_context Context = DefaultDisAsmContext();
    
    s32 Count = DisAsmByteCount;
    while(Registers->Registers[Register_ip] < DisAsmByteCount)
    {
        instruction Instruction = DecodeInstruction(&Context, Memory, &At);
        if(Instruction.Op)
        {
            executeInstruction(Instruction, Memory, Registers);
            At.SegmentOffset = Registers->Registers[Register_ip];
            UpdateContext(&Context, Instruction);
            
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognized binary in instruction stream.\n");
            break;
        }
    }
}

int main(int ArgCount, char **Args)
{
    memory *Memory = (memory *)malloc(sizeof(memory));
    RegisterSet *Registers = (RegisterSet *)malloc(sizeof(RegisterSet));

    bool exec = true;
    
    if(ArgCount > 1)
    {
        for(int ArgIndex = 1; ArgIndex < ArgCount; ++ArgIndex)
        {
            //if(Args[ArgIndex] == "--exec"){
                //exec = true;
                //continue;
            //}


            char *FileName = Args[ArgIndex];
            u32 BytesRead = LoadMemoryFromFile(FileName, Memory, 0);
            
            if(exec){
                Sim8086(Memory, Registers, BytesRead, {});
                printf("Ax: %d\n", Registers->Registers[0]);
                printf("Bx: %d\n", Registers->Registers[1]);
                printf("Cx: %d\n", Registers->Registers[2]);
                printf("Dx: %d\n", Registers->Registers[3]);
                printf("SP: %d\n", Registers->Registers[4]);
                printf("BP: %d\n", Registers->Registers[5]);
                printf("SI: %d\n", Registers->Registers[6]);
                printf("DI: %d\n", Registers->Registers[7]);
            }else{
                printf("; %s disassembly:\n", FileName);
                printf("bits 16\n");
                DisAsm8086(Memory, BytesRead, {});
            }
        }

    }
    else
    {
        fprintf(stderr, "USAGE: %s [8086 machine code file] ...\n", Args[0]);
    }
    
    return 0;
}
