#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


//TODO: couple bugs to check out, just moving on for now
//Listing 39, either misses the last instruction, or writes a malformed if I do outer while loop for read file
//Check values that are output, immediate/addresses some dont seem right, but otherwise the overall logic is working
//
enum memType{
    REGISTER,
    REGISTER_MEMORY,
    DIRECT_ADDRESS,
    IMMEDIATE
};

const char *ops[64];
char masks[64];
char codes[64];
const char *registers[16];

void setup()
{
    //Is there a better way to represent this? or will it make sense in the future?
    //would it be better to access as array search vs something like dict
    //Ideally I could index off the mask result, but the variable length mask isn't ideal? maybe? maybe arr[255] would work in the future, but I'll get to that when I get farther
    ops[0] = "mov"; //  register/memory to/ from register
    masks[0] = 0b11111100;
    codes[0] = 0b10001000;

    ops[1] = "mov"; // immediate to register/memory
    masks[1] = 0b11111110;
    codes[1] = 0b11000110;

    ops[2] = "mov"; // immediate to register
    masks[2] = 0b11110000;
    codes[2] = 0b10110000;

    ops[3] = "mov"; // Memory to accumulator
    masks[3] = 0b11111110;
    codes[3] = 0b10100000;

    ops[4] = "mov"; // accumulator to memory
    masks[4] = 0b11111110;
    codes[4] = 0b10100010;

    ops[5] = "mov"; // Register/memory to segment register
    masks[5] = 0b11111111;
    codes[5] = 0b10001110;

    ops[6] = "mov"; // segment register to register/memory
    masks[6] = 0b11111111;
    codes[6] = 0b10001100;
                            
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
}


