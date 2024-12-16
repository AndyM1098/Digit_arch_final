#include "ts.hpp"
#include <iostream>

bool ts::operator<(const ts &other) const {
        
    if (time < other.time) return true;
    if (time > other.time) return false;

    if (lat < other.lat) return true;
    if (lat > other.lat) return false;

    if (lon < other.lon) return true;
    if (lon > other.lon) return false;

    if (sog < other.sog) return true;
    if (sog > other.sog) return false;

    if (cog < other.cog) return true;
    if (cog > other.cog) return false;

    if (heading < other.heading) return true;
    if (heading > other.heading) return false;

    return false; // All fields are equal, so this is not less
}

void ts::print(const ts& instance) {
    std::cout << "time: " << instance.time << "\n"
                << "lat: " << instance.lat << "\n"
                << "lon: " << instance.lon << "\n"
                << "sog: " << instance.sog << "\n"
                << "cog: " << instance.cog << "\n"
                << "heading: " << instance.heading << "\n";
}
bool ts::operator!=(const ts &other) const {
    return time != other.time ||
            lat != other.lat ||
            lon != other.lon ||
            sog != other.sog ||
            cog != other.cog ||
            heading != other.heading;
}