#pragma once

#include <sstream>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "ship.hpp"
#include "string.h"
#include "bi_input.hpp"
#include "ship_defines.hpp"

class ships{

    public:

        ships();
        
        //Main compress/decompress
        bool compress_raw_csv(const std::string &in_file_path, const std::string &out_file_path);
        bool read_from_compressed(const std::string &in_file_path);
        bool compare_info(ships &r);

        // Utility functions
        float get_load_factor();
        size_t get_num_ships();
        int get_raw_csv_count();
    private:

        //Each ship is keyed in on MMSI
        std::unordered_map<__SHIP_SIZEOF_MMSI, ship*> s;
        int raw_csv_line_count;
};