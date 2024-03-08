#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

RequestHandler::RequestHandler(const  transport::TransportCatalogue& db, const renderer::MapRenderer& renderer)
	:db_(db),
	renderer_(renderer) {};

 // Возвращает информацию о маршруте (запрос Bus)
std::optional<RequestHandler::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
	return std::optional<RequestHandler::BusInfo>{RequestHandler::BusInfo{ db_.GetBusInfo(bus_name).stops, db_.GetBusInfo(bus_name).unique_stops, db_.GetBusInfo(bus_name).length, db_.GetBusInfo(bus_name).curvature }};
};

// Возвращает маршруты, проходящие через
std::set<std::string> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
	/*const std::unordered_set<std::string> con_db = db_.GetStopInfo(stop_name);
	const std::unordered_set<std::string> link = &con_db;*/
	return db_.GetStopInfo(stop_name);;

};

std::string RequestHandler::RenderMap(){
	
	return renderer_.GetMap();

};