#pragma once
#include <string>

static bool try_parse_int(const std::string& str) {
    if (str.empty()) return false;

    int stop_index = 0;

    if (str[0] == '-') {
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }

    for (int index = (int)str.length() - 1; index >= stop_index; index--) {
        char c = str[index];
        if (!(c >= 48 && c <= 57)) {
            return false;
        }
    }

    return true;
}

static bool try_parse_hex(const std::string& str) {
    if (str.length() < 2) return false;
    if (str[0] != '0' || str[1] != 'x') return false;

    for (size_t index = str.length() - 1; index > 1; index--) {
        char c = str[index];
        if (!(c >= 48 && c <= 57) || !(c >= 65 && c <= 70) || !(c >= 97 && c <= 102)) {
            return false;
        }
    }

    return true;
}

static bool try_parse_dbl(const std::string& str) {
    if (str.empty()) return false;

    int stop_index = 0;

    if (str[0] == '-') {
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }

    int dec_place_index = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '.') {
            dec_place_index = i;
            break;
        }
    }

    if (dec_place_index == -1) {
        return try_parse_int(str);
    }

    for (int index = dec_place_index - 1; index >= stop_index; index--) {
        char c = str[index];
        if (!(c >= 48 && c <= 57)) {
            return false;
        }
    }

    for (int index = dec_place_index + 1; index < str.length(); index++) {
        char c = str[index];
        if (!(c >= 48 && c <= 57)) {
            return false;
        }
    }

    return true;
}

static bool try_parse_flt(const std::string& str) {
    if (str.empty()) return false;
    if (str.back() != 'f' && str.back() != 'F')
        return false;

    int stop_index = 0;

    if (str[0] == '-') {
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }

    int dec_place_index = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '.') {
            dec_place_index = i;
            break;
        }
    }

    if (dec_place_index == -1) {
        return try_parse_int(str);
    }

    for (int index = dec_place_index - 1; index >= stop_index; index--) {
        char c = str[index];
        if (!(c >= 48 && c <= 57)) {
            return false;
        }
    }

    for (int index = dec_place_index + 1; index < str.length() - 1; index++) {
        char c = str[index];
        if (!(c >= 48 && c <= 57)) {
            return false;
        }
    }

    return true;
}

static bool try_parse_bin(const std::string& str) {
    if (str.length() < 2)
        return false;
    if (str[0] != '0' || str[1] != 'b')
        return false;

    for (size_t index = str.length() - 1; index > 1; index--) {
        char c = str[index];
        if (c != '1' && c != '0') {
            return false;
        }
    }
    return true;
}



static bool try_parse_int(const std::string& str, int* result) {
    if (str.empty()) return false;

    bool is_negative = false;
    int stop_index = 0;

    if (str[0] == '-') {
        is_negative = true;
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }
    
    int factor = 1;
    *result = 0;

    for (int index = (int)str.length()-1; index >= stop_index; index--) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 10;
        }
        else {
            return false;
        }
    }

    if (is_negative)
        *result *= -1;

    return true;
}

static bool try_parse_hex(const std::string& str, uint32_t* result) {
    if (str.length() < 2) return false;
    if (str[0] != '0' || str[1] != 'x') return false;

    uint32_t factor = 1;
    *result = 0;

    for (size_t index = str.length()-1; index > 1; index--) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 16;
        }
        else if (c >= 65 && c <= 70) {
            *result += (c - 55) * factor;
            factor *= 16;
        }
        else if (c >= 97 && c <= 102) {
            *result += (c - 87) * factor;
            factor *= 16;
        }
        else {
            return false;
        }
    }

    return true;
}

static bool try_parse_dbl(const std::string& str, double* result) {
    if (str.empty()) return false;

    bool is_negative = false;
    int stop_index = 0;

    if (str[0] == '-') {
        is_negative = true;
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }
 
    int dec_place_index = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '.') {
            dec_place_index = i;
            break;
        }
    }

    if (dec_place_index == -1) {
        int tmp_result = 0;
        if (try_parse_int(str, &tmp_result)) {
            *result = (double)tmp_result;
            return true;
        }
        else {
            return false;
        }
    }


    double factor = 1.0;
    *result = 0;

    for (int index = dec_place_index - 1; index >= stop_index; index--) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 10.0;
        }
        else {
            return false;
        }
    }

    factor = 0.1;
    for (int index = dec_place_index + 1; index < str.length(); index++) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 0.1;
        }
        else {
            return false;
        }
    }

    if (is_negative)
        *result *= -1;

    return true;
}

static bool try_parse_flt(const std::string& str, float* result) {
    if (str.empty()) return false;
    if (str.back() != 'f' && str.back()!='F')
        return false;

    bool is_negative = false;
    int stop_index = 0;

    if (str[0] == '-') {
        is_negative = true;
        stop_index = 1;
    }
    else if (str[0] == '+') {
        stop_index = 1;
    }

    int dec_place_index = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '.') {
            dec_place_index = i;
            break;
        }
    }

    if (dec_place_index == -1) {
        return try_parse_int(str);
    }


    float factor = 1.0f;
    *result = 0;

    for (int index = dec_place_index - 1; index >= stop_index; index--) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 10.0f;
        }
        else {
            return false;
        }
    }

    factor = 0.1f;
    for (int index = dec_place_index + 1; index < str.length()-1; index++) {
        char c = str[index];
        if (c >= 48 && c <= 57) {
            *result += (c - 48) * factor;
            factor *= 0.1f;
        }
        else {
            return false;
        }
    }

    if (is_negative)
        *result = -*result;;

    return true;
}

static bool try_parse_bin(const std::string& str, uint32_t* result) {
    if (str.length() < 2) 
        return false;
    if (str[0] != '0' || str[1] != 'b')
        return false;

    *result = 0;
    int power = 0;

    for (size_t index = str.length() - 1; index > 1; index--) {
        char c = str[index];
        if (c == '1') {
            *result += 1 << power;
        }
        else if(c != '0') {
            return false;
        }
        power++;
    }
    return true;
}



static bool is_numeric(const std::string& str) {
    if (try_parse_int(str)) return true;
    if (try_parse_dbl(str)) return true;
    if (try_parse_flt(str)) return true;
    if (try_parse_hex(str)) return true;
    if (try_parse_bin(str)) return true;
    return false;
}
