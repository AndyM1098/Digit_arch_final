#include "bi_input.hpp"

bi_input::bi_input(){
    clear_fields();
}

void bi_input::clear_fields(){
    MMSI = 0;
    date_i = 0;
    time_series.clear();
    name = "";
    imo = 0;
    callsign = "";
    v_type = 0;
    status = 0;
    length = 0.0;
    width = 0.0;
    draft = 0.0;
    cargo = "";
    trans_type = "";
}