#ifndef SIM86_REG_H
#define SIM86_REG_H
#include "sim86.h"
struct RegisterSet
{
    u16 Registers[Register_count];
};

enum Access_Type{
    Low,
    High,
    Wide
};

#define LOW_ACCESS_MASK 0x0f
#define HIGH_ACCESS_MASK 0xf0

//TODO: reg access in sim.h can it be reused/modified?
struct reg_access
{
    u16 RegisterIdx;
    Access_Type Access;
};

static u16 ReadRegister(reg_access registerAccess);
static u16 ReadRegister(effective_address_base effective_address);
static u16 SetRegister(reg_access registerAccess);

#endif
