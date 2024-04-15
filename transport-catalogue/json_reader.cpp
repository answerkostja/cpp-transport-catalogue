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

			for (const auto& [key2, value2] : dict.at("road_distances"s).AsDict()) {
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
		if (key == "from"s && value.IsString()) {
			command.from = value.AsString();
		}
		if (key == "to"s && value.IsString()) {
			command.to = value.AsString();
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

void JSON::FillRoute(const json::Dict& dict) {
	routeset::RouteSettings route_set;
	route_set.bus_wait_time = dict.at("bus_wait_time").AsInt();
	route_set.bus_velocity = dict.at("bus_velocity").AsDouble();

	route_set_ = route_set;
}




void JSON::Fill(std::string line) {
	json::Document doc = json::LoadJSON(line);
	json::Node node = doc.GetRoot();
	if (node.IsDict()) {
		for (const auto& [key, value] : node.AsDict()) {
			if (key == "base_requests"s) {
				if (value.IsArray()) {
					for (auto content : value.AsArray()) {
						if (content.IsDict()) {
							if (content.AsDict().at("type"s) == "Stop"s) {
								FillStop(content.AsDict());
							}
						}
					}
					for (auto content : value.AsArray()) {
						if (content.IsDict()) {
							if (content.AsDict().at("type"s) == "Bus"s) {
								FillBus(content.AsDict());
									}
								}
								

							}
							
						}
					}
				
			
			if (key == "stat_requests"s) {
				if (value.IsArray()) {
					for (auto content : value.AsArray()) {
						if (content.IsDict()) {
							//for (const auto& [key2, value2] : content.AsDict()) {
								FillCommand(content.AsDict());
							//}
						}
					}
				}
			}
			if (key == "render_settings"s) {
				if (value.IsDict()) {
					FillRender(value.AsDict());
				}
			}
			if (key == "routing_settings") {
				if (value.IsDict()) {
					FillRoute(value.AsDict());
				}
			}
		}
	}
}
void JSON::DestructJSON() {
	char c;
	std::string line;
	int i = 0;
	while (std::cin.get(c)) {
		line += c;
		i++;
	}
	JSON::Fill(line);
}


void JSON::CreateJSON() {
	renderer::MapRenderer mp_(tc_, set_);
	RequestHandler rh_(tc_, mp_);
	routeset::TransportRouter<double> tr(tc_, route_set_);
	
	graph::Router<double> r(std::move(tr.graph_));

	json::Builder builder;
	builder.StartArray();

	int iter = 0;

	for (CommandDescription command : commands_) {

		iter++;
		int id = command.id;

		if (command.type == "Bus"s) {
			std::optional<RequestHandler::BusInfo> result = rh_.GetBusStat(command.name);
			
			int stops = result.value().stops;
			int unique_stops = result.value().unique_stops;
			double length = result.value().length;
			double curvature = result.value().curvature;

			if (stops == 0 && unique_stops == 0 && length == 0 && curvature == 0) {
				builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(id).EndDict();
				
				continue;

			}
			builder.StartDict().Key("curvature"s).Value(curvature).Key("request_id"s).Value(id).Key("route_length"s).Value(length).Key("stop_count"s).Value(stops)
				.Key("unique_stop_count"s).Value(unique_stops).EndDict();
			

		}
		else if (command.type == "Stop"s) {
			
			std::set<std::string> result = rh_.GetBusesByStop(command.name);
			
			if (result == std::set{ std::string{ "No stops"s } }) {
				builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(id).EndDict();
					
					continue;
			}
			else {
				builder.StartDict().Key("buses"s).StartArray();
				json::Array array;
				for (auto res : result) {
					builder.Value(res);
					
				}
				builder.EndArray();
				builder.Key("request_id"s).Value(id).EndDict();
				
				continue;
			}

			
		}
		else if (command.type == "Map"s) {
			
			std::string result = rh_.RenderMap();
			builder.StartDict().Key("map"s).Value(result).Key("request_id"s).Value(id).EndDict();
			
			continue;
		}
		else if (command.type == "Route") {
			
			int id_vertex_begin = tr.GetVertexPairStop(tc_.FindStop(command.from)).begin;
			int id_vertex_end = tr.GetVertexPairStop(tc_.FindStop(command.to)).begin;
			if (id_vertex_begin == id_vertex_end) {
				builder.StartDict()
					.Key("items"s)
					.StartArray()
					.EndArray()
					.Key("request_id"s)
					.Value(id)
					.Key("total_time")
					.Value(0)
					.EndDict();
				continue;
			}
			std::optional<typename graph::Router<double>::RouteInfo> route_info = (tr.BuildOptimalRoute(r, id_vertex_begin, id_vertex_end));
			if (route_info == std::nullopt) {
				builder.StartDict()
					.Key("request_id"s)
					.Value(id)
					.Key("error_message"s)
					.Value("not found"s)
					.EndDict();

				continue;
			}
			

			
			/*
			std::cout << route_info.value().weight << std::endl;
			for (int i = 0; i < route_info.value().edges.size(); i++) {
				std::cout << tr.GetEdgesInfoMap().at(route_info.value().edges[i]).bus<<std::endl;
			}*/

			
			builder.StartDict()
				.Key("items"s)
				.StartArray();

			for (int i = 0; i < route_info.value().edges.size(); i++) {
				size_t number_edge = route_info.value().edges[i];
				if (tr.GetEdgesInfo(number_edge).expect) {
						builder.StartDict()
							.Key("type"s)
							.Value("Wait"s)
							.Key("stop_name")
							.Value(tr.GetStop(tr.GetEdgesInfo(number_edge).to)->name)
							.Key("time"s)
							.Value(tr.GetEdgesInfo(number_edge).weight)
							.EndDict();
				}
				else {
					routeset::EdgeInfo edges_info = tr.GetEdgesInfo(number_edge);
					builder.StartDict()
						.Key("type"s)
						.Value("Bus"s)
						.Key("bus"s)
						.Value(edges_info.bus)
						.Key("span_count"s)
						.Value(edges_info.span_count)
						.Key("time"s)
						.Value(edges_info.weight)
						.EndDict();
				}
			}
			builder.EndArray()
				.Key("request_id"s)
				.Value(id)
				.Key("total_time"s)
				.Value(route_info.value().weight)
				.EndDict();

			
		}

	}
	builder.EndArray();
	result_doc_ = json::Document{ builder.Build()};
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

routeset::RouteSettings JSON::AsRouteSettings() {
	return route_set_;
}