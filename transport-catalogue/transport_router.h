#pragma once

#include "router.h"
#include "transport_catalogue.h"

namespace routeset{
	struct RouteSettings {
		int bus_wait_time;
		double bus_velocity;
	};

	struct EdgeInfo {
		std::string bus;
		bool expect;
		size_t from;
		size_t to;
		double weight;
		int span_count;
	};

	struct Number {
		size_t begin;
		size_t end;
	};

	template <typename Weight>
	class TransportRouter {
	public:
		using Graph = graph::DirectedWeightedGraph<Weight>;

		TransportRouter() = default;

		TransportRouter(transport::TransportCatalogue tc, routeset::RouteSettings rs) 
			:graph_(tc.AsStop().size() * 2){
			FillEdges(tc, rs);
		}
			
		Number GetVertexPairStop(transport::Domain::Stop* link) {
			return pair_stops_.at(link);
		}

		transport::Domain::Stop* GetStop(size_t number) {
			for (auto [link, numbers] : pair_stops_) {
				if (numbers.begin == number || numbers.end == number) {
					return link;
				}
			}
			transport::Domain::Stop stop;
			transport::Domain::Stop* link = &stop;
			return link;
		}


		EdgeInfo GetEdgesInfo(size_t number) {
			return edges_info_.at(number);
		}

		std::optional<typename graph::Router<double>::RouteInfo> BuildOptimalRoute(graph::Router<double> router_, int from, int to) {
			return router_.BuildRoute(from, to);
		}
	
		Graph graph_;

	private:

		void FillEdgesStop(transport::TransportCatalogue tc, routeset::RouteSettings rs) {
			size_t number = 0;
			for (auto stop : tc.AsStop()) {
				graph::Edge<double> edge{ number, number + 1, static_cast<double>(rs.bus_wait_time) };
				size_t id = graph_.AddEdge(edge);
				edges_info_.insert({ id, EdgeInfo{"0", true, number, number + 1, static_cast<double>(rs.bus_wait_time), 0 } });
				pair_stops_.insert({ tc.AsNameToStop(stop.name), Number{number, number + 1} });
				number += 2;
			}
		}

		void FillEdgesBus(transport::TransportCatalogue tc, routeset::RouteSettings rs) {
			for (auto bus : tc.AsBus()) {
				for (size_t i = 0; i < bus.stops.size() - 1; i++) {
					double weight = 0;
					for (size_t j = i + 1; j < bus.stops.size(); j++) {
						weight += static_cast<double>(tc.AsDistance(bus.stops[j - 1], bus.stops[j])) / 1000 / rs.bus_velocity * 60;
						graph::Edge<double> edge{ pair_stops_.at(bus.stops[i]).end, pair_stops_.at(bus.stops[j]).begin, weight };
						size_t id = graph_.AddEdge(edge);
						edges_info_.insert({ id, EdgeInfo{ bus.number, false, edge.from, edge.to, weight, static_cast<int>(j - i) } });
					}
				}
			}
		}
		void FillEdges(transport::TransportCatalogue tc, routeset::RouteSettings rs) {
			FillEdgesStop(tc, rs);
			FillEdgesBus(tc, rs);
		}


		
		std::unordered_map<transport::Domain::Stop*, Number> pair_stops_;
		std::unordered_map<size_t, EdgeInfo> edges_info_;
		
	};
}
