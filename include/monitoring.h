#ifndef MONITORING_H_INCLUDED
#define MONITORING_H_INCLUDED

#include <string>
#include <monitor.h> //also includes utils.h

using namespace std;

class Monitoring {
    public:
        bool redis, riak;
        Monitor mon_iostat, mon_free, mon_df, mon_redis, mon_riak;

        //Init system monitoring
        Monitoring();
        Monitoring(std::string out_folder);
        
        void init(std::string out_folder);
        void detect();
        void close();
};

Monitoring::Monitoring() { };
Monitoring::Monitoring(std::string out_folder) {
    redis = false;
    riak = false;
    mon_iostat.init(out_folder + "iostat.txt");
    mon_free.init(out_folder + "free.txt");
    mon_df.init(out_folder + "df.txt");
    
    detect();
    if (redis) {
        mon_redis.init(out_folder + "redis.txt");
    }
    if (riak) {
        mon_riak.init(out_folder + "riak.txt");
    }
};

//Detect running databases
void Monitoring::detect() {
    //Redis
    redis = false;
    std::string cmd = "redis-cli ping";
    std::string expected = "PONG\n";
    if (exec(cmd.c_str()) == expected) {
        redis = true;
    }

    //Riak
    riak = false;
    cmd = "sudo riak ping";
    expected = "pong\n";
    if (exec(cmd.c_str()) == expected) {
        riak = true;
    }
};

//Free resources
void Monitoring::close() {
    mon_iostat.close();
    mon_free.close();
    mon_df.close();
    mon_redis.close();
    mon_riak.close();
}

#endif