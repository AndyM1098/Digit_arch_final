#ifndef SHIPS_DEFINES
#define SHIPS_DEFINES

#include <string>
#include <vector>

#include "ts.hpp"

#define MMSI_FIELD 0
#define DATETIME_FIELD 1
#define LAT_FIELD 2
#define LON_FIELD 3
#define SOG_FIELD 4
#define COG_FIELD 5
#define HEADING_FIELD 6
#define VESSEL_NAME_FIELD 7
#define IMO_FIELD 8
#define CALL_SIGN_FIELD 9
#define VESSEL_TYPE_FIELD 10
#define STATUS_FIELD 11
#define LENGTH_FIELD 12
#define WIDTH_FIELD 13
#define DRAFT_FIELD 14
#define CARGO_FIELD 15
#define TRANSCEIVERCLASS_FIELD 16

#define __SHIP_SIZEOF_MMSI long long         // Maritime identifier
#define __SHIP_SIZEOF_DATE int               // Date
#define __SHIP_SIZEOF_TIMESERIES std::vector<ts*> // Vector of time series pointers
#define __SHIP_SIZEOF_NAME std::string       // Name of the ship
#define __SHIP_SIZEOF_IMO unsigned int       // IMO - International
#define __SHIP_SIZEOF_CALLSIGN std::string   // Call sign
#define __SHIP_SIZEOF_VESSEL_TYPE short      // Vessel type
#define __SHIP_SIZEOF_STATUS unsigned char   // Status
#define __SHIP_SIZEOF_LENGTH float           // Length of the ship
#define __SHIP_SIZEOF_WIDTH float            // Width of the ship
#define __SHIP_SIZEOF_DRAFT float            // Draft of the ship
#define __SHIP_SIZEOF_CARGO std::string      // Cargo information
#define __SHIP_SIZEOF_TRANS_TYPE std::string // Transceiver type

#endif