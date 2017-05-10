#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <unistd.h>
#include <monitoring.h>

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " FOLDER_NFS" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    //from http://stackoverflow.com/a/18027897/3136474
    if (!folder.empty() && folder.back() != '/')
        folder += '/'; //add trailing slash if not present
    std::string file_cmd = folder + "exec";
    std::string file_start = folder + "start";
    std::string file_stop = folder + "stop";
    std::cout << "Waiting for " << file_start << " to show up..." << std::endl;

    int i = 0;
    while (true) {
        i++;
        if (i % 10 == 0) { //echo each 10s
            std::cout << to_string(i) << " seconds waiting for start file" << std::endl;
        }

        std::string cmd = "ls " + folder;
        std::string ls_output = exec(cmd.c_str());
        std::cout << ls_output << std::endl;
        
        std::ifstream ifcmd(file_cmd);
        if (ifcmd.good()) {
            std::cout << "Command file " << file_cmd << " detected, but it should not be" << std::endl;
        } else {
            ifcmd.close();
        }

        std::ifstream ifstart(file_start);
        if (ifstart.good()) {
            std::cout << "Start file detected, waiting for " << file_stop << " to show up" << std::endl;
            remove(file_start.c_str());

            int c = 0;
            while (true) {
                c++;
                if (c % 10 == 0) { //echo each 10s
                    std::cout << to_string(c) << " monitoring iteration" << std::endl;
                }

                if (file_exists(file_stop)) {
                    remove(file_stop.c_str());
                    std::cout << "Stop file detected, stopping." << std::endl;
                    break;
                }
            }

            std::cout << "Stopped. Waiting for " << file_start << " to show up again..." << std::endl;
            i = 0;
        } else {
            ifstart.close();
        }

        // Wait for 1 sec and check file existence again
        sleep(1);
    }

    return 0;
}
