#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

void JSON::FillStop(const json::Dict& dict) {
	std::string name;
	geo::Coordinates coords;
	std::unordered_map<std::string, int> map_dist;

	
	
		
			name = dict.at("name"s).AsString();
		
			coords.lat = dict.at("latitude"s).AsDouble();
		
			coords.lng = dict.at("longitude"s).AsDouble();

			for (const auto& [key2, value2] : dict.at("road_distances"s).AsMap()) {
				map_dist.insert({ key2, value2.AsInt() });
			}
		
		
	
	tc_.AddStop(std::move(name), std::move(coords), std::move(map_dist));

}

void JSON::FillBus(const json::Dict& dict) {

	std::string number;
	std::vector<std::string> routes;
	std::string final_stop;
	int index = 0;

	
		number = dict.at("name"s).AsString();


		for (auto i : dict.at("stops"s).AsArray()) {
			routes.push_back(i.AsString());
		}
		
		if (!dict.at("is_roundtrip"s).AsBool()) {
			std::vector<std::string> reverse;
			index = routes.size() - 1;
			for (int i = routes.size() - 2; i >= 0; i--) {
				reverse.push_back(routes[i]);
			}
			for (size_t i = 0; i < reverse.size(); i++) {
				routes.push_back(reverse[i]);
			}
				//routes.push_back(routes[0]);
		}
		
		final_stop = routes[index];
	
	tc_.AddBus(number, routes, dict.at("is_roundtrip"s).AsBool(), final_stop);

}

void JSON::FillCommand(const json::Dict& dict) {

	CommandDescription command;

	for (const auto& [key, value] : dict) {
		if (key == "id"s && value.IsInt()) {
			command.id = value.AsInt();
		}

		if (key == "type"s && value.IsString()) {
			command.type = value.AsString();
		}
		if (key == "name"s && value.IsString()) {
			command.name = value.AsString();
		}
	}

	commands_.push_back(command);


}

svg::Color JSON::FillColor(json::Node node){
	if (node.IsArray()) {
		if (node.AsArray().size() == 4) {
			svg::Rgba color;
			int shade = 0;
			for (auto ar : node.AsArray()) {
				switch (shade)
				{
				case 0:
					color.red = ar.AsInt();
					break;
				case 1:
					color.green = ar.AsInt();
					break;
				case 2:
					color.blue = ar.AsInt();
					break;
				case 3:
					color.opacity = ar.AsDouble();
					break;

				}
				shade++;
			}
			return color;
		}
		if (node.AsArray().size() == 3) {
			svg::Rgb color;
			int shade = 0;
			for (auto ar : node.AsArray()) {
				switch (shade)
				{
				case 0:
					color.red = ar.AsInt();
					break;
				case 1:
					color.green = ar.AsInt();
					break;
				case 2:
					color.blue = ar.AsInt();
					break;
				}
				shade++;
			
			}
			return color;
		}


	}
	
	return node.AsString();
	
}

void JSON::FillRender(const json::Dict& dict) {

	renderer::RenderSettings set;

	set.width = dict.at("width"s).AsDouble();
	set.height = dict.at("height"s).AsDouble();

	set.padding = dict.at("padding"s).AsDouble();

	set.line_width = dict.at("line_width"s).AsDouble();
	set.stop_radius = dict.at("stop_radius"s).AsDouble();

	set.bus_label_font_size = dict.at("bus_label_font_size"s).AsDouble();
	for (auto ar : dict.at("bus_label_offset"s).AsArray()) {
		set.bus_label_offset.push_back(ar.AsDouble());
	}

	set.stop_label_font_size = dict.at("stop_label_font_size"s).AsDouble();
	for (auto ar : dict.at("stop_label_offset"s).AsArray()) {
		set.stop_label_offset.push_back(ar.AsDouble());
	}

	set.underlayer_color = FillColor(dict.at("underlayer_color"s));
	set.underlayer_width = dict.at("underlayer_width"s).AsDouble();
	
	
	for (auto ar : dict.at("color_palette"s).AsArray()) {
		set.color_palette.push_back(FillColor(ar));
	}

	set_ = set;

}




