#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

using namespace std::literals;

namespace transport {
    namespace detail {
        coord::Coordinates ParseCoordinates(std::string_view str);
        void ParseDistance(std::string_view str, std::unordered_map<std::string, int>& map_dist);
        std::string_view Trim(std::string_view string);
        std::vector<std::string_view> Split(std::string_view string, char delim);
        std::vector<std::string_view> ParseRoute(std::string_view route);

        struct CommandDescription {
            // Определяет, задана ли команда (поле command непустое)
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }


            std::string command;      // Название команды
            std::string id;           // id маршрута или остановки
            std::string description;
            std::string distantion;// Параметры команды
        };

    }
    class InputReader {
    public:
        /**
         * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
         */
        void ParseLine(std::string_view line);

        /**
         * Наполняет данными транспортный справочник, используя команды из commands_
         */
        void ApplyCommands(TransportCatalogue& catalogue);

    private:
        std::vector<detail::CommandDescription> commands_;
    };
}
