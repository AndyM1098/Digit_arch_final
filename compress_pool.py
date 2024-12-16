import os
import requests
import zipfile
import re
from datetime import date, timedelta
from concurrent.futures import ProcessPoolExecutor, as_completed
import pandas as pd
from dataclasses import dataclass
import time
from collections import defaultdict
import io
import subprocess

def generate_files_for_year(year):
    start_date = date(year, 1, 1)
    end_date = date(year, 12, 31)
    delta = timedelta(days=1)

    files = []
    current_date = start_date
    while current_date <= end_date:
        files.append(f"AIS_{current_date.strftime('%Y_%m_%d')}.zip")
        current_date += delta

    return files

# Main processing function for a single file
def process_file(data_file):
    
    cpp_program = "./bin/main"  # Path to your C++ executable

    # Run the C++ program and capture its output
    result = subprocess.run([cpp_program, data_file.strip(".csv")], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, check=True)

    # Append the output to a single file safely

    return result.stdout

def process_files_concurrently(files):
    
    results = []
    
    with ProcessPoolExecutor(max_workers=4) as executor:
        futures = {
            executor.submit(
                process_file, data_file): data_file
            for data_file in files
        }
        for future in as_completed(futures):
            data_file = futures[future]
            try:
                result = future.result()
                results.append(result)
                print("Proccessed: " + data_file)
            except Exception as exc:
                print(f"{data_file} generated an exception: {exc}")

    with open("results.txt", "w") as f:
        f.write("file_name,csv_read_time,binary_read_time,compare_time,csv_size_bits,bin_size_bits,compression_ratio,space_saving_percent,speedup,num_ships,raw_csv_line_count")
        for result in results:
            f.write(result)

files =os.listdir("raw/")
print(files)
time_start = time.time()
process_files_concurrently(files)
print(f"Time to process all files: {time.time() - time_start}")
