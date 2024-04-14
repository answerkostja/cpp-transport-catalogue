#pragma once

/*
 * Здесь можно разместить код транспортного справочника
 */



#include "domain.h"



namespace transport {

	using namespace std::literals;

	class TransportCatalogue : public Domain {
		// Реализуйте класс самостоятельно
	public:

		

		void AddStop(const std::string& name, geo::Coordinates coords, std::unordered_map<std::string, int> map_dist);
		void AddBus(const std::string& number, const std::vector<std::string>& routes,  bool is_roundtrip, const std::string& final_stop);
		Stop* FindStop(std::string_view name);
		Bus* FindBus(std::string_view number) const;
		void FillDistance(Stop* stop, const std::unordered_map<std::string, int>& map_dist);
		BusInfo GetBusInfo(std::string_view request) const;
		std::set<std::string> GetStopInfo(std::string_view request) const;

		std::deque<Bus> AsBus(); 

		std::deque<Stop> AsStop(); 

		int AsDistance(Stop* begin, Stop* end);

		Stop* AsNameToStop(std::string_view query); 

	private:
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Stop*> stop_name_to_stop;
		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_bus;
		std::unordered_map<StopPtrPair, int, StopPtrPairHasher> distance;
		
		


	};
}
