#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid) : pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

int Process::Pid_Const() const { return pid_; }

//DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
    string fline;
    float time, sec;
    std::ifstream fstream(LinuxParser::kProcDirectory + to_string(Pid_Const()) + LinuxParser::kStatFilename);

    if (fstream.is_open()) {
        while (getline(fstream, fline)) {
            replace(fline.begin(), fline.end(), '(', '_');
            replace(fline.begin(), fline.end(), ')', '_');
            replace(fline.begin(), fline.end(), '-', '_');
            
            std::istringstream l_stream(fline);
            std::istream_iterator<string> start(l_stream), end;
            vector<string> cpus(start, end);

            time = stoi(cpus[13]) + stoi(cpus[14]) + stoi(cpus[15]) + stoi(cpus[16]);
            sec = LinuxParser::UpTime() - stof(cpus[21]) / sysconf(_SC_CLK_TCK);
        }
    }

    return (time / sysconf(_SC_CLK_TCK)) / sec;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return (this->CpuUtilization() > a.CpuUtilization()); }