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
    
        Monitor();
        // Monitor(std::string filename); //I had problems with this guy

        void init(std::string filename);
        void close();
        void writeCmdResult(std::string cmd, std::string preffix);
};

Monitor::Monitor() { };
void Monitor::init(std::string filename) {
    file = filename;
    fstream.open(filename, std::ofstream::trunc | std::ofstream::app);
};

void Monitor::writeCmdResult(std::string cmd, std::string preffix) {
    fstream << preffix << exec(cmd.c_str());
};

void Monitor::close() {
    fstream.close();
};

#endif