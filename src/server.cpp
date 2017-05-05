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
    std::string file_start = folder + "start";
    std::string file_stop = folder + "stop";
    std::cout << "Waiting for " << file_start << " to show up..." << std::endl;

    int i = 0;
    while (true) {
        i++;
        std::cout << to_string(i) << " seconds waiting for start file" << std::endl;
        ifstream ifstart(file_start);
        if (ifstart.good()) {
            std::cout << "Start file detected, will monitor until " << file_stop << " show up." << std::endl;
            
            //from http://stackoverflow.com/a/2912614/3136474
            std::string folder_out( (std::istreambuf_iterator<char>(ifstart) ),
                                (std::istreambuf_iterator<char>()) );
            //start file must have the folder name for recording output
            trim(folder_out);
            if (!folder_out.empty() && folder_out.back() != '/')
                folder_out += '/'; //add trailing slash if not present
            folder_out = folder + folder_out;
            ifstart.close();
            remove(file_start.c_str());
            mkdir(folder_out.c_str(), 7777);

            Monitoring monitor(folder_out);

            std::cout << "Redis " << (monitor.redis ? "online" : "offline") << std::endl;
            std::cout << "Riak " << (monitor.riak ? "online" : "offline") << std::endl;
            std::cout << "Started monitoring, output goes on " << folder_out << "..." << std::endl;
            
            int c = 0;
            while (true) {
                c++;
                std::cout << to_string(c) << " monitoring iteration" << std::endl;
                //disk performance (I/O)
                //CPU (general)
                //Memory usage
                //SWAP detection

                //Get CPU and disk usage through iostat
                //avg-cpu:  %user   %nice %system %iowait  %steal   %idle
                //Device:  rrqm/s  wrqm/s  r/s  w/s  rMB/s  wMB/s  avgrq-sz  avgqu-sz  await  r_await  w_await  svctm  %util
                monitor.mon_iostat.read_status();

                //Get memory usage through free
                // total  used  free  shared  buff/cache  available
                // Mem, Swap, Total
                monitor.mon_free.read_status();

                //Get disk usage through df
                // Filesystem  Type  Size  Used  Avail  Use%  Mounted on
                monitor.mon_df.read_status();

                //Get Redis info
                if (monitor.redis) {
                    //https://redis.io/commands/info
                    monitor.mon_redis.read_status();
                }

                //Get Riak info
                if (monitor.riak) {
                    //http://docs.basho.com/riak/kv/2.2.3/using/reference/statistics-monitoring/
                    monitor.mon_riak.read_status();
                }

                monitor.flush(); //flush all to disk

                sleep(5); //@TODO 5 secs?

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
