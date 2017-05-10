#include <unistd.h>
#include <fstream>
#include <utils.h>

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
    // std::string file_cmd = folder + "exec";
    std::string file_start = folder + "start";
    std::string file_stop = folder + "stop";
    
    std::cout << "Creating file on " << file_start << std::endl;

    // Create start file for server-side script
    std::ofstream fstream;
    fstream.open(file_start);
    fstream << "file" << std::endl; //the name of the file is the name of the folder created with output
    fstream.flush();
    fstream.close();
    std::cout << "File created" << std::endl;
    // Waits for server to start monitoring
    sleep(1); //== sleep time on file check on server.cpp

    sleep(10);

    fstream.open(file_stop);
    fstream << "\n" << std::endl; //this file is not readed
    fstream.close(); //flushes

    sleep(1);
    
}