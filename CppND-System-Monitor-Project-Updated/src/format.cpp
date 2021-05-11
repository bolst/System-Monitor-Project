#include <string>
#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {

    long hour = seconds/3600;
    long min = seconds/60;

    while (seconds >= 60){
        seconds = seconds - 60;
        min = min + 1;
    }

    while (min >= 60){
        min = min - 60;
        hour = hour + 1;
    }

    return std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(seconds) + "\n";
}