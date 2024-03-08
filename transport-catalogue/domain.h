#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

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

	struct Domain {
	public:

		struct Stop {
			std::string name;
			geo::Coordinates coords;

			bool operator<(const Stop& other) const{
				return name < other.name;
			}

			bool operator==(const Stop& other) const {
				return name == other.name;
			}
		};

		struct Bus {
			std::string number;
			std::vector<Stop*> stops;
			bool is_roundtrip;
			Stop* final_stop_link;
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
			double length = 0.;
			double curvature = 0.;
		};

		struct StopInfo {
			std::string info;
		};

		std::deque<Bus> AsBus() {
			return buses_;
		}

		std::deque<Stop> AsStop() {
			return stops_;
		}

	protected:


		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop;
		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_bus;
		std::unordered_map<StopPtrPair, int, StopPtrPair_Hesher> distance;
		
	};

}


