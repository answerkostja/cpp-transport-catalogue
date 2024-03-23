#include "transport_catalogue.h"

/*
 * Здесь можно разместить код транспортного справочника
 */

#include "transport_catalogue.h"

#include <iostream>
#include <utility>

namespace transport {

	void TransportCatalogue::FillDistance(Stop* stop, const std::unordered_map<std::string, int>& map_dist) {

		for (auto [i, j] : map_dist) {
			if (stop_name_to_stop.count(i) != 0) {
				StopPtrPair p = { stop, stop_name_to_stop.at(i) };
				int dist = j;
				if (distance.count(p) > 0) {
					distance.erase(p);
				}
				distance.insert({ std::move(p), dist });
				StopPtrPair p_reverse = { stop_name_to_stop.at(i), stop };
				if (distance.count(p_reverse) == 0) {
					distance.insert({ std::move(p_reverse), dist });
				}

			}
			else if (stop_name_to_stop.count(i) == 0) {
				TransportCatalogue::Stop add_stop = Stop{ i, {} };

				stops_.push_back(std::move(add_stop));
				std::string_view name_sv = stops_[stops_.size() - 1].name;

				stop_name_to_stop.insert({ name_sv, &stops_[stops_.size() - 1] });

				StopPtrPair p = { stop, stop_name_to_stop.at(i) };
				int dist = j;
				if (distance.count(p) > 0) {
					distance.erase(p);
				}
				distance.insert({ std::move(p), dist });
				StopPtrPair p_reverse = { stop_name_to_stop.at(i), stop };
				if (distance.count(p_reverse) == 0) {
					distance.insert({ std::move(p_reverse), dist });
				}

			}
		}


	}

	void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coords, std::unordered_map<std::string, int> map_dist) {

		if (stop_name_to_stop.count(name)) {
			stop_name_to_stop.at(name)->coords = coords;
		}
		else {
			TransportCatalogue::Stop add_stop = Stop{ name, coords };



			stops_.push_back(std::move(add_stop));
			std::string_view name_sv = stops_[stops_.size() - 1].name;

			stop_name_to_stop.insert({ name_sv, &stops_[stops_.size() - 1] });
		}

		TransportCatalogue::FillDistance(stop_name_to_stop.at(name), map_dist);

	}

	void TransportCatalogue::AddBus(const std::string& number, const std::vector<std::string>& routes, bool is_roundtrip, const std::string& final_stop) {
		std::vector<Stop*> routes_link;
		TransportCatalogue::Stop* final_stop_link = stop_name_to_stop.at(static_cast<std::string_view>(final_stop));
		for (size_t index = 0; index < routes.size(); index++) {
			TransportCatalogue::Stop* link = stop_name_to_stop.at(static_cast<std::string_view>(routes.at(index)));
			routes_link.push_back(std::move(link));
		}
		
		TransportCatalogue::Bus add_bus = { number, std::move(routes_link),  is_roundtrip, final_stop_link };
		buses_.push_back(std::move(add_bus));
		std::string_view number_sv = buses_[buses_.size() - 1].number;
		busname_to_bus.insert({ std::move(number_sv),&buses_[buses_.size() - 1] });
		
		
	}


	TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view name) {

		return (*(stop_name_to_stop.find(name))).second;
	}


	TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view number) const {
		if (busname_to_bus.find(number) == busname_to_bus.end()) {
			return nullptr;
		}

		return (*(busname_to_bus.find(number))).second;
	};



	TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(std::string_view request) const {
		int stops = 0;
		int unique_stops = 0;
		double length = 0.;
		double curvature = 0.;

		Bus* bus = TransportCatalogue::FindBus(request);

		if (bus == nullptr) {
			return { 0, 0, 0, 0 };
		}

		stops = static_cast<int>((*bus).stops.size());

		std::unordered_set<Stop*> set_unique_stops;
		for (auto s : (*bus).stops) {
			set_unique_stops.insert(s);
		}
		unique_stops = static_cast<int>(set_unique_stops.size());

		double dist = 0.;

		for (size_t i = 0; i < (*(bus)).stops.size() - 1; i++) {

			dist += ComputeDistance((*bus).stops[i]->coords, (*bus).stops[i + 1]->coords);
			StopPtrPair p = { (*bus).stops[i], (*bus).stops[i + 1] };
			length += distance.at(p);

		}

		curvature = length / dist;

		return { stops, unique_stops, length,  curvature };

	}

	std::set<std::string> TransportCatalogue::GetStopInfo(std::string_view request) const {
		std::set<std::string> result;
		if (stop_name_to_stop.count(request) == 0) {
			return  std::set<std::string>{"No stops"s};
		}


		std::set<transport::Domain::Bus*> bus;
		for (const auto& [name, link] : busname_to_bus) {
			for (auto i : (*link).stops) {

				if ((*i).name == request) {
					bus.insert(link);
				}

			}
		}
		
		for (auto i : bus) {
			result.insert(static_cast<std::string>(i->number));
		}
		
		return  result;
	};

	std::deque<transport::Domain::Bus> TransportCatalogue::AsBus() {
		return buses_;
	}

	std::deque<transport::Domain::Stop> TransportCatalogue::AsStop() {
		return stops_;
	}

}