#include <stdio.h>

const char *ops[64];
const char *registers[8]; const char *registersWide[8];

void setup()
{
    ops[0b100010] = "mov"; // 34 move
    registers[0] = "al";
    registers[1] = "cl";
    registers[2] = "dl";
    registers[3] = "bl";
    registers[4] = "ah";
    registers[5] = "ch";
    registers[6] = "dh";
    registers[7] = "bh";

    registersWide[0] = "ax";
    registersWide[1] = "cx";
    registersWide[2] = "dx";
    registersWide[3] = "bx";
    registersWide[4] = "sp";
    registersWide[5] = "bp";
    registersWide[6] = "si";
    registersWide[7] = "di";
}


int main(int argc, char **argv){

    setup();
    if(argc != 3){
        printf("need 3 args");
        return 1;
    }
    char buff[100];
    FILE* file = fopen(argv[1], "rb");
    fgets(buff, 100, file);
    fclose(file);

    int idx = 0;
    int opIdx, regAIdx, regBIdx;
    file = fopen(argv[2], "w");
    while(buff[idx] != '\0'){
        int regA, regB;
        regA = (buff[idx+1] & 0b00111000) >> 3 & 0b111;
        regB = (buff[idx+1] & 0b00000111);
        if(buff[idx] & 1){
            fprintf(file, "%s %s, %s \n",ops[buff[idx]>>2 & 0b00111111], registersWide[regB], registersWide[regA]);
        }else{
            fprintf(file, "%s %s, %s \n",ops[buff[idx]>>2 & 0b00111111], registers[regB], registers[regA]);
        }
        idx+=2;
    }
    fclose(file);




    return 0;
}
