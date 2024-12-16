#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <string.h>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <filesystem>
#include <mutex>

#include "ship_defines.hpp"
#include "ts.hpp"
#include "bi_input.hpp"
#include "ship.hpp"
#include "ships.hpp"

using namespace std;
namespace fs = std::filesystem;

void process_file(const string& file_name, const string& in_file_path, const string& out_file_path) {
    ships raw;
    ships compressed;

    try {
        // Timing CSV read
        auto start = chrono::high_resolution_clock::now();
        raw.compress_raw_csv(in_file_path, out_file_path); // Includes reading CSV and writing binary
        auto end = chrono::high_resolution_clock::now();
        auto csv_read_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        // Timing binary read
        start = chrono::high_resolution_clock::now();
        compressed.read_from_compressed(out_file_path);
        end = chrono::high_resolution_clock::now();
        auto binary_read_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        // // Timing compare_info
        // start = chrono::high_resolution_clock::now();
        // raw.compare_info(compressed);
        // end = chrono::high_resolution_clock::now();
        // auto compare_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        int compare_time_temp = 0;
        // File size calculations
        auto csv_size = fs::file_size(in_file_path);
        auto bin_size = fs::file_size(out_file_path);

        // Compression ratio
        double compression_ratio = static_cast<double>(csv_size) / static_cast<double>(bin_size);

        // Space saving
        double space_saving = 1.0 - (static_cast<double>(bin_size) / static_cast<double>(csv_size));

        // Speedup calculations
        double speedup = static_cast<double>(csv_read_time) / binary_read_time;

        // Write results to output file
        //std::lock_guard<std::mutex> lock(output_mutex); // Protect shared output
        cout << file_name << ","\
            << csv_read_time << ","\
            << binary_read_time << ","\
            << compare_time_temp << " ,"\
            << csv_size * 8 << ","\
            << bin_size * 8 << ","\
            << compression_ratio << ","\
            << (space_saving * 100.0) << ","\
            << speedup << ","\
            << raw.get_num_ships() << ","\
            << raw.get_raw_csv_count() << "\n";

    } catch (const std::exception& e) {
        cout << "Error processing file: " << file_name << "\n";
        cout << "Exception: " << e.what() << "\n\n";
        cerr << "Error processing file: " << file_name << ". Exception: " << e.what() << endl;
    }

    return;
}

int main(int argc, char** argv) {
    
    string in_file = argv[1];
    
    (void)argc;
    (void)argv;

    string input_folder = "raw/";
    string output_folder = "bin/";
    
    string in_file_path = input_folder + in_file + ".csv";
    string out_file_path = output_folder + in_file + ".bin";

    process_file(in_file, in_file_path, out_file_path);

    return 0;
}
