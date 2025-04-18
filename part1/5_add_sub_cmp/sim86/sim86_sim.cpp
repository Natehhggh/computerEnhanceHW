#include "sim86.h"
#include "sim86_memory.h"
#include "sim86_reg.h"
#include <cstdio>


//TODO: relook at register struct, I dont like registers-registers format
static u32 getSourceValue(instruction_operand op, memory *memory, RegisterSet *registers){
    u32 data;
    switch(op.Type){
        case Operand_None:
            break;
        case Operand_Register:
            {
                register_access reg = op.Register;
                data = registers->Registers[reg.Index-1];
//                if(reg.Offset == 2){
 //                   data = registers->Registers[reg.Index-1];
  //              }else{
   //                 data = (registers->Registers[reg.Index-1] << (8 * reg.Offset&1)) & 0x00ff;
    //            }
            }
            break;
        case Operand_Memory:
            {
                            
            }
            break;
        case Operand_Immediate:
            {
                data = (u32)op.ImmediateS32;
            }
            break;
        case Operand_RelativeImmediate:
            break;
    }
    return data;
}

static void writeToDest(instruction_operand op, u32 val, memory *memory, RegisterSet *registers){
    u32 data;
    switch(op.Type){
        case Operand_None:
            break;
        case Operand_Register:
            {
                register_access reg = op.Register;
                registers->Registers[reg.Index-1] = val;

                std::printf("regIdx: %d -> %d\n",reg.Index -1, val);
                //if(reg.Offset == 2){
                //    data = registers->Registers[reg.Index];
                //}else{
                    //data = (registers->Registers[reg.Index] << (8 * reg.Offset&1)) & 0x00ff;
                //}

            }
            break;
        case Operand_Memory:
            {
                            
            }
            break;
        case Operand_Immediate:
            {

            }
            break;
        case Operand_RelativeImmediate:
            break;
    }
}


static void setFlags(u32 result, RegisterSet *registers){
    u16 flagsNew = 0;
    bool flag;
    //do I want to do a change in flags? or just report the state?
    //u16 flagsOld = registers->Registers[Register_flags];

    //SF
    flag = result & 0x8000;
    flagsNew |= flag << 7;
    if(flag){
        std::printf("SF ");
    }

    //zf
    flag = result == 0;
    flagsNew |=  (flag << 6);

    if(flag){
        std::printf("ZF ");
    }

    if(flagsNew){
        std::printf("\n");
    }
    

    registers->Registers[Register_flags] = flagsNew;
}

static void executeInstruction(instruction instruction, memory *memory, RegisterSet *registers){
    switch(instruction.Op){
        case Op_mov:{
            u32 data = getSourceValue(instruction.Operands[1], memory, registers);
            writeToDest(instruction.Operands[0], data, memory, registers);
        }
        break;

        case Op_add:{
            s32 dataRight = getSourceValue(instruction.Operands[1], memory, registers);
            s32 dataLeft = getSourceValue(instruction.Operands[0], memory, registers);
            dataLeft = dataLeft + dataRight;
            setFlags(dataLeft, registers);
            writeToDest(instruction.Operands[0], dataLeft, memory, registers);
        }
        break;

        case Op_sub:{
            s32 dataRight = (s32)getSourceValue(instruction.Operands[1], memory, registers);
            s32 dataLeft = (s32)getSourceValue(instruction.Operands[0], memory, registers);
            dataLeft = dataLeft - dataRight;
            setFlags(dataLeft, registers);
            writeToDest(instruction.Operands[0], dataLeft, memory, registers);
        }
        break;

        case Op_cmp:{
            s32 dataRight = getSourceValue(instruction.Operands[1], memory, registers);
            s32 dataLeft = getSourceValue(instruction.Operands[0], memory, registers);
            dataLeft = dataLeft - dataRight;
            setFlags(dataLeft, registers);

        }
        break;

    }


}


