import os
import requests
import zipfile
import re
from datetime import date, timedelta
from concurrent.futures import ProcessPoolExecutor, as_completed
import pandas as pd
from dataclasses import dataclass
import time
import shutil
import csv
from collections import defaultdict
import io

# Field index constants
MMSI_FIELD = 0
DATETIME_FIELD = 1
LAT_FIELD = 2
LON_FIELD = 3
SOG_FIELD = 4
COG_FIELD = 5
HEADING_FIELD = 6
VESSEL_NAME_FIELD = 7
IMO_FIELD = 8
CALL_SIGN_FIELD = 9
VESSEL_TYPE_FIELD = 10
STATUS_FIELD = 11
LENGTH_FIELD = 12
WIDTH_FIELD = 13
DRAFT_FIELD = 14
CARGO_FIELD = 15

# SHIP_WHITELIST = set([30, 35, 36, 37, 60, 61, 62, 63, 64, 65,
#                     66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
#                     76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
#                     86, 87, 88, 89])

class ship:
    
    __slots__ = [
        "MMSI", 
        "date", 
        "IMO", 
        "callsign", 
        "length", 
        "width", 
        "draft", 
        "cargo", 
        "vessel_type", 
        "name", 
        "time_series"
    ]
    
    def __init__(self):
        self.MMSI = int()
        self.date = str()
        self.IMO = int()
        self.callsign = int()
        self.length = int()
        self.width = int()
        self.draft = int()
        self.cargo = int()
        self.vessel_type = int()
        self.name = str()
        self.time_series = list()

    def add_ship(self,
                 MMSI: int,
                 date: str,
                 vesselName: str,
                 IMO: str,
                 callSign: str,
                 vesselType: int,
                 leng: int,
                 width: int,
                 draft: int,
                 cargo: int,
                 ):

        # Already an int
        self.MMSI = int(MMSI)

        #Convert date to an int
        
        #   each digit represents a part of the YYYY:MM:DD
        #   If we take note that this file is in 2020
        #       We can cut it out and just the the 19/20
        #       so then the digit representation of the date becomes
        #        YYMMDD
        #   With a bit of bit logic, we can extract it out again
        
        year = date[2:4]
        month = date[5:7]
        day = date[8:]

        self.date = int(year+month+day)
        
        self.IMO = int(re.sub("IMO", "", IMO))
        
        
        b_callsign = int()
        char_vals = [int(c) for c in callsign]
        
        
        self.callsign = callSign
        
        self.length = leng
        self.width = width
        self.draft = draft
        self.cargo = cargo
        self.vessel_type = vesselType
        self.name = vesselName

    def add_point_data(self, 
                       time: str,
                       lat: float,
                       lon: float,
                       sog: float, 
                       cog: float, 
                       heading: float):
        data_point = time_point(time=time, lat=lat, lon=lon, sog=sog, cog=cog, heading=heading)
        self.time_series.append(data_point)

    def compress_to_binary(self):
        ship_info = [
            self.MMSI,
            self.date,
            self.IMO,
            self.callsign,
            self.length,
            self.width,
            self.draft,
            self.cargo,
            self.vessel_type,
            self.name
        ]
        time_series_data = [
            f"{tp.compress_time()};{tp.lat};{tp.lon};{tp.sog};{tp.cog};{tp.heading}"
            for tp in self.time_series
        ]
        time_series_str = "|".join(time_series_data)
        return ",".join(map(str, ship_info)) + f',{time_series_str}'

def get_raw_csv(url: str, extract_path: str):
    
    s = time.time()
    file_name = url.split("/")[-1]

    if not os.path.exists(extract_path):
        os.makedirs(extract_path, exist_ok=True)

    response = requests.get(url, stream=True)

    if response.status_code == 200:
        with zipfile.ZipFile(io.BytesIO(response.content)) as zip_ref:
            zip_ref.extractall(path=extract_path)
    else:
        raise Exception(f"Failed to download {url}. HTTP status code: {response.status_code}")

    raw_csv_path = os.path.join(extract_path, re.sub(".zip", ".csv", file_name))
    e = time.time()
    print(f"Downloaded and extracted {file_name} to {extract_path} in {e - s} seconds")
    return raw_csv_path

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
def process_file(data_file, site, raw_data_folder, compressed_folder):
    try:
        full_time_start = time.time()
        url = site + data_file

        # # Download the zip file
        # downloaded_zip_path = get_raw_csv(url, download_path)

        # Unzip the raw data

        csv_file_location = get_raw_csv(url, raw_data_folder)
        # Process the data
        s_time = time.time()
        ship_info = defaultdict(ship)
        line_num = 1

        with open(csv_file_location, "r") as f:
            reader = csv.reader(f)
            next(reader)  # Skip header
            for info in reader:
                ship_num = info[0]
                if info[VESSEL_TYPE_FIELD] == '' or\
                    int(info[VESSEL_TYPE_FIELD], base=10) not in SHIP_WHITELIST:
                    continue
                
                date, time_info = info[DATETIME_FIELD].split("T")
                
                if ship_num not in ship_info:
                    ship_info[ship_num].add_ship(
                        info[MMSI_FIELD],
                        date,
                        info[VESSEL_NAME_FIELD],
                        info[IMO_FIELD],
                        info[CALL_SIGN_FIELD],
                        info[VESSEL_TYPE_FIELD],
                        info[LENGTH_FIELD],
                        info[WIDTH_FIELD],
                        info[DRAFT_FIELD],
                        info[CARGO_FIELD],
                    )

                ship_info[ship_num].add_point_data(
                    time_info,
                    float(info[LAT_FIELD]),
                    float(info[LON_FIELD]),
                    float(info[SOG_FIELD]),
                    float(info[COG_FIELD]),
                    float(info[HEADING_FIELD]),
                )

        # Save compressed data
        print(f"Processed: {data_file} in {time.time() - s_time} seconds")
        s_time = time.time()
        compressed_path = os.path.join(compressed_folder, re.sub(".zip", ".csv", data_file))
        with open(compressed_path, "w") as f:
            f.write("MMSI,Date,IMO,CallSign,Length,Width,Draft,Cargo,VesselType,Name,TimeSeries\n")
            for s in ship_info.values():
                f.write(s.to_compressed_csv() + "\n")
        print(f"Compressed: {data_file} in {time.time() - s_time}")
        # Cleanup
        os.remove(csv_file_location)
        
        return f"Fully processed: {data_file} in {time.time() - full_time_start} seconds"

    except Exception as e:
        print(f"Error processing {data_file}: {e}")
        return f"Error: {data_file}"


def process_files_concurrently(files, site, unzip_path, compressed_folder):
    with ProcessPoolExecutor(max_workers=4) as executor:
        futures = {
            executor.submit(
                process_file, data_file, site, unzip_path, compressed_folder): data_file
            for data_file in files
        }
        for future in as_completed(futures):
            data_file = futures[future]
            try:
                result = future.result()
                print(result)
            except Exception as exc:
                print(f"{data_file} generated an exception: {exc}")

# Usage
year = 2019
site = "https://coast.noaa.gov/htdata/CMSP/AISDataHandler/" + str(year) + "/"
raw_data_folder = "raw_data/"
download_path = "downloads/"
compressed_folder = "compressed_data/"

if not os.path.exists(compressed_folder):
    os.makedirs(compressed_folder)

files = generate_files_for_year(year)
# print(files[0])
time_start = time.time()
process_files_concurrently(files, site, raw_data_folder, compressed_folder)
print(f"Time to process all files: {time.time() - time_start}")
