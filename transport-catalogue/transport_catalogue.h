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

		struct BusInfo {
			int stops;
			int uniq_stops;
			double dist;
		};

		struct StopInfo {
			std::string info;
		};

		
		
			void AddStop(const std::string& name, coord::Coordinates coords);
			void AddBus(const std::string& number, const std::vector<std::string>& routes);
			Stop* FindStop(std::string_view name);
			Bus* FindBus(std::string_view number) const;
			BusInfo GetBusInfo(std::string_view request) const;
			StopInfo GetStopInfo(std::string_view request) const;

		
	private:
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop;
		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_bus;
		


	};
}
