#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float mTot, mFree;
  string fline, temp, ans;
  std::ifstream fstream(kProcDirectory + kMeminfoFilename);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      replace(fline.begin(), fline.end(), ':', ' ');
      std::istringstream l_stream(fline);
      while (l_stream >> temp >> ans) {
        if (temp == "MemTotal") { mTot = stof(ans); }
        if (temp == "MemFree") { mFree = stof(ans); }
      }
    }
  }

  return (mTot-mFree)/mTot;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string fline, t;
  std::ifstream fstream(kProcDirectory + kUptimeFilename);

  if (fstream.is_open())
  {
    getline(fstream, fline);
    std::istringstream l_stream(fline);
    l_stream >> t;
    return stol(t);
  }

  return stol(t);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string fline, temp;
  vector<string> vals;
  long ans = 0;
  std::ifstream fstream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename + to_string(pid));

  if (fstream.is_open()) {
    getline(fstream, fline);
    std::istringstream l_stream(fline);
    while (l_stream >> temp) vals.push_back(temp);
  }

  if (vals.size() > 21) 
    for (int i = 13; i < 17; i++)
      ans += stol(vals[i]);

  return ans;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jifs = CpuUtilization();
  return (stol(jifs[CPUStates::kGuest_]) + stol(jifs[CPUStates::kGuestNice_]) + stol(jifs[CPUStates::kIRQ_]) + stol(jifs[CPUStates::kNice_])
   + stol(jifs[CPUStates::kSoftIRQ_]) + stol(jifs[CPUStates::kSteal_]) + stol(jifs[CPUStates::kSystem_]) + stol(jifs[CPUStates::kUser_]));
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jifs = CpuUtilization();
  return (stol(jifs[CPUStates::kIdle_]) + stol(jifs[CPUStates::kIOwait_]));
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string fline, temp, value;
  std::ifstream fstream(kProcDirectory + kStatFilename);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      std::istringstream l_stream(fline);
      while (l_stream >> temp >> value) 
        if (temp == "processes") { return stoi(value); }
    }
  }

  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string fline, temp, value;
  std::ifstream fstream(kProcDirectory + kStatFilename);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      std::istringstream l_stream(fline);
      while (l_stream >> temp >> value) 
        if (temp == "procs_running") { return stoi(value); }
    }
  }

  return 0;
 }

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string fline, ans;
  std::ifstream fstream(kProcDirectory + kCmdlineFilename + to_string(pid));

  if (fstream.is_open())
  {
    getline (fstream, fline);
    std::istringstream l_stream(fline);
    l_stream >> ans;
  }

  return ans;  
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  int ram;
  string fline, temp, ans;
  std::ifstream fstream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      replace(fline.begin(), fline.end(), ':', ' ');
      std::istringstream l_stream(fline);
      while(l_stream >> temp >> ans)
        if (temp =="VmSize") ram = stof(ans);
    }
    return to_string(ram/1000);
  }  

  return 0;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string fline, temp, ans;
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      replace(fline.begin(), fline.end(), ':', ' ');
      std::istringstream l_stream(fline);
      while(l_stream >> temp >> ans)
        if (temp =="Uid") return ans;
    }  
  }

  return {""};
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string fline, temp, ans, xval;
  std::ifstream fstream(kPasswordPath);

  if (fstream.is_open()) {
    while (getline(fstream, fline)) {
      replace(fline.begin(), fline.end(), ':', ' ');
      std::istringstream l_stream(fline);
      while(l_stream >> temp >> xval >> ans)
        if (ans == Uid(pid)) return temp;
    }  
  }

  return {""}; 
}

//  DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string fline, t;
  std::ifstream fstream(kProcDirectory + to_string(pid) + kStatFilename);
  
  if (fstream.is_open()) {
    getline(fstream, fline);
    std::istringstream l_stream(fline);
    for (int i = 0; i < 22; i++)
      l_stream >> t;

    return stol(t) /sysconf(_SC_CLK_TCK);
  } 

  return 0;
}