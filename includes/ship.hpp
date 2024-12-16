#pragma once
#include <vector>

#include "ship_defines.hpp"
#include "ts.hpp"
#include "fstream"
#include "bi_input.hpp"

class ship {

    public:
        // Constructor
        ship();

        
        void insert_ship_info(std::vector<std::string> info);
        void write_to_file(std::ofstream &out_file);
        void insert_time_point(std::vector<std::string>& info);
        bool insert_ship_info_from_binary(const bi_input &input);
        bool compare_ships(ship* r_ship);

    private:

        template <typename FieldType>
        void insert_ts_data(ts *t, 
                            const std::string &s, 
                            const char start_bit_shift, 
                            const char upper_limit, 
                            FieldType ts::*field);

        __SHIP_SIZEOF_MMSI MMSI;             // Maritime identifier
        __SHIP_SIZEOF_DATE date_i;           // Date
        __SHIP_SIZEOF_TIMESERIES time_series; // Vector of time series
        __SHIP_SIZEOF_NAME name;             // Name
        __SHIP_SIZEOF_IMO imo;               // IMO - International
        __SHIP_SIZEOF_CALLSIGN callsign;     // Call sign
        __SHIP_SIZEOF_VESSEL_TYPE v_type;    // Vessel type
        __SHIP_SIZEOF_STATUS status;         // Status
        __SHIP_SIZEOF_LENGTH length;         // Length
        __SHIP_SIZEOF_WIDTH width;           // Width
        __SHIP_SIZEOF_DRAFT draft;           // Draft
        __SHIP_SIZEOF_CARGO cargo;           // Cargo
        __SHIP_SIZEOF_TRANS_TYPE trans_type; // Transceiver type
        friend class ships;
};