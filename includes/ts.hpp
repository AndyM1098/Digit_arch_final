#pragma once
/**
 * @class ts
 * @brief A structure representing a time-series data point for maritime navigation.
 *
 * The ts structure encapsulates key navigation parameters such as time, latitude, longitude,
 * speed over ground (sog), course over ground (cog), and heading. It also provides
 * comparison operators for sorting and equality checks, as well as a static print function
 * for displaying the structure's contents.
 */
struct ts{
    
    int time = 0;
    unsigned int lat = 0;
    unsigned int lon = 0;
    unsigned short sog = 0;
    unsigned short cog = 0;
    unsigned short heading = 0;

    bool operator<(const ts &other) const;
    bool operator!=(const ts &other) const;
    static void print(const ts& instance);

};

