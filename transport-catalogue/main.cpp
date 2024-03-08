#include <cstdio>


//#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

int main() {
    JSON in_json;

    in_json.DestructJSON();
    in_json.CreateJSON();
    
    in_json.PrintJSON();
   
    
    
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
}