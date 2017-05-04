#ifndef MONITOR_H_INCLUDED
#define MONITOR_H_INCLUDED

#include <cstdio>
#include <iostream>
#include <string>
#include <utils.h>

using namespace std;

class Monitor {
    public:
        std::ofstream fstream;
        std::string file;
        std::string cmd;
    
        Monitor();
        // Monitor(std::string filename); //I had problems with this guy

        void init(std::string filename, std::string command);
        void read_status();
        void flush();
        void close();
};

Monitor::Monitor() { };
void Monitor::init(std::string filename, std::string command) {
    file = filename;
    cmd = command;
    // fstream.open(filename, std::ofstream::trunc | std::ofstream::app);
    fstream.open(filename);
};

void Monitor::read_status() {
    fstream << get_prefix() << exec(cmd.c_str());
};

void Monitor::flush() {
    fstream.flush();
};

void Monitor::close() {
    fstream.close();
};

#endif