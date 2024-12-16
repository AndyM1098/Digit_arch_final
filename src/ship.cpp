#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include "ship.hpp"
#include "ship_defines.hpp"
#include "ts.hpp"
#include "bi_input.hpp"

ship::ship() {
    this->MMSI = 0;
    this->callsign = "";
    this->date_i = 0;
    this->imo = 0;
    this->date_i = 0;
    this->length = 0;
    this->width = 0;
    this->draft = 0;
    this->cargo = "";
    this->v_type = 0.0;
    this->name = "";
    this->time_series = {};
    this->status = 0;
}

void ship::insert_ship_info(std::vector<std::string> info){
    
    //Grab MMSI
    this->MMSI = !((int)info[MMSI_FIELD].size()) ? 0 : stoll(info[MMSI_FIELD]);

    //Extract and store datetime information
    if(info[DATETIME_FIELD] == ""){
        this->date_i = 0;
    }else{
        this->date_i = stoi(info[DATETIME_FIELD].substr(2, 2) +\
                        info[DATETIME_FIELD].substr(5, 2) +\
                        info[DATETIME_FIELD].substr(8, 2));
    }
    
    this->name = info[VESSEL_NAME_FIELD];
    this->trans_type = info[TRANSCEIVERCLASS_FIELD];
    this->callsign = info[CALL_SIGN_FIELD];
    this->cargo = info[CARGO_FIELD];

    // Encode other fields

    // Next three are floats --> Might encode these into ints 
    //      Resolution xxx.x
    this->length = !((int)info[LENGTH_FIELD].size()) ? 0.0 : stof(info[LENGTH_FIELD]);
    this->width = !((int)info[WIDTH_FIELD].size()) ? 0.0 : stof(info[WIDTH_FIELD]);
    this->draft = !((int)info[DRAFT_FIELD].size()) ? 0.0 : stof(info[DRAFT_FIELD]);

    //  Looking at COLREGS status code, can fit the following into a char!
    //      Encode empty to be 15 --> undefined according to COLREGGS;
    this->status = !((int)info[STATUS_FIELD].size()) ? 15 : static_cast<char>(stoi(info[STATUS_FIELD]));
    
    // This needs to be a short
    this->v_type = !((int)info[VESSEL_TYPE_FIELD].size()) ? 0 : static_cast<short>(stoi(info[VESSEL_TYPE_FIELD]));

    // IMO: Can cut off "IMO" substring since every IMO starts with it!
    this->imo = !((int)info[IMO_FIELD].size()) ? 0 : stoi(info[IMO_FIELD].substr(3,7));

    return;

}

template <typename FieldType>
void ship::insert_ts_data(ts *t,\
                    const std::string &s,\
                    const char start_bit_shift,\
                    const char upper_limit,\
                    FieldType ts::*field) {
    
    static std::string upper, lower;
    static std::stringstream ss;
    
    upper = lower = "";

    char shift;

    ss.clear();
    ss.str(s);
    std::getline(ss, upper, '.');
    std::getline(ss, lower, '.');

    shift = start_bit_shift - (upper_limit - static_cast<char>(upper.size()));

    // Reset the field before modifying
    t->*field = 0;

    // Process the upper part
    for (int i = 0; i < (int)upper.size(); i++) {
        t->*field |= static_cast<FieldType>((upper[i] - '0') << (--shift * 4));
    }

    // Process the lower part
    for (int i = 0; i < (int)lower.size(); i++) {
        t->*field |= static_cast<FieldType>((lower[i] - '0') << (--shift * 4));
    }

    return;
}

void ship::write_to_file(std::ofstream &out_file) {
    
    char len = 0;
    // MMSI // Condensed
    // cout << this->MMSI<<endl;

    out_file.write(reinterpret_cast<const char*>(&MMSI), sizeof(long long));

    // // DATE // Condensed
    // // cout << this->date_i << endl;
    out_file.write(reinterpret_cast<const char*>(&date_i), 3);

    // NAME // Condensed
    // printf("%s\n", this->name);
    len = (char)this->name.length();
    out_file.write(reinterpret_cast<const char*>(&len), 1);
    out_file.write(reinterpret_cast<const char*>(name.data()), len);

    // IMO
    // printf("%d\n", this->imo);
    out_file.write(reinterpret_cast<const char*>(&imo), __SIZEOF_INT__);

    // CallSign
    len = (char)this->callsign.size();
    out_file.write(reinterpret_cast<const char*>(&len), 1);
    out_file.write(reinterpret_cast<const char*>(&callsign[0]), len);
    
    // Vessel Type
    out_file.write(reinterpret_cast<const char*>(&v_type), sizeof(short));

    // Status
    out_file.write(reinterpret_cast<const char*>(&this->status), sizeof(char));
    
    // length
    out_file.write(reinterpret_cast<const char*>(&this->length), sizeof(float));

    // width
    out_file.write(reinterpret_cast<const char*>(&this->width), sizeof(float));
    
    // draft
    out_file.write(reinterpret_cast<const char*>(&this->draft), sizeof(float));

    // cargo
    len = (char)this->cargo.size();
    out_file.write(reinterpret_cast<const char*>(&len), 1);
    out_file.write(reinterpret_cast<const char*>(&this->cargo[0]), len);

    // transceiverclass
    len = (char)this->trans_type.size();
    out_file.write(reinterpret_cast<const char*>(&len), 1);
    out_file.write(reinterpret_cast<const char*>(&this->trans_type[0]), len);

    int ts_len = time_series.size();
    out_file.write(reinterpret_cast<const char*>(&ts_len), sizeof(int));

    ts* t;
    for(int i = 0; i < (int)time_series.size();i++){
        t = time_series[i];
        out_file.write(reinterpret_cast<const char*>(&t->time), sizeof(int));
        out_file.write(reinterpret_cast<const char*>(&t->lat), sizeof(unsigned int));
        out_file.write(reinterpret_cast<const char*>(&t->lon), sizeof(unsigned int));
        out_file.write(reinterpret_cast<const char*>(&t->sog), sizeof(unsigned short));
        out_file.write(reinterpret_cast<const char*>(&t->cog), sizeof(unsigned short));
        out_file.write(reinterpret_cast<const char*>(&t->heading), sizeof(unsigned short));
    }

    return;
}

