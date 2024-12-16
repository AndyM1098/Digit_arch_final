#pragma once

#include <vector>
#include <string>

#include "ts.hpp"

/**
 * @class bi_input
 * @brief A structure representing input data for a maritime vessel.
 *
 * The bi_input structure contains detailed information about a vessel, including its
 * unique identifiers, physical dimensions, and associated time-series data points.
 * It serves as a container for maritime data parsed from raw input files.
 */
struct bi_input{

    long long MMSI;             //Martime identifier
    int date_i;                 //Date
    std::vector<ts*> time_series;    // See struct
    std::string name;
    unsigned int imo;           // IMO - International
    std::string callsign;
    short v_type;               // Vessel Type
    unsigned char status;
    float length;
    float width;
    float draft;
    std::string cargo;
    std::string trans_type;

    bi_input();
    void clear_fields();
};
