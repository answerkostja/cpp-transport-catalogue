#pragma once

#include <deque>
#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_set>
#include <string_view>
#include <set>
#include <algorithm>
#include <optional>
#include <functional>
#include <utility>

#include "geo.h"

namespace transport {

	using namespace std::literals;

	class TransportCatalogue {
		// Реализуйте класс самостоятельно
	public:

		struct Stop {
			std::string name;
			coord::Coordinates coords;
		};

		struct Bus {
			std::string number;
			std::vector<Stop*> stops;
		};

		struct StopPtrPair {
			Stop* start;
			Stop* end;

			bool operator==(const StopPtrPair& other) const {
				return start == other.start && end == other.end;
			}
		};

		
		
			void AddStop(const std::string& name, coord::Coordinates coords);
			void AddBus(const std::string& number, const std::vector<std::string>& routes);
			Stop* FindStop(const std::string_view& name);
			Bus* FindBus(const std::string_view& number) const;
			//void FillingDistance(Stop* stop);
			std::tuple<int, int, double > GetBusInfo(std::string_view request) const;
			std::string GetStopInfo(std::string_view request) const;

		
	private:
		std::deque<Stop> Stops;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop;
		std::deque<Bus> Buses;
		std::unordered_map<std::string_view, Bus*> busname_to_bus;
		


	};
}
