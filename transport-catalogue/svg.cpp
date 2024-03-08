#include "svg.h"

namespace svg {

    using namespace std::literals;

    Rgb::Rgb()
        :red(0),
        green(0),
        blue(0) {
    }
    Rgb::Rgb(uint8_t red, uint8_t green, uint8_t blue)
        :red(red),
        green(green),
        blue(blue) {
    }

    uint8_t Rgb::GetRed() {
        return red;
    }

    uint8_t Rgb::GetGreen() {
        return green;
    }

    uint8_t Rgb::GetBlue() {
        return blue;
    }



    Rgba::Rgba()
        :red(0),
        green(0),
        blue(0),
        opacity(1.0) {
    }
    Rgba::Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
        :red(red),
        green(green),
        blue(blue),
        opacity(opacity) {
    }

    uint8_t Rgba::GetRed() {
        return red;
    }

    uint8_t Rgba::GetGreen() {
        return green;
    }

    uint8_t Rgba::GetBlue() {
        return blue;
    }

    double Rgba::GetOpacity() {
        return opacity;
    }

    std::ostream& operator<<(std::ostream& output, Color value) {
        std::visit(ColorPrinter{ output }, value);
        return output;
    };

    std::ostream& operator<<(std::ostream& output, StrokeLineCap value) {


        using namespace std::literals;
        switch (value) {
        case StrokeLineCap::BUTT:
            output << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            output << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            output << "square"sv;
            break;
        }

        // Оператор должен вернуть ссылку на переданный поток вывода
        return output;
    }



    std::ostream& operator<<(std::ostream& output, StrokeLineJoin value) {

        using namespace std::literals;

        switch (value) {
        case StrokeLineJoin::ARCS:
            output << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL:
            output << "bevel"sv;
            break;
        case StrokeLineJoin::MITER:
            output << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP:
            output << "miter-clip"sv;
            break;
        case StrokeLineJoin::ROUND:
            output << "round"sv;
            break;
        }

        // Оператор должен вернуть ссылку на переданный поток вывода
        return output;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------



    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline::Polyline(Point point) {
        points_.push_back(point);
    }

    // Добавляет очередную вершину к ломаной линии
    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        if (points_.size() == 0) {
            out << "\" />"sv;
            return;
        }

        for (size_t i = 0; i < points_.size(); i++) {
            if (i != points_.size() - 1) {
                out << points_.at(i).x << ","sv << points_.at(i).y << " "sv;
            }
            else {
                out << points_.at(i).x << ","sv << points_.at(i).y << "\"";

                RenderAttrs(context.out);
                out << "/>"sv;
            }
        }
    }


    // ---------- Text ------------------

    Text::Text(Point pos, Point offset, uint32_t size)
        :pos_(pos),
        offset_(offset),
        size_(size) {
    }

    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    };

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    };

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    };

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    };

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    };

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    };

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(context.out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" font-size=\""sv << size_ << "\""sv;

        if (font_family_.empty() && font_weight_.empty() && data_.empty()) {
            out << "></text>"sv;
            return;
        }
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        if (!data_.empty()) {
            std::string data = data_;
            size_t f = 0;
            while (data.find("&"s, f) != data.npos) {
                size_t f_symbol = data.find("&"s, f);
                data.erase(f_symbol, 1);
                data.insert(f_symbol, "&amp;");
                f = f_symbol + 1;
            }
            f = 0;
            while (data.find("\""s, f) != data.npos) {
                size_t f_symbol = data.find("\""s, f);
                data.erase(f_symbol, 1);
                data.insert(f_symbol, "&quot;");
            }
            while (data.find("<"s, f) != data.npos) {
                size_t f_symbol = data.find("<"s, f);
                data.erase(f_symbol, 1);
                data.insert(f_symbol, "&lt;");
            }
            while (data.find(">"s, f) != data.npos) {
                size_t f_symbol = data.find(">"s, f);
                data.erase(f_symbol, 1);
                data.insert(f_symbol, "&gt;");
            }
            while (data.find("\'"s, f) != data.npos) {
                size_t f_symbol = data.find("\'"s, f);
                data.erase(f_symbol, 1);
                data.insert(f_symbol, "&apos;");
            }


            out << ">"sv << data << "</text>";
        }
        else {
            out << "></text>"sv;
        }
    }

    // Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    };

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        RenderContext ctx{ out, 2, 2 };
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (auto& obj : objects_) {

            obj->Render(ctx);
        }
        out << "</svg>"sv;
    };

}  // namespace svg