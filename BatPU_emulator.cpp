#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <bitset>
#include <functional>
#include <unordered_set>
#include <stdexcept>
#include "Lexer.h"
#include "Parser.h"
#include "Assembler.h"

class BatPU
{
public:
    void run_program(uint16_t program[1024]) {
        memcpy(InstructionMemory, program, 1024 * sizeof(uint16_t));
        bool is_running = true;

        PC = 0;

        while (is_running) {
            uint16_t instruction = InstructionMemory[PC] >> 12;
            uint8_t opcode = instruction >> 12;
            uint8_t regA = (instruction & 0x0f00) >> 8;
            uint8_t regB = (instruction & 0x00f0) >> 4;
            uint8_t regC = (instruction & 0x000f);
            uint8_t offset = regC;
            uint8_t imm = (instruction & 0x00ff);
            uint16_t addr = (instruction & 0b0000001111111111);
            uint8_t cond = (instruction & 0b0000110000000000) >> 10;

            switch (opcode)
            {
            case 0:
            {
                NOP();
                break;
            }
            case 1:
            {
                HLT();
                is_running = false;
            }    break;
            case 2:
            {
                ADD(regA, regB, regC);
                break;
            }
            case 3:
            {
                SUB(regA, regB, regC);
                break;
            }
            case 4:
            {
                NOR(regA, regB, regC);
                break;
            }
            case 5:
            {
                AND(regA, regB, regC);
                break;
            }
            case 6:
            {
                XOR(regA, regB, regC);
                break;
            }
            case 7:
            {
                RSH(regA, regC);
                break;
            }
            case 8:
            {
                LDI(regA, imm);
                break;
            }
            case 9:
            {
                ADI(regA, imm);
                break;
            }
            case 10:
            {
                JMP(addr);
                break;
            }
            case 11:
            {
                BRH(cond, addr);
                break;
            }
            case 12:
            {
                CAL(addr);
                break;
            }
            case 13:
            {
                RET();
                break;
            }
            case 14:
            {
                LOD(regA, regB, offset);
                break;
            }
            case 15:
            {
                STR(regA, regB, offset);
                break;
            }
            default:
                is_running = false;
                break;
            }

            PC++;
            if (PC >= 1024) {
                is_running = false;
            }
        }
    }

    void print_state() const {
        std::cout << "PC:" << PC << "\tZ:" << std::to_string(Z) << "\tC:" << std::to_string(C) << '\n';
        std::cout << instruction_to_str(InstructionMemory[PC])  << "\n\n";
        int mem_index = 0;
        for (int i = 0; i < 16; i++) {
            std::cout << "R" << i << ((i < 10) ? " " : "") << ':' << to_hex(Registers[i], 8) << '\t';
            for (int j = 0; j < 16; j++) {
                std::cout << to_hex(DataMemory[mem_index], 8) << ' ';
                mem_index++;
            }
            std::cout << '\n';
        }
    }

private:
    void NOP() {}

    void HLT() {}

    void ADD(uint8_t regA, uint8_t regB, uint8_t regC) {
        Registers[regC] = Registers[regA] + Registers[regB] + C;
        Z = (Registers[regC] == 0);
        C = (Registers[regC] > 255);
    }

    void SUB(uint8_t regA, uint8_t regB, uint8_t regC) {
        Registers[regC] = Registers[regA] + ~Registers[regB] + C;
        Z = (Registers[regC] == 0);
        C = (Registers[regC] > 255);
    }

    void NOR(uint8_t regA, uint8_t regB, uint8_t regC) {
        Registers[regC] = !(Registers[regA] | Registers[regB]);
        Z = (Registers[regC] == 0);
    }

    void AND(uint8_t regA, uint8_t regB, uint8_t regC) {
        Registers[regC] = Registers[regA] & Registers[regB];
        Z = (Registers[regC] == 0);
    }

    void XOR(uint8_t regA, uint8_t regB, uint8_t regC) {
        Registers[regC] = Registers[regA] ^ Registers[regB];
        Z = (Registers[regC] == 0);
    }

    void RSH(uint8_t regA, uint8_t regC) {
        Registers[regC] = Registers[regA] >> 1;
    }

    void LDI(uint8_t regA, uint8_t imm) {
        Registers[regA] = imm;
    }

    void ADI(uint8_t regA, uint8_t imm) {
        Registers[regA] += imm;
        Z = (Registers[regA] == 0);
    }

