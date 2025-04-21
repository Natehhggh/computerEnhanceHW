#include "sim86.h"
#include "sim86_reg.h"
#include <cstdio>



static s32 getEffectiveRegisterValue(effective_address_base base, RegisterSet *registers){
    switch(base){
        case EffectiveAddress_bx:
        {
            printf("effectiveAddress_bx\n");
            return registers->Registers[Register_b - 1];

        }
            break;

    }

    return 0;
}

//TODO: relook at register struct, I dont like registers-registers format
static s32 getSourceValue(instruction_operand op, memory *memory, RegisterSet *registers){
    s32 data;
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
                u32 regVal = registers->Registers[op.Address.Segment];
                u32 addr = GetAbsoluteAddressOf(0, op.Address.Base,  op.Address.Displacement+ regVal);
                data = (ReadMemory(memory, addr) << 8) | ReadMemory(memory, addr + 1);


            }
            break;
        case Operand_Immediate:
            {
                data = (u32)op.ImmediateS32;
            }
            break;
        case Operand_RelativeImmediate:
                data = (u32)op.ImmediateS32;
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
                std::printf("Operand_Register\n");
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
                std::printf("Operand_memory\n");
                u32 regVal = getEffectiveRegisterValue(op.Address.Base, registers);
                u32 addr = GetAbsoluteAddressOf(0, 0,  op.Address.Displacement);
                std::printf("storing %d at [%d + %d]\n", data, regVal, addr);
                StoreMemoryWide(memory, regVal + addr, val);
            }
            break;
        case Operand_Immediate:
            {

                std::printf("storing immediate \n");
            }
            break;
        case Operand_RelativeImmediate:
                std::printf("storing relative Immediate \n");
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
    
    std::printf("IP: %d -> ", registers->Registers[Register_ip]);
    registers->Registers[Register_ip] += instruction.Size;
    std::printf("%d\n", registers->Registers[Register_ip]);
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

        case Op_jne:{
            u16 flags = registers->Registers[Register_flags];
            if(!(flags & 0b1000000)){
                s32 data = (s32)getSourceValue(instruction.Operands[0], memory, registers);
                printf("jumping: %d bytes\n", data);
                //TODO: Casey says the binary writes the offset with the instruction size, but it's not working without an offset?
                registers->Registers[Register_ip] += data - 2;
                std::printf("%d\n", registers->Registers[Register_ip]);
            }

        }
        break;

    }


}


