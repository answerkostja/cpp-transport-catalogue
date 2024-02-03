#include <string>
#include <tuple>
#include <iostream>
#include <iomanip>

#include "stat_reader.h"

using namespace std::literals;

namespace transport {

    void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
        std::ostream& output) {
        // Реализуйте самостоятельно
        auto space_pos = request.find(' ');
        if (std::string(request.substr(0, space_pos)) == "Bus"s) {
            std::string_view request_bus = request.substr(space_pos + 1);
            std::tuple<int, int, double> result = tansport_catalogue.GetBusInfo(request_bus);

            int stops = std::get<0>(result);
            int unique_stops = std::get<1>(result);
            double length = std::get<2>(result);

            if (stops == 0 && unique_stops == 0 && length == 0) {
                std::string res = static_cast <std::string>(request) + ": not found"s + "\n"s;
                output << res;

                return;
            }


            output << static_cast <std::string>(request) << ": "s << std::to_string(stops) << " stops on route, "s << std::to_string(unique_stops) << " unique stops, "s << std::setprecision(6) << length << " route length"s << std::endl;
        }
        else if (request.substr(0, space_pos) == "Stop"s) {
            std::string_view request_stop = request.substr(space_pos + 1);
            std::string result = tansport_catalogue.GetStopInfo(request_stop);
            if (result == std::string{ "No stops"s }) {
                std::string res = static_cast <std::string>(request) + ": not found"s + "\n"s;
                output << res;
                return;
            }
            else if (result.empty()) {
                std::string res = static_cast <std::string>(request) + ": no buses"s + "\n"s;
                output << res;
                return;
            }

            std::string res = static_cast <std::string>(request) + ": buses "s + result;

            res.erase(res.size() - 1);
            res += "\n";
            output << res;
            return;
        }



    }
}