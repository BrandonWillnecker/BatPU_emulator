#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <bitset>
#include <functional>

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
            }
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


static void to_lower(std::string& s) {
    for (int i = 0; i < s.size(); i++) {
        s[i] = tolower(s[i]);
    }
}

static std::string str_pad_right(const std::string& str, size_t total_length) {
    if (str.length() >= total_length) return str;
    return str + std::string(total_length - str.length(), ' ');
}

static void assemble(const std::string& filename) {
    std::ifstream assembly_file(filename + ".as");
    std::ofstream machine_code_file(filename + ".mc");
    std::ofstream machine_code_bin_file(filename + ".bin");

    std::unordered_map<std::string, uint16_t> symbols = {
        {"nop",0}, {"hlt",1},{"add",2},{"sub",3},{"nor",4},{"and",5},{"xor",6},{"rsh",7},{"ldi",8},{"adi",9},{"jmp",10},{"brh",11},{"cal",12},{"ret",13},{"lod",14},{"str",15},

        {"r0",0},{"r1",1},{"r2",2},{"r3",3},{"r4",4},{"r5",5},{"r6",6},{"r7",7},{"r8",8},{"r9",9},{"r10",10},{"r11",11},{"r12",12},{"r13",13},{"r14",14},{"r15",15},

        {"eq",0},  {"ne",1},     {"ge",2},   {"lt",      3},
        {"=",0},   {"!=",1},     {">=",2},   {"<",       3},
        {"z",0},   {"nz",1},     {"c",2},    {"nc",      3},
        {"zero",0},{"notzero",1},{"carry",2},{"notcarry",3},

        {"pixel_x",240},{"pixel_y",241},{"draw_pixel",242},{"clear_pixel",243},{"load_pixel",244},{"buffer_screen",245},{"clear_screen_buffer",246},{"write_char",247},
        {"buffer_chars",248},{"clear_chars_buffer",249},{"show_number",250},{"clear_number",251},{"signed_mode",252},{"unsigned_mode",253},{"rng",254},{"controller_input",255},

        {"' '",0},{"'a'",1},{"'b'",2},{"'c'",3},{"'d'",4},{"'e'",5},{"'f'",6},{"'g'",7},{"'h'",8},{"'i'",9},{"'j'",10},{"'k'",11},{"'l'",12},{"'m'",13},{"'n'",14},{"'o'",15},
        {"'p'",16},{"'q'",17},{"'r'",18},{"'s'",19},{"'t'",20},{"'u'",21},{"'v'",22},{"'w'",23},{"'x'",24},{"'y'",25},{"'z'",26},{"'.'",27},{"'!'",28},{"'?'",29},

        {"\" \"",0},{"\"a\"",1},{"\"b\"",2},{"\"c\"",3},{"\"d\"",4},{"\"e\"",5},{"\"f\"",6},{"\"g\"",7},{"\"h\"",8},{"\"i\"",9},{"\"j\"",10},{"\"k\"",11},{"\"l\"",12},{"\"m\"",13},{"\"n\"",14},{"\"o\"",15},
        {"\"p\"",16},{"\"q\"",17},{"\"r\"",18},{"\"s\"",19},{"\"t\"",20},{"\"u\"",21},{"\"v\"",22},{"\"w\"",23},{"\"x\"",24},{"\"y\"",25},{"\"z\"",26},{"\".\"",27},{"\"!\"",28},{"\"?\"",29} };

    std::function<uint16_t(std::string)> resolve = [&symbols](std::string obj) 
        {
            if (obj[0] == 45 || (obj[0] >= 48 && obj[0] <= 57)) { // obj[0] in "-0123456789"
                //numeric
                if (obj.length() >= 2 && obj.substr(0, 2) == "0b") {
                    //binary 
                    return (uint16_t)std::stoi(obj.substr(2,obj.length()-2), nullptr, 2);
                }
                else if (obj.length() >= 2 && obj.substr(0, 2) == "0x") {
                    //hex
                    return (uint16_t)std::stoi(obj.substr(2, obj.length() - 2), nullptr,16);
                }
                else {
                    //dec
                    return (uint16_t)std::stoi(obj, nullptr,0);
                }
                
            }
            return symbols[obj];
        };

    std::vector<std::vector<std::string>> instructions;
    std::unordered_map<uint16_t, std::string> jmp_location_names;
    std::string line;
    uint16_t pc = 0;

    while (getline(assembly_file, line)) {
        if (line.length() == 0) continue;

        //skip all leading space and trailing space
        size_t start_index = 0;
        size_t end_index = line.length() - 1;
        while (line[start_index] == ' ' || line[start_index] == '\t') start_index++;
        while (end_index>0 && (line[end_index] == ' ' || line[end_index] == '\t')) end_index--;

        //tokenize and preprocessor
        std::vector<std::string> tokens;
        std::string token; 

        bool in_quotes = false;
        for (char c : line.substr(start_index, end_index - start_index + 1)) {
            if (c == '/' || c== '#' || c == ';') {
                break;
            }
            else if (c == ' ' && !in_quotes) {
                if (token != "") {
                    to_lower(token);
                    tokens.push_back(token);
                }  
                token = "";
            }
            else if (c == ' ' && in_quotes) {
                token += c;
            }
            else if (c == 0x27 || c == 0x22) {
                in_quotes = !in_quotes;
                token += c;
            }
            else {
                token += c;
            }
        }
        if (token != "") {
            to_lower(token);
            tokens.push_back(token);
        }

        if (tokens.size() > 0) {
            if (tokens[0] == "define") {
                //definition
                symbols[tokens[1]] = std::stoi(tokens[2]);
            }
            else if (tokens[0][0] == '.') {
                //label
                symbols[tokens[0]] = pc;
                jmp_location_names[pc] = tokens[0];
                if (tokens.size() > 1) {
                    pc++;
                    instructions.push_back(std::vector<std::string>(tokens.begin() + 1, tokens.end()));
                }
            }//pseudo instructions
            else if (tokens[0] == "cmp") { 
                pc++;
                instructions.push_back({ "sub",tokens[1],tokens[2],"r0" });
            }
            else if (tokens[0] == "mov") {
                pc++;
                instructions.push_back({ "add",tokens[1],"ro",tokens[2] });
            }
            else if (tokens[0] == "lsh") {
                pc++;
                instructions.push_back({ "add",tokens[1],tokens[1],tokens[2] });
            }
            else if (tokens[0] == "inc") {
                pc++;
                instructions.push_back({ "adi",tokens[1],"1" });
            }
            else if (tokens[0] == "dec") {
                pc++;
                instructions.push_back({ "adi",tokens[1],"-1" });
            }
            else if (tokens[0] == "not") {
                pc++;
                instructions.push_back({ "nor",tokens[1],"r0",tokens[2] });
            }
            else if (tokens[0] == "neg") {
                pc++;
                instructions.push_back({ "sub","ro",tokens[1],tokens[2] });
            }
            else if ((tokens[0] == "lod" || tokens[0] == "str") && tokens.size() == 3) {
                //lod/str optional offsets
                tokens.push_back("0");
                instructions.push_back(tokens);
                pc++;
            }
            else {
                //add instruction
                instructions.push_back(tokens);
                pc++;
            }
        }  
    }

    size_t MAX_LEN_INSTRUCTION = 0;

    //generate machine code
    std::vector<uint16_t> machine_code_instructions;
    machine_code_instructions.reserve(instructions.size());
    size_t ins_index = 0;
    for (const std::vector<std::string>& instruction : instructions) {
        size_t ins_len = 0;
        for (const std::string& s : instruction) ins_len += s.length() + 1;
        if (ins_len > MAX_LEN_INSTRUCTION) MAX_LEN_INSTRUCTION = ins_len;

        uint8_t opcode = symbols[instruction[0]];
        uint16_t machine_code = opcode << 12;
        if (opcode >= 2 && opcode <= 6) {
            machine_code |= resolve(instruction[1]) << 8 | resolve(instruction[2]) << 4 | resolve(instruction[3]);
        }
        else if (opcode == 7) {
            machine_code |= resolve(instruction[1]) << 8 | resolve(instruction[2]);
        }
        else if (opcode == 8 || opcode == 9) {
            machine_code |= resolve(instruction[1]) << 8 | (resolve(instruction[2]) & 255);
        }
        else if (opcode == 10 || opcode == 12) {
            machine_code |= resolve(instruction[1]);
        }
        else if (opcode == 11) {
            machine_code |= resolve(instruction[1]) << 10 | resolve(instruction[2]);
        }
        else if (opcode == 14 || opcode == 15) {
            machine_code |= resolve(instruction[1]) << 8 | resolve(instruction[2]) << 4 | (resolve(instruction[3]) & 15);
        }   

        machine_code_instructions.push_back(machine_code);

        machine_code_bin_file.write((char*)(&machine_code), sizeof(uint16_t));
        machine_code_file << std::bitset<16>(machine_code).to_string();
        if (ins_index < instructions.size()-1) machine_code_file << '\n';

        ins_index++;
    }

    //display code
    uint16_t line_index = 0;
    std::cout << "LINE " << str_pad_right("INSTRUCTION",MAX_LEN_INSTRUCTION) << " MACHINE CODE\n";
    for (const std::vector<std::string>& instruction : instructions) {

        if (jmp_location_names.find(line_index) != jmp_location_names.end()) {
            //insert jump location
            std::cout << jmp_location_names[line_index] << '\n';
        }

        //insert instruction
        std::cout << str_pad_right(std::to_string(line_index+1), 4) << " ";
        std::string instruction_str = "";
        for (std::string s : instruction) instruction_str += s + ' ';
        std::cout << str_pad_right(instruction_str, MAX_LEN_INSTRUCTION) << " ";
        std::cout << std::bitset<16>(machine_code_instructions[line_index]).to_string() << "\n";

        line_index++;
  
    }
}


int main()
{
    assemble("minesweeper");
}