    void JMP(uint16_t addr) {
        PC = addr;
    }

    void BRH(uint8_t cond, uint16_t addr) {
        if (cond == 0 && Z == 1) {
            PC = addr;
        }
        else if (cond == 1 && Z == 0) {
            PC = addr;
        }
        else if (cond == 2 && C == 1) {
            PC = addr;
        }
        else if (cond == 3 && C == 0) {
            PC = addr;
        }
        else {
            PC++;
        }
    }

    void CAL(uint16_t addr) {
        if (CallStack.size() + 1 > 16) {

        }

        CallStack.push_back(PC + 1);
        PC = addr;
    }

    void RET() {
        PC = CallStack.back();
        CallStack.pop_back();
    }

    void LOD(uint8_t regA, uint8_t regB, uint8_t offset) {
        Registers[regB] = DataMemory[regA + offset];
    }

    void STR(uint8_t regA, uint8_t regB, uint8_t offset) {
        DataMemory[regA + offset] = Registers[regB];
    }
   
private:
    static std::string to_hex(uint64_t bytes, uint8_t n_bits) {
        std::stringstream ss;
        static const std::string hex_chars = "0123456789ABCDEF";
        while (n_bits > 0) {
            ss << hex_chars[bytes & 0xF];
            bytes = bytes >> 4;
            n_bits -= 4;
        }
        return ss.str();
    }

    static std::string instruction_to_str(uint16_t instruction) {
        std::stringstream ins_str;
        uint8_t opcode = instruction >> 12;
        uint8_t regA = (instruction & 0x0f00) >> 8;
        uint8_t regB = (instruction & 0x00f0) >> 4;
        uint8_t regC = (instruction & 0x000f);
        uint8_t offset = regC;
        uint8_t imm = (instruction & 0x00ff);
        uint16_t addr = (instruction & 0b0000001111111111);
        uint8_t cond = (instruction & 0b0000110000000000) >> 10;

        static const std::string mnemonics[] = {"NOP", "HLT", "ADD", "SUB", "NOR", "AND","XOR","RSH","LDI","ADI","JMP","BRH","CAL","RET","LOD","STR"};
        
        if (opcode == 0 || opcode == 1 || opcode == 13) {
            ins_str << mnemonics[opcode];
        }
        else if (2 <= opcode && opcode <= 6) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(regA,4) << ' ' << to_hex(regB,4) << ' ' << to_hex(regC,4);
        }
        else if (opcode == 7) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(regA,4)<< ' ' << to_hex(regC,4);
        }
        else if (opcode == 8 || opcode == 9) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(regA,4) << ' ' << to_hex(imm,8);
        }
        else if (opcode == 10 || opcode == 12) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(addr, 10);
        }
        else if (opcode == 11) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(cond, 2) << ' ' << to_hex(addr, 10);
        }
        else if (opcode == 14 || opcode == 15) {
            ins_str << mnemonics[opcode] << ' ' << to_hex(regA, 4) << ' ' << to_hex(regB, 4) << ' ' << to_hex(offset, 4);
        }

        return ins_str.str();
    }

private:
    uint8_t  Registers[16] = {}; //16 registers where r0 is a zero register  
    uint16_t InstructionMemory[1024] = {};
    uint8_t  DataMemory[256] = {}; //MemoryMappedIO entries 240-255 of DataMemory are reserved
    uint32_t Screen[32] = {}; //32 rows of 32 cols of 1 bit screen pixels
    uint8_t  CharDisplay[10] = {}; //Character display that can display 10 characters
    std::vector<uint16_t> CallStack;

    //Number display, displays an 8bit singed or unsigned number
    //Inputs - start, select, A, B, up, right, down, left

    uint8_t Z = 0;  //Z flag
    uint8_t C = 0;  //C flag
    uint16_t PC = 0; //should be 10bits according to the specifications by MattBatWings
};


static void compile(const std::string& filename) {
    //tokenize the .c file into a vector of tokens
    std::vector<TOKEN> tokens;
    tokenize(filename, tokens);

    //create a tree of the code flow
    PARSE_TREE parse_tree;
    parse(tokens, parse_tree);

    __debugbreak();
    //turn the tree into assembly code

    //Turn the .as file into .mc and .bin files
    //assemble(filename);
}


int main()
{
    compile("parse_test.c");
}
