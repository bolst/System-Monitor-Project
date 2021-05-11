#include "processor.h"
#include "linux_parser.h"
#include <iostream>

using std::string;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float act_u;
    string fline, temp, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    std::ifstream fstream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);

    if (fstream.is_open()) {
        getline(fstream, fline);
        std::istringstream l_stream(fline);
        while (l_stream >> temp >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
            if (temp == "cpu") {
                act_u = stof(user) + stof(nice) + stof(system) + stof(irq) + stof(softirq)
                 + stof(idle) + stof(iowait) + stof(steal);

                return (act_u - stof(idle) - stof(iowait))/act_u;
            }
        }
    }

    return 0;
}