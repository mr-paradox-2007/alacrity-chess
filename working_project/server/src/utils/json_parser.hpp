#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

class json_value {
public:
    enum type { null_type, bool_type, number_type, string_type, array_type, object_type };
    
    type value_type;
    bool bool_val;
    double number_val;
    std::string string_val;
    std::vector<json_value> array_val;
    std::unordered_map<std::string, json_value> object_val;
    
    json_value() : value_type(null_type), bool_val(false), number_val(0) {}
};

class json_parser {
public:
    static json_value parse(const std::string& json_str);
    static std::string stringify(const json_value& val);
    
private:
    static json_value parse_value(const std::string& str, size_t& pos);
    static json_value parse_object(const std::string& str, size_t& pos);
    static json_value parse_array(const std::string& str, size_t& pos);
    static json_value parse_string(const std::string& str, size_t& pos);
    static json_value parse_number(const std::string& str, size_t& pos);
    static void skip_whitespace(const std::string& str, size_t& pos);
};

inline void json_parser::skip_whitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && std::isspace(str[pos])) pos++;
}

inline json_value json_parser::parse_number(const std::string& str, size_t& pos) {
    size_t start = pos;
    if (str[pos] == '-') pos++;
    while (pos < str.length() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == 'e' || str[pos] == 'E' || str[pos] == '+' || str[pos] == '-')) {
        pos++;
    }
    json_value val;
    val.value_type = json_value::number_type;
    val.number_val = std::stod(str.substr(start, pos - start));
    return val;
}

inline json_value json_parser::parse_string(const std::string& str, size_t& pos) {
    pos++;
    size_t start = pos;
    while (pos < str.length() && str[pos] != '"') {
        if (str[pos] == '\\') pos++;
        pos++;
    }
    json_value val;
    val.value_type = json_value::string_type;
    val.string_val = str.substr(start, pos - start);
    pos++;
    return val;
}

inline json_value json_parser::parse_array(const std::string& str, size_t& pos) {
    json_value val;
    val.value_type = json_value::array_type;
    pos++;
    skip_whitespace(str, pos);
    while (pos < str.length() && str[pos] != ']') {
        val.array_val.push_back(parse_value(str, pos));
        skip_whitespace(str, pos);
        if (str[pos] == ',') {
            pos++;
            skip_whitespace(str, pos);
        }
    }
    pos++;
    return val;
}

inline json_value json_parser::parse_object(const std::string& str, size_t& pos) {
    json_value val;
    val.value_type = json_value::object_type;
    pos++;
    skip_whitespace(str, pos);
    while (pos < str.length() && str[pos] != '}') {
        skip_whitespace(str, pos);
        if (str[pos] == '"') {
            json_value key_val = parse_string(str, pos);
            skip_whitespace(str, pos);
            if (str[pos] == ':') {
                pos++;
                skip_whitespace(str, pos);
                val.object_val[key_val.string_val] = parse_value(str, pos);
            }
        }
        skip_whitespace(str, pos);
        if (str[pos] == ',') {
            pos++;
        }
    }
    pos++;
    return val;
}

inline json_value json_parser::parse_value(const std::string& str, size_t& pos) {
    skip_whitespace(str, pos);
    if (str[pos] == '{') return parse_object(str, pos);
    if (str[pos] == '[') return parse_array(str, pos);
    if (str[pos] == '"') return parse_string(str, pos);
    if (str[pos] == 't' || str[pos] == 'f') {
        json_value val;
        val.value_type = json_value::bool_type;
        val.bool_val = (str[pos] == 't');
        pos += (val.bool_val ? 4 : 5);
        return val;
    }
    if (str[pos] == 'n') {
        pos += 4;
        json_value val;
        val.value_type = json_value::null_type;
        return val;
    }
    return parse_number(str, pos);
}

inline json_value json_parser::parse(const std::string& json_str) {
    size_t pos = 0;
    return parse_value(json_str, pos);
}

inline std::string json_parser::stringify(const json_value& val) {
    std::stringstream ss;
    if (val.value_type == json_value::null_type) {
        ss << "null";
    } else if (val.value_type == json_value::bool_type) {
        ss << (val.bool_val ? "true" : "false");
    } else if (val.value_type == json_value::number_type) {
        ss << val.number_val;
    } else if (val.value_type == json_value::string_type) {
        ss << "\"" << val.string_val << "\"";
    } else if (val.value_type == json_value::array_type) {
        ss << "[";
        for (size_t i = 0; i < val.array_val.size(); i++) {
            if (i > 0) ss << ",";
            ss << stringify(val.array_val[i]);
        }
        ss << "]";
    } else if (val.value_type == json_value::object_type) {
        ss << "{";
        bool first = true;
        for (const auto& [key, value] : val.object_val) {
            if (!first) ss << ",";
            ss << "\"" << key << "\":" << stringify(value);
            first = false;
        }
        ss << "}";
    }
    return ss.str();
}

#endif
