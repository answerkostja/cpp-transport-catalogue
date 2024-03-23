#include "json.h"

/*
 * Место для вашей JSON-библиотеки
 */

using namespace std;

namespace json {

    void PrintNode(const Node& node, std::ostream& out);

    void PrintValue(std::nullptr_t, std::ostream& out) {
        out << "null"sv;
    }

    void PrintValue(const bool& b, std::ostream& out) {
        if (b == true) {
            out << "true"sv;
        }
        else { out << "false"sv; }

    }

    void PrintValue(const std::string& s, std::ostream& out) {

        out << "\"";
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '\\') {
                out << "\\\\";
            }
            else if (s[i] == '\n') {
                out << "\\n";
            }
            else if (s[i] == '\r') {
                out << "\\r";
            }
            else if (s[i] == '\"') {
                out << "\\\"";
            }
            else if (s[i] == '\t') {
                out << "\\t";
            }
            else {
                out << s[i];
            }
        }

        out << "\"";
    }

    void PrintValue(const Array& array, std::ostream& out) {
        out << "["sv << std::endl;
        size_t size = 0;
        for (auto ar : array) {
            PrintNode(ar, out);
            size++;
            if (size != array.size()) {
                out << ", "sv << std::endl;
            }
        }
        out <<std::endl<< "]"sv;
    }

    void PrintValue(const Dict& dict, std::ostream& out) {
        out << "{"sv << std::endl;

        size_t size = 0;
        for (auto [key, value] : dict) {
            out << "\""sv << key << "\": "sv;
            PrintNode(value, out);
            size++;
            if (size != dict.size()) {
                out << ", "sv << std::endl;
            }
        }
        out << std::endl << "}"sv;
    }

    void PrintValue(const bool& value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }

    void PrintValue(const std::string& value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }

    void PrintValue(std::nullptr_t value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }

    void PrintValue(const Array& value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }

    void PrintValue(const Dict& value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }



    // Другие перегрузки функции PrintValue пишутся аналогично

    void PrintNode(const Node& node, std::ostream& out) {
        std::visit(
            [&out](const auto& value) { PrintValue(value, out); },
            node.GetValue());
    }

    bool operator==(Node left, Node right) {
        return left.GetValue() == right.GetValue();
    };

    bool operator!=(Node left, Node right) {
        return !(left == right);
    };

    Node::Node()
    {
        value_ = std::nullptr_t{ nullptr };
    }

    Node::Node(Value value)
        :value_(value) {
    }

    Node::Node(std::nullptr_t n)
        :value_(n) {};

    Node::Node(Array a)
        :value_(a) {};

    Node::Node(Dict m)
        :value_(m) {};

    Node::Node(bool b)
        :value_(b) {};

    Node::Node(int i)
        :value_(i) {};

    Node::Node(double d)
        :value_(d) {};

    Node::Node(std::string s) {
        if (s == "null"s) {
            value_ = std::nullptr_t{ nullptr };
        }
        else
            value_ = s;
    };


    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    };

    bool Node::IsDouble() const {
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);;
    };

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    };

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    };

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    };

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    };

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    };

    bool Node::IsDict() const {
        return std::holds_alternative<Dict>(value_);
    };

    int Node::AsInt() const {
        if (IsInt()) {
            return std::get<int>(value_);
        }
        else throw std::logic_error("value is not int");
    };

    bool Node::AsBool() const {
        if (IsBool()) {
            return std::get<bool>(value_);
        }
        else throw std::logic_error("value is not bool");
    };

    double Node::AsDouble() const {
        if (IsDouble()) {
            if (IsPureDouble()) {
                return std::get<double>(value_);
            }
            else return static_cast<double>(std::get<int>(value_));

        }
        else throw std::logic_error("value is not double");
    };

    const std::string& Node::AsString() const {
        if (IsString()) {
            return std::get<std::string>(value_);
        }
        else throw std::logic_error("value is not string");
    };

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(value_);
        }
        else throw std::logic_error("value is not Array");
    };

    const Dict& Node::AsDict() const {
        if (IsDict()) {
            return std::get<Dict>(value_);
        }
        else throw std::logic_error("value is not Dict");
    };

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            char c;
            for (; input >> c && c != ']';) {

                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }


            if (c != ']' && c != ',') {
                throw ParsingError("");
            }

            return Node(move(result));
        }

        Node LoadInt(istream& input) {
            int result = 0;
            bool not_number = true;
            bool is_negative_int = false;

            if (input.peek() == '-') {
                is_negative_int = true;
                input.get();
            }
            while (isdigit(input.peek())) {
                result *= 10;
                result += input.get() - '0';
                not_number = false;
            }
            double dresult = 0.;
            if (input.peek() == '.') {
                int after = 0;
                dresult += static_cast<double>(result);
                input.get();
                double fraction = 1.0 / 10.0;
                while (isdigit(input.peek())) {
                    dresult += (static_cast<double>(input.get() - '0') * fraction);
                    fraction /= 10.;
                    after++;
                }
                int ten = 1;
                for (int i = 0; i <= after; i++) {
                    ten *= 10;
                }
                dresult = round(dresult * ten) / ten;

                if (input.peek() == 'e' || input.peek() == 'E') {
                    int degree = 0;
                    input.get();
                    bool is_negative = false;

                    if (input.peek() == '-') {
                        is_negative = true;
                        input.get();
                    }
                    if (input.peek() == '+') {
                        is_negative = false;
                        input.get();
                    }
                    while (isdigit(input.peek())) {
                        degree *= 10;
                        degree += input.get() - '0';
                    }
                    double fraction2 = 1.0;
                    if (!is_negative) {
                        for (int i = degree; i != 0; i--) {
                            dresult *= 10;
                            ten *= 10;
                        }

                    }
                    else {
                        for (int i = degree; i != 0; i--) {

                            fraction2 /= 10.;
                            ten *= 10;
                        }
                        dresult *= fraction2;
                    }
                    dresult = round(dresult * ten) / ten;

                }

                if (is_negative_int) {
                    dresult *= (-1);
                }

                return Node(dresult);
            }

            if (input.peek() == 'e' || input.peek() == 'E') {
                dresult += static_cast<double>(result);
                int degree = 0;
                input.get();
                bool is_negative = false;

                if (input.peek() == '-') {
                    is_negative = true;
                    input.get();
                }
                if (input.peek() == '+') {
                    is_negative = false;
                    input.get();
                }

                while (isdigit(input.peek())) {
                    degree *= 10;
                    degree += input.get() - '0';
                }
                double fraction2 = 1.0;
                if (!is_negative) {
                    for (int i = degree; i != 0; i--) {
                        dresult *= 10;

                    }

                }
                else {
                    for (int i = degree; i != 0; i--) {

                        fraction2 /= 10.;

                    }
                    dresult *= fraction2;
                }
                if (is_negative_int) {
                    dresult *= (-1);
                }
                return Node(dresult);
            }

            if (is_negative_int) {
                result *= (-1);
            }




            if (not_number) {
                throw ParsingError("");
            }
            return Node(result);
        }

        Node LoadString(istream& input) {
            string line;
            getline(input, line, '"');
            if (input.eof()) {
                throw ParsingError("");
            }



            while (line[line.size() - 1] == '\\' && line[line.size() - 2] != '\\') {

                string line2;
                getline(input, line2, '"');
                line2.insert(0, 1, '\"');
                line += line2;
            }


            for (size_t i = 0; i < line.size(); i++) {
                if (line[i] == '\\') {
                    line.erase(i, 1);
                    if (line[i] == 't') { line[i] = '\t'; }
                    if (line[i] == 'n') { line[i] = '\n'; }
                    if (line[i] == 'r') { line[i] = '\r'; }
                }


            }
            return Node(move(line));
        }

        Node LoadNull(istream& input) {
            string line;

            char c;

            while (input >> c && c != ',' && c != '\t' && c != '\r' && c != '\n' && c != ' ' && c != '}' && c != ']') {

                line += c;
            }

            input.putback(c);


            if (line == "null"s) {
                return Node(std::nullptr_t{ nullptr });
            }

            throw ParsingError("");

            return Node(move(line));
        }

        Node LoadBool(istream& input) {
            string line;

            char c;

            while (input >> c && c != ',' && c != '\t' && c != '\r' && c != '\n' && c != ' ' && c != '}' && c != ']') {

                line += c;
            }

            input.putback(c);

            if (line == "true"s) {
                return Node(bool{ true });
            }

            if (line == "false"s) {
                return Node(bool{ false });
            }

            throw ParsingError("");


            return Node(move(line));
        }

        Node LoadDict(istream& input) {
            Dict result;

            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key;
                if (c == '"') {
                    getline(input, key, '"');
                }

                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            //std::cout << c << std::endl;

            if (c != '}') {
                throw ParsingError("");
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            while (c == '\t' || c == '\r' || c == '\n' || c == ' ') {
                input >> c;
            }

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                return LoadInt(input);
            }
        }

    }  // namespace



    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);

        // Реализуйте функцию самостоятельно
    }


    bool operator==(Document left, Document right) {
        return left.GetRoot() == right.GetRoot();
    };

    bool operator!=(Document left, Document right) {
        return !(left == right);
    };

    json::Document LoadJSON(const std::string& s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }

    std::string Print(const Node& node) {
        std::ostringstream out;
        Document doc{ node };
        Print(doc, out);
        return out.str();
    }
}  // namespace json