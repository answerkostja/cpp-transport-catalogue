#pragma once

#include <iostream>

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace std::literals;


class JSON {
public:

	JSON() = default;
	

	struct CommandDescription {
		// Определяет, задана ли команда (поле command непустое)
		explicit operator bool() const {
			return !type.empty();
		}

		bool operator!() const {
			return !operator bool();
		}


		std::string type;      // Название команды
		int id = 0;           // id маршрута или остановки
		std::string name;
	};

	

	void FillStop(const json::Dict& dict); 

	void FillBus(const json::Dict& dict); 

	void FillCommand(const json::Dict& dict);

	svg::Color FillColor(json::Node node);

	void FillRender(const json::Dict& dict);

	void Fill(std::string line); 

	void DestructJSON();

	void CreateJSON(); 

	void PrintJSON(); 

	renderer::RenderSettings AsSettings();

	transport::TransportCatalogue AsCatalogue();

	

private:
	transport::TransportCatalogue tc_;
	std::vector<CommandDescription> commands_;
	json::Document result_doc_ = json::Document{json::Node(nullptr)};
	renderer::RenderSettings set_;
	

};