int main(int argc, char **argv){
    setup();
    if(argc != 3){
        printf("need 3 args");
        return 1;
    }

    //Read binary to buffer
    char buff[100];
    FILE* file = fopen(argv[1], "rb");
    FILE* fileOut = fopen(argv[2], "w");
    int instNum = 0;
    while(fgets(buff, 100, file) != NULL){
    //fread(buff, sizeof(buff[0], 200, file));

    //Write Assembly to new file
    int idx = 0;
    while(buff[idx] != '\0'){
        //Get Operation
        int opIdx;
        for(opIdx = 0; opIdx < 64; opIdx++){
            //printf("buff: %i | mask: %i | res: %i | code: %i\n", (uint8_t)buff[idx], (uint8_t)masks[opIdx], (uint8_t)(buff[idx] & masks[opIdx]), (uint8_t)codes[opIdx]);
            if((buff[idx] & masks[opIdx]) == codes[opIdx]){
                break;
            }
        }
        //printf("opIdx: %i\n", opIdx);
        bool d, w;
        //TODO mod is always at the start of 2nd byte if it's there, might be able to pull it out of the switch
        int mod = 0b11, reg, rm;
        enum memType dst, src;
        char dstStr[20], srcStr[20];
        uint16_t disp, data, addr;

        //Get Data from Byte data based on operation
        //TODO: refactor reading 1-2 byte data/disp/addr
        switch(opIdx){
            case 0: //Register/Memory to/from register
                d = (buff[idx] & 0b10);
                w = buff[idx] & 0b1;
                mod = (buff[idx + 1] >> 6) & 0b11;
                reg = (buff[idx + 1] >> 3) & 0b111;
                rm = buff[idx + 1] & 0b111;
                idx+=2;

                src = REGISTER;
                if(mod == 0b00){
                    if(rm == 0b110){
                        dst = DIRECT_ADDRESS;
                    }else{
                        dst = REGISTER_MEMORY;
                    }

                }else if(mod == 0b01 || mod == 0b10){
                        dst = REGISTER_MEMORY;
                }else if(mod == 0b11){
                        dst = REGISTER;
                }
                break;
            case 1:// immediate to register/memory
                w = buff[idx] & 0b1;
                rm = buff[idx+1] & 0b111;
                mod = buff[idx + 1] >> 6 & 0b11;
                idx+=2;

                src = IMMEDIATE;
                if(mod == 0b00){
                    if(rm == 0b110){
                        dst = DIRECT_ADDRESS;
                    }else{
                        dst = REGISTER_MEMORY;
                    }
                }else if(mod == 0b01 || mod == 0b10){
                        dst = REGISTER_MEMORY;
                }else if(mod == 0b11){
                        dst = REGISTER;
                }
                break;
            case 2:// immediate to register
                w = buff[idx] & 0b1000;
                reg = buff[idx] & 0b111;
                src = REGISTER;
                dst = IMMEDIATE;
                d = true;
                idx+=1;
                break;
            case 3:// Memory to accumulator
            case 4:// Accumulator to memory
                src = DIRECT_ADDRESS;
                dst = REGISTER;
                rm = 0;
                d = opIdx == 4;
                idx+=1;
                break;
            //TODO: See how these work? dont think they are apart of the instructions so I cant test
            case 5:// Register/Memory to segment register
                mod = buff[idx + 1] >> 6 & 0b11;
                idx+=2;
                break;
            case 6:// Segment Register to register/memory
                mod = buff[idx + 1] >> 6 & 0b11;
                idx+=2;
                break;
            case 7:
                printf("unknown op id\n");
                idx+=1;
        }

        printf("State Post Byte Read\n");
        printf("inst Num: %i\n", instNum);
        printf("Byte idx: %i\n", idx);
        printf("op idx: %i\n", opIdx);
        printf("mod : %i\n", mod);
        printf("reg : %i\n", reg);
        printf("rm : %i\n", rm);
        printf("wide : %d\n", w);
        printf("d : %d\n", d);
        printf("dst type: %i\n", dst);
        printf("src type: %i\n", src);
        if(opIdx == 3 || opIdx == 4){
            printf("address: %i\n", addr);
            printf("address signed: %i\n", (int16_t)addr);
        }


        //Get Disp if needed
        if(mod == 0b10){ // 2 extra byte displacement
            disp = (((uint16_t)buff[idx+1]) << 8) | (uint16_t)buff[idx];
            idx += 2;
        }else if(mod == 0b01){ // 1 extra byte displacement
            disp = (uint16_t)buff[idx];
            idx += 1;
        }

        // Get Data if Immediate
        if(opIdx == 1 || opIdx == 2){
            printf("w: %d\n", w);
            if(w){ // 2 extra byte displacement
                data = (((uint16_t)buff[idx+1]) << 8) | (uint16_t)buff[idx];
            }else{ 
                data = (uint16_t)buff[idx];
            }
            idx+=1 + w;
        }
        if(src == DIRECT_ADDRESS || dst == DIRECT_ADDRESS){
            if(w){ // 2 extra byte displacement
                addr = (((uint16_t)buff[idx+1]) << 8) | (uint16_t)buff[idx];
            }else{ 
                addr = (uint16_t)buff[idx];
            }
            idx+=1 + w;
        }

        printf("\nState Post Disp/Buffer/Address\n");
        printf("Byte idx: %i\n", idx);
        printf("disp: %i\n", disp);
        printf("disp signed: %i\n", (int16_t)disp);
        printf("data: %i\n\n\n", data);
        printf("next Byte : %i\n", buff[idx]);
        printf("nextnext Byte : %i\n", buff[idx+1]);
        printf("nextnextnext Byte : %i\n", buff[idx+2]);

        //TODO method this
        //Get dest string
        switch(dst){
        case REGISTER:
            sprintf(dstStr, "%s", registers[rm + (8 * w)]);
            break;
        case REGISTER_MEMORY:
    //registers[10] = "dx";
    //registers[11] = "bx";
    //registers[12] = "sp";
    //registers[13] = "bp";
    //registers[14] = "si";
    //registers[15] = "di";
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
            break;
        case DIRECT_ADDRESS:
            sprintf(dstStr, "[%i]", addr);
            break;
        case IMMEDIATE:
            sprintf(dstStr, "%i", data);
            break;
        }
        //Get Src String, copy paste of dest, with some changes, not sure if I need it all, since dest only can be RM if d is set
        switch(src){
        case REGISTER:
            sprintf(srcStr, "%s", registers[reg + (8 * w)]);
            break;
        //case REGISTER_MEMORY:
         //   switch(rm & 0b100){
                //todo can add the mask of 0b10 to 11 to get 13, and 0b1 to 14 for 15
          //      case 0b000:
           //         sprintf(srcStr, "[%s + %s", registers[11 + (rm & 0b10)], registers[14 + (rm & 0b1)]);
            //        break;
             //   case 0b100:
              //      sprintf(srcStr, "[%s", registers[14  - ((rm << 1) &0b100) + (rm & 0b1)]);
               //     break;
            //}
            //if(mod == 0b01 || mod == 0b10){
            //sprintf(srcStr, "%s + %i]", srcStr, disp);
            //}else{
            //sprintf(srcStr, "%s]", srcStr);
            //}
            //break;
        case DIRECT_ADDRESS:
            sprintf(srcStr, "[%i]", addr);
            break;
        case IMMEDIATE:
            sprintf(srcStr, "%i", data);
            break;
        }

        instNum++; 
        if(d){
            fprintf(fileOut, "%s %s, %s \n",ops[opIdx],srcStr , dstStr);
        }else{
            fprintf(fileOut, "%s %s, %s \n",ops[opIdx],dstStr , srcStr);
        }


    }
    for(int i = 0; i < 100; i++){
        buff[i] = 0;
    }
    }

    fclose(file);
    fclose(fileOut);
    return 0;
}


