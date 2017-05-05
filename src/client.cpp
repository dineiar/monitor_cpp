#include <unistd.h>
#include <fstream>
#include <utils.h>

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " FOLDER FOLDER_NFS" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    std::string folder_nfs = argv[2];
    //from http://stackoverflow.com/a/18027897/3136474
    if (!folder.empty() && folder.back() != '/')
        folder += '/'; //add trailing slash if not present
    if (!folder_nfs.empty() && folder_nfs.back() != '/')
        folder_nfs += '/'; //add trailing slash if not present
    std::string folder_out = folder + ".out/";

    std::cout << "NFS mounted on " << folder_nfs << std::endl;
    std::cout << "Creating folder for output on " << folder_out << std::endl;
    mkdir(folder_out.c_str(), 7777);
    std::cout << "Monitoring files on " << folder << "..." << std::endl;

    int i = 0;
    while (true) {
        i++;
        std::cout << to_string(i) << " iteration on folder monitoring" << std::endl;

        std::string cmd = "ls " + folder;
        std::string ls_output = exec(cmd.c_str());
        trim(ls_output);
        if (!ls_output.empty()) {
            std::vector<std::string> files = splitNl(ls_output);
            std::cout << files.size() << " file(s) found" << std::endl;

            for(auto const& file : files) {
                // Read command from file
                std::string full_filename = folder + file;
                ifstream ifstart(full_filename);
                std::string command( (std::istreambuf_iterator<char>(ifstart) ),
                                    (std::istreambuf_iterator<char>()) );
                ifstart.close();
                trim(command);
                
                std::cout << "Starting " << file << " command: '" << command << "'" << std::endl;

                remove(full_filename.c_str()); //Delete file

                // Create start file for server-side script
                std::string filename = folder_nfs + "start";
                std::ofstream fstream;
                fstream.open(filename);
                fstream << file; //the name of the file is the name of the folder created with output
                fstream.close(); //flushes
                std::cout << "Monitoring triggered" << std::endl;
                // Waits for server to start monitoring
                sleep(1); //== sleep time on file check on server.cpp

                // Runs command
                std::string cmd_output = exec(command.c_str());

                std::cout << "Command executed, stopping monitoring..." << std::endl;
                // Create stop file for server-side script
                filename = folder_nfs + "stop";
                fstream.open(filename);
                fstream << "\n"; //this file is not readed
                fstream.close(); //flushes

                sleep(3 * 5); //== 3 x sleep time on monitoring loop on server.cpp

                std::string cmd = "mv " + folder_nfs + file + " " + folder_out;
                std::cout << "Getting output... " << cmd << std::endl;
                exec(cmd.c_str());

                // Writes command output as well
                filename = folder_out + file + "/client_output.txt";
                fstream.open(filename);
                fstream << cmd_output;
                fstream.close();

                std::cout << file << " done. Output on " << folder_out << file << std::endl;
            }

            i = 0; // Reset seconds counter
        }

        sleep(1);
    }
}