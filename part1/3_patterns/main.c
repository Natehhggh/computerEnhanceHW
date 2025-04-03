#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


//TODO: couple bugs to check out, just moving on for now
//Listing 39, either misses the last instruction, or writes a malformed if I do outer while loop for read file
//Check values that are output, immediate/addresses some dont seem right, but otherwise the overall logic is working
//

enum encodeType{
    REGMEM_TO_REG,
    IMMEDIATE_TO_REGMEM,
    IMMEDIATE_TO_REG,
    MEMORY_TO_REG, //I'm going to just hardcode specifics if I can, to acc or
    JUMPS
};

//https://www.righto.com/2023/05/8086-processor-group-decode-rom.html
//I dont know the full order, I'm only doing the subset for the assignment
const char *ops[36];
char masks[36];
char codes[36];
//tmp maybe, I kinda want to do the kmaps, but this is quicker for now
enum encodeType encoding[36];


const char *registers[16];
const char *arithStrs[8];
const char *jumpStrs[16];
const char *loopStrs[4];

void setup()
{
    //Arithmetic/
    ops[0] = "arith"; // reg/mem to from reg
    masks[0] = 0b11000100; //00XX X0XX
    codes[0] = 0b00000000; 
    encoding[0] = REGMEM_TO_REG;
    ops[1] = "immed"; // immediate to reg/mem
    masks[1] = 0b11111100; //1000 X10X
    codes[1] = 0b10000000; 
    encoding[1] = IMMEDIATE_TO_REGMEM;
    ops[2] = "arith"; // immediate to acc
    masks[2] = 0b11000110; //00XX X10X
    codes[2] = 0b00000100; 
    encoding[2] = IMMEDIATE_TO_REG;
                                      
    //Moves
    ops[3] = "mov"; // reg/mem to reg
    masks[3] = 0b11111100; //1000 10XX
    codes[3] = 0b10001000; 
    encoding[3] = REGMEM_TO_REG;
    //ops[4] = "mov"; // reg/mem from reg might remove this one for now, seems meaningless for this case?
    ops[4] = "mov"; // memory to/from acc I dont know why it's 2 things and doesn't use d, but I am 
    masks[4] = 0b11111100; //1010 00XX
    codes[4] = 0b10100000; 
    encoding[4] = MEMORY_TO_REG;
    ops[5] = "mov"; // immediate to register
    masks[5] = 0b11110000; //1011 XXXX
    codes[5] = 0b10110000; 
    encoding[5] = IMMEDIATE_TO_REG;
    ops[6] = "mov"; // immediate to reg/mem
    masks[6] = 0b11111110; //1100 011X
    codes[6] = 0b11000110; 
    encoding[6] = IMMEDIATE_TO_REGMEM;

    //Jump/loops
    ops[7] = "jmp"; // jump row
    masks[7] = 0b11110000; //0111 XXXX
    codes[7] = 0b01110000; 
    encoding[7] = JUMPS;

    ops[8] = "loop"; // loops and jcxz
    masks[8] = 0b11111100; //1110 00XX
    codes[8] = 0b11100000; 
    encoding[8] = JUMPS;
    ops[9] = "bad"; // loops and jcxz

                            
    registers[0] = "al";
    registers[1] = "cl";
    registers[2] = "dl";
    registers[3] = "bl";
    registers[4] = "ah";
    registers[5] = "ch";
    registers[6] = "dh";
    registers[7] = "bh";
    registers[8] = "ax";
    registers[9] = "cx";
    registers[10] = "dx";
    registers[11] = "bx";
    registers[12] = "sp";
    registers[13] = "bp";
    registers[14] = "si";
    registers[15] = "di";

    arithStrs[0] = "add";
    arithStrs[1] = "or";
    arithStrs[2] = "adc";
    arithStrs[3] = "sbb";
    arithStrs[4] = "and";
    arithStrs[5] = "sub";
    arithStrs[6] = "xor";
    arithStrs[7] = "cmp";

    jumpStrs[0] = "J0";
    jumpStrs[1] = "JN0";
    jumpStrs[2] = "JB";
    jumpStrs[3] = "JNB";
    jumpStrs[4] = "JE";
    jumpStrs[5] = "JNE";
    jumpStrs[6] = "JBE";
    jumpStrs[7] = "JNBE";
    jumpStrs[8] = "JS";
    jumpStrs[9] = "JNS";
    jumpStrs[10] = "JP";
    jumpStrs[11] = "JNP";
    jumpStrs[12] = "JL";
    jumpStrs[13] = "JNL";
    jumpStrs[14] = "JLE";
    jumpStrs[15] = "JNLE";

    loopStrs[0] = "LOOPNZ";
    loopStrs[1] = "LOOPZ";
    loopStrs[2] = "LOOP";
    loopStrs[3] = "JCXZ";
}


