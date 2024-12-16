#include "ships.hpp"
#include "ship_defines.hpp"

ships::ships(){
    s.reserve(1300 * 3);
    this->raw_csv_line_count = 0;
}

float ships::get_load_factor(){
    return s.load_factor();
}

bool ships::compress_raw_csv(const std::string &in_file_path, const std::string &out_file_path){
    
    std::ifstream in_file(in_file_path);

    if (!in_file.is_open()) {
        throw std::runtime_error("Failed to open file: " + in_file_path);
    }

    std::vector<std::string> info(17, "");
    std::stringstream ss;
    std::string line = "";
    std::string token = "";
    int idx = 0;
    int k = 0;
    //int line_num = 0;

    //SKIP HEADER
    getline(in_file, line);

    while(getline(in_file, line)){

        this->raw_csv_line_count++;
        std::fill(info.begin(), info.end(), "");
        ss.clear();
        ss.str("");
        ss.str(line);
        idx = 0;

        while (std::getline(ss, token, ',')){
            info[idx] = token;
            idx++;
        }
        
        k = stoi(info[MMSI_FIELD]);
        
        //Avoid using find, Two hashs no bueno. 
        auto it = this->s.insert(std::pair<int, ship*>(k, new ship()));
        
        //Newly inserted means new ship was found
        if(it.second == true){
            this->s[k]->insert_ship_info(info);
        }

        this->s[k]->insert_time_point(info);

        line.clear();

    }

    in_file.close();
    
    // printf("Number of lines from raw csv: %d\n", line_num);
    
    int num_ships = this->s.size();
    // cout << num_ships << endl;
    // out_file(out_file_path, ios::binary);
    
    std::unordered_map<long long, ship*>::iterator it = this->s.begin();
    std::unordered_map<long long, ship*>::iterator it_end = this->s.end();
    
    std::ofstream out_file(out_file_path, std::ios::binary);
    
    if( !out_file.is_open()){
        in_file.close();
        throw std::runtime_error("Failed to open file: " + out_file_path);
    }

    // Write num of ships
    out_file.write(reinterpret_cast<const char*>(&num_ships), sizeof(int));

    while(it != it_end){
        it->second->write_to_file(out_file);
        it++;
    }

    out_file.close();

    return true;
}

// bool ships::read_from_compressed(const std::string &in_file_path){
    
//     if(s.size() != 0){
//         fprintf(stderr, "ERROR: map is not empty!\n");
//         return false;
//     }

//     std::ifstream in_file(in_file_path, std::ios::binary);

//     if(!in_file.is_open()){
//         throw std::runtime_error("Failed to open " + in_file_path);
//     }

//     int num_ships = 0;
//     in_file.read(reinterpret_cast<char*>(&num_ships), sizeof(int));

//     bi_input input;
//     int num_time_series = 0;
//     int len = 0;

//     for(int i = 0; i < num_ships; i++){
        
//         in_file.read(reinterpret_cast<char*>(&(input.MMSI)), __SIZEOF_LONG_LONG__);

//         // cout << input.MMSI << endl;

//         // in_file.read(reint)

//         in_file.read(reinterpret_cast<char*>(&input.date_i), 3);

//         len = 0;
//         in_file.read(reinterpret_cast<char*>(&len), 1);
//         // cout << "name_len: " << len << endl;
        
//         if(len != 0){
//             input.name.resize(len);
//             in_file.read(reinterpret_cast<char *>(&(input.name[0])), len);
//         }
        
//         in_file.read(reinterpret_cast<char *>(&(input.imo)), sizeof(unsigned int));

//         len = 0;
//         in_file.read(reinterpret_cast<char*>(&len), 1);
//         if(len != 0){
//             input.callsign.resize(len);
//             in_file.read(reinterpret_cast<char *>(&(input.callsign[0])), len);
//         }

//         // cout << "call sign: " << input.callsign << endl;
        
//         in_file.read(reinterpret_cast<char*>(&input.v_type), __SIZEOF_SHORT__);
//         // cout << "v type: " << input.v_type << endl;

//         in_file.read(reinterpret_cast<char *>(&input.status), 1);
//         // cout << "status: " << input.status << endl;

//         in_file.read(reinterpret_cast<char*>(&(input.length)), __SIZEOF_FLOAT__);
//         // cout << "length: " << input.length << endl;

//         in_file.read(reinterpret_cast<char*>(&(input.width)), __SIZEOF_FLOAT__);
//         // cout << "width: " << input.width << endl;
        
//         in_file.read(reinterpret_cast<char*>(&(input.draft)), __SIZEOF_FLOAT__);
//         // cout << "draft: " << input.draft << endl;

//         len = 0;
//         in_file.read(reinterpret_cast<char*>(&len), 1);
//         if(len != 0){
//             input.cargo.resize(len);
//             in_file.read(reinterpret_cast<char *>(&(input.cargo[0])), len);
//         }
//         // cout << "cargo: " << input.cargo << endl;

//         len = 0;
//         in_file.read(reinterpret_cast<char*>(&len), 1);
//         if(len != 0){
//             input.trans_type.resize(len);
//             in_file.read(reinterpret_cast<char *>(&(input.trans_type[0])), len);

//         }
//         // cout << "trans_type: " << input.trans_type << endl;

//         in_file.read(reinterpret_cast<char *>(&num_time_series), __SIZEOF_INT__);
//         // cout << num_time_series << endl;
        
//         std::vector<ts*> time_series;
//         ts* t = nullptr;

