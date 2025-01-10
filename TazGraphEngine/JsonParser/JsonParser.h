#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>

enum class JsonType {
    Object, Array, String, Number, Boolean, Null
};

struct JsonValue {
    JsonType type;
    std::map<std::string, JsonValue> obj;
    std::vector<JsonValue> arr;
    std::string str;
    double num;
    bool boolean;
};

class JsonParser {
public:
    explicit JsonParser(const std::string& input) : input(input), pos(0) {}

    explicit JsonParser(const char* filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file.");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        input = buffer.str();
        pos = 0;
        file.close();
    }

    JsonValue parse() {
        JsonValue value = parseValue();
        skipWhitespace();
        if (!eof()) {
            throw std::runtime_error("Unexpected characters at end of input.");
        }
        return value;
    }

private:
    std::string input;
    size_t pos;

    void skipWhitespace() {
        while (!eof() && std::isspace(input[pos])) {
            pos++;
        }
    }

    bool eof() const {
        return pos >= input.size();
    }

    char peek() const {
        return input[pos];
    }

    char consume() {
        return input[pos++];
    }

    JsonValue parseValue() {
        skipWhitespace();
        if (eof()) {
            throw std::runtime_error("Unexpected end of input.");
        }

        char ch = peek();
        if (ch == '{') return parseObject();
        if (ch == '[') return parseArray();
        if (ch == '\"') return parseString();
        if (std::isdigit(ch) || ch == '-') return parseNumber();
        if (ch == 't' || ch == 'f') return parseBoolean();
        if (ch == 'n') return parseNull();

        throw std::runtime_error("Unexpected character.");
    }

    JsonValue parseObject() {
        consume(); // '{'
        JsonValue obj;
        obj.type = JsonType::Object;

        skipWhitespace();
        if (peek() == '}') {
            consume();
            return obj;
        }

        while (true) {
            skipWhitespace();
            JsonValue key = parseString();
            skipWhitespace();
            if (consume() != ':') {
                throw std::runtime_error("Expected ':' after key in object.");
            }
            JsonValue value = parseValue();
            obj.obj[key.str] = value;

            skipWhitespace();
            char ch = consume();
            if (ch == '}') break;
            if (ch != ',') {
                throw std::runtime_error("Expected ',' or '}' in object.");
            }
        }
        return obj;
    }

    JsonValue parseArray() {
        consume(); // '['
        JsonValue array;
        array.type = JsonType::Array;

        skipWhitespace();
        if (peek() == ']') {
            consume();
            return array;
        }

        while (true) {
            JsonValue value = parseValue();
            array.arr.push_back(value);

            skipWhitespace();
            char ch = consume();
            if (ch == ']') break;
            if (ch != ',') {
                throw std::runtime_error("Expected ',' or ']' in array.");
            }
        }
        return array;
    }

    JsonValue parseString() {
        std::ostringstream result;
        consume(); // the initial '"'
        while (true) {
            if (eof()) throw std::runtime_error("Unexpected end of input during string parsing.");

            char ch = consume();
            if (ch == '\"') break; // end of string
            if (ch == '\\') { // handle escapes
                if (eof()) throw std::runtime_error("Unexpected end of input during escape sequence.");
                char esc = consume();
                switch (esc) {
                case '\"': result << '\"'; break;
                case '\\': result << '\\'; break;
                case '/':  result << '/'; break;
                case 'b':  result << '\b'; break;
                case 'f':  result << '\f'; break;
                case 'n':  result << '\n'; break;
                case 'r':  result << '\r'; break;
                case 't':  result << '\t'; break;
                default: throw std::runtime_error("Invalid escape sequence.");
                }
            }
            else {
                result << ch;
            }
        }

        JsonValue strValue;
        strValue.type = JsonType::String;
        strValue.str = result.str();
        return strValue;
    }

    JsonValue parseNumber() {
        std::ostringstream result;
        if (peek() == '-') {
            result << consume();
        }

        while (!eof() && (std::isdigit(peek()) || peek() == '.')) {
            result << consume();
        }

        JsonValue number;
        number.type = JsonType::Number;
        number.num = std::stod(result.str());
        return number;
    }

    JsonValue parseBoolean() {
        std::ostringstream result;
        for (int i = 0; i < 4 && !eof(); ++i) { // "true" or "fals"
            result << consume();
        }

        JsonValue boolValue;
        boolValue.type = JsonType::Boolean;
        std::string res = result.str();
        if (res == "true") {
            boolValue.boolean = true;
        }
        else if (res == "false") {
            boolValue.boolean = false;
        }
        else {
            throw std::runtime_error("Invalid value for boolean.");
        }
        return boolValue;
    }

    JsonValue parseNull() {
        std::string result;
        for (int i = 0; i < 4 && !eof(); ++i) { // "null"
            result += consume();
        }
        if (result != "null") {
            throw std::runtime_error("Invalid value for null.");
        }
        JsonValue nullValue;
        nullValue.type = JsonType::Null;
        return nullValue;
    }
};