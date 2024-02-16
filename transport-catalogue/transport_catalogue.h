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

		class StopPtrPair_Hesher {
		public:
			size_t operator() (const StopPtrPair& ptr) const {
				size_t s_start = s_hasher_((*ptr.start).name);
				size_t s_end = s_hasher_((*ptr.end).name);
				return s_start + s_end * 1000;

			}
	
		private:
			std::hash<std::string> s_hasher_;
		};

		struct BusInfo {
			int stops = 0;
			int unique_stops = 0;
			int length = 0.;
			double curvature = 0.;
		};

		struct StopInfo {
			std::string info;
		};
		
			void AddStop(const std::string& name, coord::Coordinates coords, std::unordered_map<std::string, int> map_dist);
			void AddBus(const std::string& number, const std::vector<std::string>& routes);
			Stop* FindStop(std::string_view name);
			Bus* FindBus(std::string_view number) const;
			void FillDistance(Stop* stop, const std::unordered_map<std::string, int>& map_dist);
			BusInfo GetBusInfo(std::string_view request) const;
			StopInfo GetStopInfo(std::string_view request) const;

		
	private:
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop;
		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_bus;
		std::unordered_map<StopPtrPair, int, StopPtrPair_Hesher> distance;


	};
}