//         for(int i = 0; i < num_time_series; i++){
//             t = new ts();
            
//             in_file.read(reinterpret_cast<char *>(&(t->time)), __SIZEOF_INT__);
//             // cout << "time: " << t->time << endl;
//             in_file.read(reinterpret_cast<char*>(&(t->lat)), __SIZEOF_INT__);
//             // cout << "lat: " << t->lat << endl;
//             in_file.read(reinterpret_cast<char*>(&(t->lon)), __SIZEOF_INT__);
//             // cout << "lon: " << t->lon << endl;
//             in_file.read(reinterpret_cast<char*>(&(t->sog)), __SIZEOF_SHORT__);
//             // cout << "sog: " << t->sog << endl;
//             in_file.read(reinterpret_cast<char*>(&(t->cog)), __SIZEOF_SHORT__);
//             // cout << "cog: " << t->cog << endl;
//             in_file.read(reinterpret_cast<char*>(&(t->heading)), __SIZEOF_SHORT__);
//             // cout << "heading: " << t->heading << endl;
//             input.time_series.push_back(t);
//             // break;
//         }


//         // cout << input.MMSI << endl;
//         auto it = s.insert(std::pair<__SHIP_SIZEOF_MMSI, ship*>(input.MMSI, nullptr));

//         if(it.second == true){
//              s[input.MMSI] = new ship();
//         }

//         s[input.MMSI]->insert_ship_info_from_binary(input);
        
//         // break;
//         input.clear_fields();
//     }

//     in_file.close();
    
//     //printf("Read in: %s\n", in_file_path.c_str());
//     return true;
// }

bool ships::read_from_compressed(const std::string& in_file_path) {
    if (s.size() != 0) {
        fprintf(stderr, "ERROR: map is not empty!\n");
        return false;
    }

    FILE* in_file = fopen(in_file_path.c_str(), "wb");
    if (!in_file) {
        throw std::runtime_error("Failed to open " + in_file_path);
    }

    int num_ships = 0;
    fread(&num_ships, sizeof(int), 1, in_file);

    bi_input input;
    int num_time_series = 0;
    unsigned char len = 0;

    for (int i = 0; i < num_ships; i++) {
        fread(&input.MMSI, sizeof(long long), 1, in_file);
        fread(&input.date_i, 3, 1, in_file);

        // Read vessel name
        len = 0;
        fread(&len, 1, 1, in_file);
        if (len > 0) {
            input.name.resize(len);
            fread(&input.name[0], len, 1, in_file);
        }

        fread(&input.imo, sizeof(unsigned int), 1, in_file);

        // Read callsign
        len = 0;
        fread(&len, sizeof(unsigned char), 1, in_file);
        if (len > 0) {
            input.callsign.resize(len);
            fread(&input.callsign[0], len, 1, in_file);
        }

        fread(&input.v_type, sizeof(short), 1, in_file);
        fread(&input.status, sizeof(char), 1, in_file);
        fread(&input.length, sizeof(float), 1, in_file);
        fread(&input.width, sizeof(float), 1, in_file);
        fread(&input.draft, sizeof(float), 1, in_file);

        // Read cargo
        len = 0;
        fread(&len, sizeof(unsigned char), 1, in_file);
        if (len > 0) {
            input.cargo.resize(len);
            fread(&input.cargo[0], len, 1, in_file);
        }

        // Read transceiver class
        len = 0;
        fread(&len, sizeof(unsigned char), 1, in_file);
        if (len > 0) {
            input.trans_type.resize(len);
            fread(&input.trans_type[0], len, 1, in_file);
        }

        fread(&num_time_series, sizeof(int), 1, in_file);

        input.time_series.clear();
        for (int j = 0; j < num_time_series; j++) {
            ts* t = new ts();
            fread(&t->time, sizeof(int), 1, in_file);
            fread(&t->lat, sizeof(int), 1, in_file);
            fread(&t->lon, sizeof(int), 1, in_file);
            fread(&t->sog, sizeof(short), 1, in_file);
            fread(&t->cog, sizeof(short), 1, in_file);
            fread(&t->heading, sizeof(short), 1, in_file);
            input.time_series.push_back(t);
        }

        // Insert into map
        // Assume each ship is unique, have to insert with no problems.
        auto it = this->s.insert(std::pair<long long, ship*>(input.MMSI, new ship())).first;
        it->second->insert_ship_info_from_binary(input);
        input.clear_fields();
    }

    fclose(in_file);
    return true;
}

bool ships::compare_info(ships &r){
    
    //Check num ships:
    if(r.s.size() != this->s.size()){
        std::cerr << "Num of ships are not correct!\n";
        return false;
    }

    std::unordered_map<__SHIP_SIZEOF_MMSI, ship*>::iterator it = this->s.begin();
    std::unordered_map<__SHIP_SIZEOF_MMSI, ship*>::iterator it_end =this->s.end();

    while(it != it_end){
        
        std::unordered_map<__SHIP_SIZEOF_MMSI, ship*>::iterator r_ship = r.s.find(it->second->MMSI);
        if(r_ship == r.s.end()){
            fprintf(stderr, "Error: Ship with MMSI {%lld} is not in in ship_r", it->first);
            return false;
        }

        if(this->s[it->second->MMSI]->compare_ships(r_ship->second) == false){
            return false;
        }

        it++;
    }
    
    return true;
}

int ships::get_raw_csv_count(){
    return this->raw_csv_line_count;
}
size_t ships::get_num_ships(){
    return this->s.size();
}