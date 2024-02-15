#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace transport {

    /**
     * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
     */
    namespace detail {
        coord::Coordinates ParseCoordinates(std::string_view str) {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comma = str.find(',');
            auto comma2 = str.find(',', comma + 1);

            if (comma == str.npos) {
                return { nan, nan };
            }

            auto not_space2 = str.find_first_not_of(' ', comma + 1);

            double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
            double lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space2)));

            return { lat, lng };
        }
        /**
     * Парсит строку вида "7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam" и возвращает словарь
     */
         void ParseDistance(std::string_view str, std::unordered_map<std::string, int>& map_dist) {
            if (str.empty() || str.find_first_not_of(' ') == str.npos) {
                return;
            }
            auto not_space = str.find_first_not_of(' ');
            auto separator = str.find('m');
            auto comma = str.find(',');

            int dist = std::stoi(std::string(str.substr(not_space, separator - not_space)));
            std::string name = std::string(str.substr(separator + 5, comma - separator - 5));
            map_dist.insert({ name, dist });
            if (comma != str.npos) {
                ParseDistance(std::string(str.substr(comma + 1)), map_dist);
            }
            
        }

        /**
         * Удаляет пробелы в начале и конце строки
         */
        std::string_view Trim(std::string_view string) {
            const auto start = string.find_first_not_of(' ');
            if (start == string.npos) {
                return {};
            }
            return string.substr(start, string.find_last_not_of(' ') + 1 - start);
        }

        /**
         * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
         */
        std::vector<std::string_view> Split(std::string_view string, char delim) {
            std::vector<std::string_view> result;

            size_t pos = 0;
            while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
                auto delim_pos = string.find(delim, pos);
                if (delim_pos == string.npos) {
                    delim_pos = string.size();
                }
                if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                    result.push_back(substr);
                }
                pos = delim_pos + 1;
            }

            return result;
        }

        /**
         * Парсит маршрут.
         * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
         * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
         */
        std::vector<std::string_view> ParseRoute(std::string_view route) {
            if (route.find('>') != route.npos) {
                return Split(route, '>');
            }

            auto stops = Split(route, '-');
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        CommandDescription ParseCommandDescription(std::string_view line) {
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos) {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos) {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos) {
                return {};
            }

            auto comma = line.find(',');
            auto comma2 = line.find(',', comma + 1);

            if (comma2 == line.npos) {
                static std::string emp = ""s;
                return { std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1, comma2 - colon_pos + 1)),
                    emp };
            }

            return { std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1, comma2 - colon_pos + 1)),
                    std::string(line.substr(comma2 + 1))};
        }
    }
        void InputReader::ParseLine(std::string_view line) {
            auto command_description = detail::ParseCommandDescription(line);
            if (command_description) {
                commands_.push_back(std::move(command_description));
            }
        }

    

    void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) {



        std::sort(commands_.begin(), commands_.end(), [](detail::CommandDescription lhs, detail::CommandDescription rhs) {
            return lhs.command > rhs.command;
            });

        for (detail::CommandDescription com : commands_) {
            if (com.command == "Stop"s) {
                coord::Coordinates coordinates = detail::ParseCoordinates(com.description);
                std::unordered_map<std::string, int> map_distance;
                detail::ParseDistance(com.distantion, map_distance);
                catalogue.AddStop(com.id, coordinates, map_distance);
                continue;
            }
            if (com.command == "Bus"s) {
                std::vector<std::string_view> routes = detail::ParseRoute(com.description);
                std::vector<std::string> routes_string;
                for (auto i : routes) {
                    routes_string.push_back(static_cast<std::string>(i));
                }
                catalogue.AddBus(com.id, routes_string);
            }
        }

        // Реализуйте метод самостоятельно
    }
}