void JSON::Fill(std::string line) {
	json::Document doc = json::LoadJSON(line);
	json::Node node = doc.GetRoot();
	if (node.IsMap()) {
		for (const auto& [key, value] : node.AsMap()) {
			if (key == "base_requests"s) {
				if (value.IsArray()) {
					for (auto content : value.AsArray()) {
						if (content.IsMap()) {
							if (content.AsMap().at("type"s) == "Stop"s) {
								FillStop(content.AsMap());
							}
						}
					}
					for (auto content : value.AsArray()) {
						if (content.IsMap()) {
							if (content.AsMap().at("type"s) == "Bus"s) {
								FillBus(content.AsMap());
									}
								}
								

							}
							
						}
					}
				
			
			if (key == "stat_requests"s) {
				if (value.IsArray()) {
					for (auto content : value.AsArray()) {
						if (content.IsMap()) {
							//for (const auto& [key2, value2] : content.AsMap()) {
								FillCommand(content.AsMap());
							//}
						}
					}
				}
			}
			if (key == "render_settings"s) {
				if (value.IsMap()) {
					FillRender(value.AsMap());
				}
			}
		}
	}
}
void JSON::DestructJSON() {
	char c;
	std::string line;
	while (std::cin.get(c)) {
		line += c;
	}
	JSON::Fill(line);
}


void JSON::CreateJSON() {
	renderer::MapRenderer mp_(tc_, set_);
	RequestHandler rh_(tc_, mp_);

	json::Array result_array;

	for (CommandDescription command : commands_) {
		if (command.type == "Bus"s) {
			std::optional<RequestHandler::BusInfo> result = rh_.GetBusStat(command.name);
			int id = command.id;
			int stops = result.value().stops;
			int unique_stops = result.value().unique_stops;
			double length = result.value().length;
			double curvature = result.value().curvature;

			if (stops == 0 && unique_stops == 0 && length == 0 && curvature == 0) {
				json::Node node(json::Dict{ std::pair<std::string, json::Node>{"error_message"s, json::Node{ "not found"s}},
						std::pair<std::string, json::Node>{"request_id"s, json::Node{ id }}
					});
				result_array.push_back(node);
				continue;

			}
			json::Node node(json::Dict{ std::pair<std::string, json::Node>{ "curvature"s , json::Node{curvature}},
				std::pair<std::string, json::Node>{ "request_id"s , json::Node{id}},
				std::pair<std::string, json::Node>{ "route_length"s , json::Node{length}},
				std::pair<std::string, json::Node>{ "stop_count"s , json::Node{stops}},
				std::pair<std::string, json::Node>{ "unique_stop_count"s , json::Node{unique_stops}} });
			result_array.push_back(node);

		}
		else if (command.type == "Stop"s) {
			int id = command.id;
			std::set<std::string> result = rh_.GetBusesByStop(command.name);
			
			if (result == std::set{ std::string{ "No stops"s } }) {
					json::Node node(json::Dict{ std::pair<std::string, json::Node>{"error_message"s, json::Node{ "not found"s}},
						std::pair<std::string, json::Node>{"request_id"s, json::Node{ id }}
						 });
					result_array.push_back(node);
					continue;
			}
			else {
				json::Array array;
				for (auto res : result) {
					array.push_back(json::Node(res));
				}
				json::Node node_array(array);
				json::Dict dict{ {"buses"s, node_array}, {"request_id"s, json::Node{ id }} };
				json::Node node(dict);
				result_array.push_back(node);
				continue;
			}

			
		}
		else if (command.type == "Map"s) {
			int id = command.id;
			std::string result = rh_.RenderMap();
			json::Node node_array(result);
			json::Dict dict{ {"map"s, node_array}, {"request_id"s, json::Node{ id }} };
			json::Node node(dict);
			result_array.push_back(node);
			continue;
		}

	}
	result_doc_ = json::Document{ json::Node{result_array} };
}

void JSON::PrintJSON() {
	json::Print(result_doc_, std::cout);
}

renderer::RenderSettings JSON::AsSettings() {
	return set_;

}

transport::TransportCatalogue JSON::AsCatalogue() {
	return tc_;
}