#pragma once
/*
 * Место для вашей JSON-библиотеки
 */



#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <cmath>
#include <sstream>


namespace json {

    

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        /* Реализуйте Node, используя std::variant */

        using Array = std::vector<Node>;

        using Dict = std::map<std::string, Node>;

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node();

        Node(Value value);

        Node(std::nullptr_t n);

        Node(Array a);

        Node(Dict m);

        Node(bool b);

        Node(int i);

        Node(double d);

        Node(std::string s);


        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsDict() const;



        int AsInt() const;

        bool AsBool() const;

        double AsDouble() const;

        const std::string& AsString() const;

        const Array& AsArray() const;

        const Dict& AsDict() const;



        // Перегрузка функции PrintValue для вывода значений null



        const Value& GetValue() const { return value_; }

        Value& GetValue() { return value_; }


    private:
        Value value_;
    };

    template <typename Value>
    void PrintValue(const Value& value, std::ostream& out) {
        out << value;
    }


    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& ctx) {
        ctx.PrintIndent();
        PrintValue(value, ctx.out);
    }

    bool operator==(Node left, Node right);

    bool operator!=(Node left, Node right);





    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    bool operator==(Document left, Document right);

    bool operator!=(Document left, Document right);

    json::Document LoadJSON(const std::string& s); 

    std::string Print(const Node& node); 

}  // namespace json

