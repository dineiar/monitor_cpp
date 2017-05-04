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
        cerr << "Usage: " << argv[0] << " FOLDER" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    std::string file_start = folder + "start";
    std::string file_stop = folder + "stop";
    std::cout << "Waiting for " << file_start << " to show up..." << std::endl;

    int i = 0;
    while (true) {
        i++;
        std::cout << "[DEBUG] " << to_string(i) << " iteration on file_start check" << std::endl;
        ifstream ifstart(file_start);
        if (ifstart.good()) {
            std::cout << "Start file detected, will monitor until " << file_stop << " show up." << std::endl;
            
            //from http://stackoverflow.com/a/2912614/3136474
            std::string folder_out( (std::istreambuf_iterator<char>(ifstart) ),
                                (std::istreambuf_iterator<char>()) );
            //start file must have the folder name for recording output
            trim(folder_out);
            folder_out = folder + folder_out;
            ifstart.close();
            remove(file_start.c_str());
            mkdir(folder_out.c_str(), 7777);

            Monitoring monitor(folder_out);

            std::cout << "Redis " << (monitor.redis ? "online" : "offline") << std::endl;
            std::cout << "Riak " << (monitor.riak ? "online" : "offline") << std::endl;
            std::cout << "Started monitoring, output goes on " << folder_out << "..." << std::endl;
            
            std::string cmd = "";

            int c = 0;
            while (true) {
                c++;
                std::cout << "[DEBUG] " << to_string(c) << " iteration on monitoring" << std::endl;
                //performance de disco (I/O)
                //CPU (geral)
                //Memória
                //Detectar SWAP 

                //Get CPU and disk usage through iostat
                cmd = "iostat -c -d -m -x -y";
                //avg-cpu:  %user   %nice %system %iowait  %steal   %idle
                //Device:  rrqm/s  wrqm/s  r/s  w/s  rMB/s  wMB/s  avgrq-sz  avgqu-sz  await  r_await  w_await  svctm  %util
                monitor.mon_iostat.writeCmdResult(cmd, get_prefix());

                //Get memory usage through free
                cmd = "free -m -t";
                // total  used  free  shared  buff/cache  available
                // Mem, Swap, Total
                monitor.mon_free.writeCmdResult(cmd, get_prefix());

                //Get disk usage through df
                cmd = "df -T -l";
                // Filesystem  Type  Size  Used  Avail  Use%  Mounted on
                monitor.mon_df.writeCmdResult(cmd, get_prefix());

                //Get Redis info
                if (monitor.redis) {
                    cmd = "redis-cli info all";
                    //https://redis.io/commands/info
                    monitor.mon_redis.writeCmdResult(cmd, get_prefix());
                }

                //Get Riak info
                if (monitor.riak) {
                    cmd = "sudo riak-admin status";
                    //https://redis.io/commands/info
                    monitor.mon_riak.writeCmdResult(cmd, get_prefix());
                }

                sleep(5);

                if (file_exists(file_stop)) {
                    remove(file_stop.c_str());
                    std::cout << "Stop file detected, stopping monitoring." << std::endl;
                    break;
                }
            }

            monitor.close();
            std::cout << "Monitoring stopped. Waiting for " << file_start << " to show up again..." << std::endl;
            i = 0;
        } else {
            ifstart.close();
        }

        // Wait for 1 sec and check file existence again
        sleep(1);
    }

    return 0;
}
