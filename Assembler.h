#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <fstream>
#include <iostream>
#include <bitset>

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

    std::unordered_map<std::string, uint16_t> symbols =
    {
        {"nop",0}, {"hlt",1},{"add",2},{"sub",3},{"nor",4},{"and",5},{"xor",6},{"rsh",7},{"ldi",8},{"adi",9},{"jmp",10},{"brh",11},{"cal",12},{"ret",13},{"lod",14},{"str",15},

        {"r0",0},{"r1",1},{"r2",2},{"r3",3},{"r4",4},{"r5",5},{"r6",6},{"r7",7},{"r8",8},{"r9",9},{"r10",10},{"r11",11},{"r12",12},{"r13",13},{"r14",14},{"r15",15},

        {"eq"  ,0}, {"ne"     ,1}, {"ge"   ,2}, {"lt"      ,3},
        {"="   ,0}, {"!="     ,1}, {">="   ,2}, {"<"       ,3},
        {"z"   ,0}, {"nz"     ,1}, {"c"    ,2}, {"nc"      ,3},
        {"zero",0}, {"notzero",1}, {"carry",2}, {"notcarry",3},

        {"pixel_x"            ,240},{"pixel_y"      ,241},{"draw_pixel"  ,242},{"clear_pixel"       ,243},{"load_pixel" ,244},{"buffer_screen",245},
        {"clear_screen_buffer",246},{"write_char"   ,247},{"buffer_chars",248},{"clear_chars_buffer",249},{"show_number",250},{"clear_number" ,251},
        {"signed_mode"        ,252},{"unsigned_mode",253},{"rng"         ,254},{"controller_input"  ,255},

        {"' '" ,0},{"'a'",1},{"'b'",2},{"'c'",3},{"'d'",4},{"'e'",5},{"'f'",6},{"'g'",7},{"'h'",8},{"'i'",9},{"'j'",10},{"'k'",11},{"'l'",12},{"'m'",13},{"'n'",14},{"'o'",15},
        {"'p'",16},{"'q'",17},{"'r'",18},{"'s'",19},{"'t'",20},{"'u'",21},{"'v'",22},{"'w'",23},{"'x'",24},{"'y'",25},{"'z'",26},{"'.'",27},{"'!'",28},{"'?'",29},

        {"\" \"",0},{"\"a\"",1},{"\"b\"",2},{"\"c\"",3},{"\"d\"",4},{"\"e\"",5},{"\"f\"",6},{"\"g\"",7},{"\"h\"",8},{"\"i\"",9},{"\"j\"",10},{"\"k\"",11},{"\"l\"",12},{"\"m\"",13},{"\"n\"",14},{"\"o\"",15},
        {"\"p\"",16},{"\"q\"",17},{"\"r\"",18},{"\"s\"",19},{"\"t\"",20},{"\"u\"",21},{"\"v\"",22},{"\"w\"",23},{"\"x\"",24},{"\"y\"",25},{"\"z\"",26},{"\".\"",27},{"\"!\"",28},{"\"?\"",29}
    };

    std::function<uint16_t(std::string)> resolve = [&symbols](std::string obj)
        {
            if (obj[0] == 45 || (obj[0] >= 48 && obj[0] <= 57)) { // obj[0] in "-0123456789"
                //numeric
                if (obj.length() >= 2 && obj.substr(0, 2) == "0b") {
                    //binary 
                    return (uint16_t)std::stoi(obj.substr(2, obj.length() - 2), nullptr, 2);
                }
                else if (obj.length() >= 2 && obj.substr(0, 2) == "0x") {
                    //hex
                    return (uint16_t)std::stoi(obj.substr(2, obj.length() - 2), nullptr, 16);
                }
                else {
                    //dec
                    return (uint16_t)std::stoi(obj, nullptr, 0);
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
        while (end_index > 0 && (line[end_index] == ' ' || line[end_index] == '\t')) end_index--;

        //tokenize and preprocessor
        std::vector<std::string> tokens;
        std::string token;

        bool in_quotes = false;
        for (char c : line.substr(start_index, end_index - start_index + 1)) {
            if (c == '/' || c == '#' || c == ';') {
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

        uint8_t opcode = (uint8_t)symbols[instruction[0]];
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
        if (ins_index < instructions.size() - 1) machine_code_file << '\n';

        ins_index++;
    }

    //display code
    uint16_t line_index = 0;
    std::cout << "LINE " << str_pad_right("INSTRUCTION", MAX_LEN_INSTRUCTION) << " MACHINE CODE\n";
    for (const std::vector<std::string>& instruction : instructions) {

        if (jmp_location_names.find(line_index) != jmp_location_names.end()) {
            //insert jump location
            std::cout << jmp_location_names[line_index] << '\n';
        }

        //insert instruction
        std::cout << str_pad_right(std::to_string(line_index + 1), 4) << " ";
        std::string instruction_str = "";
        for (std::string s : instruction) instruction_str += s + ' ';
        std::cout << str_pad_right(instruction_str, MAX_LEN_INSTRUCTION) << " ";
        std::cout << std::bitset<16>(machine_code_instructions[line_index]).to_string() << "\n";

        line_index++;
    }
}

