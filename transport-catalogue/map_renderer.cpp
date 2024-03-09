#include "map_renderer.h"
namespace renderer {
    /*
     * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
     * Визуализация маршртутов вам понадобится во второй части итогового проекта.
     * Пока можете оставить файл пустым.
     */

    svg::Point renderer::SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    void renderer::MapRenderer::RenderLine(svg::Document& doc, size_t& number_color, 
        std::deque<transport::Domain::Bus>& buses, SphereProjector& sph_proj_) {

        

        for (auto bus : buses) {
            svg::Polyline polyline;
            for (auto stop : bus.stops) {
                polyline.AddPoint(sph_proj_.operator()(stop->coords));
            }
            polyline.SetStrokeColor(settings_.color_palette[number_color % settings_.color_palette.size()]);
            polyline.SetFillColor(svg::Color());
            polyline.SetStrokeWidth(settings_.line_width);
            polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            doc.AddPtr(std::make_unique<svg::Polyline>(polyline));
            if (!bus.stops.size() == 0) {
                number_color++;
            }
        }

    }

    void renderer::MapRenderer::RenderBus(svg::Document& doc, size_t& number_color, std::deque<transport::Domain::Bus>& buses, SphereProjector& sph_proj_) {

        using namespace std::literals;

        for (auto bus : buses) {

            if (!bus.stops.empty()) {

                svg::Text sub_text2;
                auto begin_stop = bus.stops[0];



                sub_text2.SetPosition(sph_proj_.operator()(begin_stop->coords));
                sub_text2.SetOffset(svg::Point{ settings_.bus_label_offset[0], settings_.bus_label_offset[1] });
                sub_text2.SetFontSize(settings_.bus_label_font_size);
                sub_text2.SetFontFamily("Verdana"s);
                sub_text2.SetFontWeight("bold"s);
                sub_text2.SetData(bus.number);
                sub_text2.SetFillColor(settings_.underlayer_color);
                sub_text2.SetStrokeColor(settings_.underlayer_color);
                sub_text2.SetStrokeWidth(settings_.underlayer_width);
                sub_text2.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                sub_text2.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                doc.AddPtr(std::make_unique<svg::Text>(sub_text2));

                svg::Text text2;

                text2.SetPosition(sph_proj_.operator()(begin_stop->coords));
                text2.SetOffset(svg::Point{ settings_.bus_label_offset[0], settings_.bus_label_offset[1] });
                text2.SetFontSize(settings_.bus_label_font_size);
                text2.SetFontFamily("Verdana"s);
                text2.SetFontWeight("bold"s);
                text2.SetData(bus.number);
                text2.SetFillColor(settings_.color_palette[number_color % settings_.color_palette.size()]);

                doc.AddPtr(std::make_unique<svg::Text>(text2));

                if (!bus.is_roundtrip) {
                    transport::Domain::Stop* end_stop = bus.final_stop_link;

                    if (end_stop != begin_stop) {
                        sub_text2.SetPosition(sph_proj_.operator()(end_stop->coords));
                        doc.AddPtr(std::make_unique<svg::Text>(sub_text2));
                        text2.SetPosition(sph_proj_.operator()(end_stop->coords));
                        doc.AddPtr(std::make_unique<svg::Text>(text2));
                    }

                }


                if (bus.stops.size() != 0) {
                    number_color++;
                }

            }
        }


    }

    void renderer::MapRenderer::RenderStop(svg::Document& doc, std::deque<transport::Domain::Bus>& buses, SphereProjector& sph_proj_) {
        
        using namespace std::literals;
        
        std::set<transport::Domain::Stop> stop_set;

        for (auto bus : buses) {
            for (auto stop : bus.stops) {
                stop_set.insert(*stop);
            }
        }
        for (auto stop : stop_set) {
            svg::Circle circle;
            circle.SetCenter(sph_proj_.operator()(stop.coords));
            circle.SetRadius(settings_.stop_radius);
            circle.SetFillColor(svg::Color("white"s));
            doc.AddPtr(std::make_unique<svg::Circle>(circle));
        }

        for (auto stop : stop_set) {
            svg::Text sub_text;
            sub_text.SetPosition(sph_proj_(stop.coords));
            sub_text.SetOffset(svg::Point{ settings_.stop_label_offset[0], settings_.stop_label_offset[1] });
            sub_text.SetFontSize(settings_.stop_label_font_size);
            sub_text.SetFontFamily("Verdana"s);

            sub_text.SetData(stop.name);
            sub_text.SetFillColor(settings_.underlayer_color);
            sub_text.SetStrokeColor(settings_.underlayer_color);

            sub_text.SetStrokeWidth(settings_.underlayer_width);

            sub_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            sub_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            doc.AddPtr(std::make_unique<svg::Text>(sub_text));

            svg::Text text;

            text.SetPosition(sph_proj_(stop.coords));
            text.SetOffset(svg::Point{ settings_.stop_label_offset[0], settings_.stop_label_offset[1] });
            text.SetFontSize(settings_.stop_label_font_size);
            text.SetFontFamily("Verdana"s);

            text.SetData(stop.name);
            text.SetFillColor(svg::Color{ "black"s });

            doc.AddPtr(std::make_unique<svg::Text>(text));

        }
    }


    void renderer::MapRenderer::Render() {
        
        svg::Document doc;
        std::vector<geo::Coordinates> geo_coords;
        size_t number_color = 0;

        std::deque<transport::Domain::Bus> buses = tc_.AsBus();

        std::sort(buses.begin(), buses.end(), [](const transport::Domain::Bus left, const transport::Domain::Bus right) {
            return (left.number < right.number);
            });


        for (auto bus : buses) {
            for (auto stop : bus.stops) {
                geo_coords.push_back(stop->coords);
            }
        }
        SphereProjector sph_proj_(geo_coords.begin(), geo_coords.end(), settings_.width, settings_.height, settings_.padding);

        renderer::MapRenderer::RenderLine(doc,  number_color,  buses, sph_proj_);

        
        number_color = 0;

        renderer::MapRenderer::RenderBus(doc, number_color, buses, sph_proj_);

        renderer::MapRenderer::RenderStop(doc, buses, sph_proj_);

        

        std::ostringstream sout;

        doc.Render(sout);

        map_ = sout.str();

    }

    std::string renderer::MapRenderer::GetMap() {
        this->Render();
        return map_;
    }
}