void ship::insert_time_point(std::vector<std::string>& info){
    
    static std::string time;
    ts* t = new ts();

    time = "      .";
    static int indices[3] = {11, 14, 17};

    for(int i = 0; i < 3; i++){
        time[i * 2] = info[DATETIME_FIELD][indices[i]];
        time[(i * 2) + 1] = info[DATETIME_FIELD][indices[i] + 1];
    }

    insert_ts_data(t, time, 6, 6, &ts::time);
    // std::cout << info[DATETIME_FIELD] << std::endl << t->time << std::endl;
    insert_ts_data(t, info[LAT_FIELD], 7, 2, &ts::lat);

    insert_ts_data(t, info[LON_FIELD].substr(1), 8, 3, &ts::lon);
    insert_ts_data(t, info[SOG_FIELD], 4, 3, &ts::sog);
    insert_ts_data(t, info[COG_FIELD], 4, 3, &ts::cog);
    insert_ts_data(t, info[HEADING_FIELD], 4, 3, &ts::heading);
    
    this->time_series.push_back(t);

    return;
}

bool ship::insert_ship_info_from_binary(const bi_input &input){
    
    this->MMSI = input.MMSI;
    this->date_i = input.date_i;
    this->time_series = input.time_series;
    this->imo = input.imo;
    this->name = input.name;
    this->callsign = input.callsign;
    this->v_type = input.v_type;
    this->status = input.status;
    this->length = input.length;
    this->width = input.width;
    this->draft = input.draft;
    this->cargo = input.cargo;
    this->trans_type = input.trans_type;

    return true;

}

bool ship::compare_ships(ship* r_ship){
    
    if(date_i != r_ship->date_i){
        fprintf(stderr, "Error: ship's dates do not match up for ship with MMSI {%lld}\n", this->MMSI);
        return false;
    }
    
    if(this->time_series.size() != r_ship->time_series.size()){
        fprintf(stderr, "Error: ship's do not have the same number of time_series points MMSI{%lld}\n", this->MMSI);
        return false;
    }
    
    std::sort(this->time_series.begin(), this->time_series.end(), [](const ts* a, const ts* b) {
        return *a < *b; // Compare the objects, not the pointers
    });

    std::sort(r_ship->time_series.begin(), r_ship->time_series.end(), [](const ts* a, const ts* b) {
        return *a < *b; // Compare the objects, not the pointers
    });

    int num_time_points = (int)this->time_series.size();
    
    for(int i = 0; i < num_time_points; i++){
        if (*(this->time_series[i]) != *(r_ship->time_series[i])) {
            fprintf(stderr, "Error {%lld}: Time_series[%d] does not match!\n", this->MMSI, i);
            // this->time_series[i]->print(*this->time_series[i]);
            // r_ship->time_series[i]->print(*r_ship->time_series[i]);
            return false;
        }
    }

    if(this->imo != r_ship->imo){
        fprintf(stderr,
                "Error {%lld}: IMO does not match --> ( %u | %u )\n!",
                this->MMSI, this->imo, r_ship->imo);
        return false;
    }

    if (this->name != r_ship->name) {
        fprintf(stderr,\
                "Error {%lld}: Name does not match --> (%s|%s)\n",\
                this->MMSI, this->name.c_str(), r_ship->name.c_str());
        return false;
    }

    if (this->callsign != r_ship->callsign) {
        fprintf(stderr, "Error {%lld}: Callsign does not match\n", this->MMSI);
        return false;
    }

    if (this->v_type != r_ship->v_type) {
        fprintf(stderr, "Error {%lld}: Vessel type does not match\n", this->MMSI);
        return false;
    }

    if (this->status != r_ship->status) {
        fprintf(stderr, "Error {%lld}: Status does not match\n", this->MMSI);
        return false;
    }

    if (this->length != r_ship->length) {
        fprintf(stderr, "Error {%lld}: Length does not match\n", this->MMSI);
        return false;
    }

    if (this->width != r_ship->width) {
        fprintf(stderr, "Error {%lld}: Width does not match\n", this->MMSI);
        return false;
    }

    if (this->draft != r_ship->draft) {
        fprintf(stderr, "Error {%lld}: Draft does not match\n", this->MMSI);
        return false;
    }

    if (this->cargo != r_ship->cargo) {
        fprintf(stderr, "Error {%lld}: Cargo does not match\n", this->MMSI);
        return false;
    }

    if (this->trans_type != r_ship->trans_type) {
        fprintf(stderr, "Error {%lld}: Transceiver type does not match\n", this->MMSI);
        return false;
    }

    return true;
}