int main(int argc, char **argv){
    setup();
    if(argc != 3){
        printf("need 3 args");
        return 1;
    }

    //Read binary to buffer
    char buff[300] = {0};
    FILE* file = fopen(argv[1], "rb");
    FILE* fileOut = fopen(argv[2], "w");
    int instNum = 0;
    fgets(buff, 300, file);
    //while(fgets(buff, 100, file) != NULL){
        //fread(buff, sizeof(buff[0], 200, file));

        //Write Assembly to new file
        int idx = 0;
        while(buff[idx] != '\0' || buff[idx + 1] != '\0'){
            //Get Operation
            int opIdx;
            for(opIdx = 0; opIdx < 9; opIdx++){
                printf("buff: %i | mask: %i | res: %i | code: %i\n", (uint8_t)buff[idx], (uint8_t)masks[opIdx], (uint8_t)(buff[idx] & masks[opIdx]), (uint8_t)codes[opIdx]);
                if((buff[idx] & masks[opIdx]) == codes[opIdx]){
                    break;
                }
            }
            //printf("opIdx: %i\n", opIdx);
            //took the useful looking flags from this
            //https://static.righto.com/8086/groupRom.html
            bool wflag, dflag, sflag, accFlag, aluOpFlag;
            bool d, w, s;
            int mod = 0b100, reg, rm, bytesRead = 1;


            //tmp hardcoding the flags based on index, instead of bit mapping because
            wflag = opIdx <= 6;
            dflag = opIdx == 0 || opIdx ==3 || opIdx == 4;
            sflag = opIdx == 1;
            accFlag = opIdx == 2 || opIdx == 4;
            aluOpFlag = opIdx <= 2;
            
            char dstStr[20], srcStr[20], opStr[20];
            uint16_t disp, data, addr;
            int8_t ip_inc8;

            //Get Data from Byte data based on operation
            //TODO: refactor reading 1-2 byte data/disp/addr
            w = wflag && ((opIdx == 5 && buff[idx] & 0b1000) || (opIdx != 5  && buff[idx] & 0b1));
            //flipping regmem, because it makes encoding easier
            d = (dflag && (buff[idx] & 0b10)) || accFlag;
            //Technically the same as d, refactor later
            s = sflag && (buff[idx] &0b10);

            if(encoding[opIdx] == IMMEDIATE_TO_REG){
                if(accFlag){
                    reg = 0;
                }
                    reg = buff[idx + bytesRead] & 0b111;
            }

            if(encoding[opIdx] == REGMEM_TO_REG || encoding[opIdx] == IMMEDIATE_TO_REGMEM){
                    mod = (buff[idx + bytesRead] >> 6) & 0b11;
                    reg = (buff[idx + bytesRead] >> 3) & 0b111;
                    rm = buff[idx + bytesRead] & 0b111;
                    bytesRead+=1;
            }

            //Get Disp if needed
            if(mod == 0b10){ // 2 extra byte displacement
                disp = (((uint16_t)buff[idx+ bytesRead + 1]) << 8) | (uint16_t)buff[idx + bytesRead];
                bytesRead += 2;
            }else if(mod == 0b01){ // 1 extra byte displacement
                disp = (uint16_t)buff[idx + bytesRead];
                 bytesRead += 1;
            }else if (mod == 0b00 && rm == 0b110){
                disp = (((uint16_t)buff[idx+ bytesRead + 1]) << 8) | (uint16_t)buff[idx + bytesRead];
                bytesRead += 2;
            }

            // Get Data if Immediate
            if(encoding[opIdx] == IMMEDIATE_TO_REGMEM || encoding[opIdx] == IMMEDIATE_TO_REG){
                if((sflag && !s && w) || (!sflag && w)){ // 2 extra byte displacement
                    data = (((uint16_t)buff[idx + bytesRead+1]) << 8) | (uint16_t)buff[idx + bytesRead];
                    bytesRead+=2;
                }else{ 
                    data = (uint16_t)buff[idx + bytesRead];
                    bytesRead+=1;
                }
            }

            if(encoding[opIdx] == MEMORY_TO_REG){
                if(w){ // 2 extra byte displacement
                    addr = (((uint16_t)buff[idx + bytesRead+1]) << 8) | (uint16_t)buff[idx + bytesRead];
                    bytesRead+=2;
                }else{ 
                    addr = (uint16_t)buff[idx + bytesRead];
                    bytesRead+=1;
                }
            }

            if(encoding[opIdx] == JUMPS){
                ip_inc8 = (int8_t)buff[idx + bytesRead];
                bytesRead+=1;
            }

            printf("inst Num: %i\n", instNum);
            printf("Byte idx: %i\n", idx);
            printf("bytes read: %i\n", bytesRead);
            printf("op idx: %i\n", opIdx);
            printf("encoding: %i\n", encoding[opIdx]);
            printf("\n");
            printf("mod : %i\n", mod);
            printf("reg : %i\n", reg);
            printf("rm : %i\n", rm);
            printf("wide : %d\n", w);
            printf("d : %d\n", d);
            printf("\n");
            printf("address: %i\n", addr);
            printf("address signed: %i\n", (int16_t)addr);
            printf("disp: %i\n", disp);
            printf("disp signed: %i\n", (int16_t)disp);
            printf("data: %i\n", data);
            printf("\n");
            printf("next Byte : %i\n", buff[idx + bytesRead]);
            printf("nextnext Byte : %i\n", buff[idx+ bytesRead+1]);
            printf("nextnextnext Byte : %i\n", buff[idx + bytesRead +2]);
            printf("\n\n\n");

            //Source string
            switch(encoding[opIdx]){
                case REGMEM_TO_REG:
                case IMMEDIATE_TO_REGMEM: //I think I flipped this so I only need it here
                    if(mod == 0b00 && rm == 0b110){
                        sprintf(dstStr, "[%i]", disp);
                    }else if(mod == 0b11){
                        sprintf(dstStr, "%s", registers[rm + (8 * w)]);
                    }else{
                        switch(rm & 0b100){
                            //todo can add the mask of 0b10 to 11 to get 13, and 0b1 to 14 for 15
                            case 0b000:
                                sprintf(dstStr, "[%s + %s", registers[11 + (rm & 0b10)], registers[14 + (rm & 0b1)]);
                            break;
                            case 0b100:
                                switch (rm & 0b10){ 
                                    case 0b00:
                                        sprintf(dstStr, "[%s", registers[14 + (rm & 0b1)]);
                                    break;
                                    case 0b10:
                                        sprintf(dstStr, "[%s", registers[13 - ((rm << 1) & 0b10)]);
                                    break;
                                }
                            break;
                        }
                        if(mod == 0b01 || mod == 0b10){
                            sprintf(dstStr, "%s + %i]", dstStr, disp);
                        }else{
                            sprintf(dstStr, "%s]", dstStr);
                        }
                    }
                break;

                case IMMEDIATE_TO_REG:
                    sprintf(dstStr, "%i", data);
                break;
                case MEMORY_TO_REG:
                    sprintf(dstStr, "[%i]", addr);
                break;
                case JUMPS:

                break;
            }

            //Get Src String, copy paste of dest, with some changes, not sure if I need it all, since dest only can be RM if d is set
            switch(encoding[opIdx]){
            case REGMEM_TO_REG:
            case MEMORY_TO_REG:
            case IMMEDIATE_TO_REG:
                if(accFlag){
                    sprintf(srcStr, "%s", registers[0 + (8 * w)]);
                }
                else{
                    sprintf(srcStr, "%s", registers[reg + (8 * w)]);
                }
            break;
            case IMMEDIATE_TO_REGMEM:
                sprintf(srcStr, "%i", data);
            break;

            case JUMPS:
            break;
            }



            if(aluOpFlag){
                int arithId = 0;
                //TODO: this can be simplified
                if(encoding[opIdx] == IMMEDIATE_TO_REGMEM){
                    arithId = (buff[idx+1] >> 3) & 0b00000111;
                }else{
                    arithId = (buff[idx] >> 3) & 0b00000111;
                }
                strcpy(opStr, arithStrs[arithId]);
            }else if(opIdx == 7){
                int jumpId = buff[idx] & 0b00001111;
                strcpy(opStr, jumpStrs[jumpId]);
            }else if(opIdx == 8){
                int loopId = buff[idx] & 0b00000011;
                strcpy(opStr, loopStrs[loopId]);
            }else{
                strcpy(opStr, ops[opIdx]);
            }

            instNum++; 
            idx += bytesRead;
            if(encoding[opIdx] == JUMPS){
                fprintf(fileOut, "%s %d\n",opStr, ip_inc8);
            }
            else if(d){
                fprintf(fileOut, "%s %s, %s \n",opStr, srcStr, dstStr);
            }else{
                fprintf(fileOut, "%s %s, %s \n",opStr, dstStr, srcStr);
            }


        }
    //}
    fclose(file);
    fclose(fileOut);
    return 0;
}

