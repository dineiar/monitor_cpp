#include <unistd.h>
#include <fstream>
#include <utils.h>

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " FOLDER FOLDER_NFS [ITERATIONS]" << std::endl;
        return 1;
    }

    std::string folder = argv[1];
    std::string folder_nfs = argv[2];
    int iterations = 1;
    if (argc > 3) {
        istringstream ss(argv[3]);
        if (!(ss >> iterations)) {
            std::cout << "Invalid iterations number " << argv[3] << ". Ignoring, will run 1 iteration." << std::endl;
            iterations = 1; // Better make sure
        } else {
            std::cout << "Will run " << to_string(iterations) << " iterations" << std::endl;
        }
    }
    //from http://stackoverflow.com/a/18027897/3136474
    if (!folder.empty() && folder.back() != '/')
        folder += '/'; //add trailing slash if not present
    if (!folder_nfs.empty() && folder_nfs.back() != '/')
        folder_nfs += '/'; //add trailing slash if not present
    std::string folder_out = folder + ".out/";
    std::string file_cmd = folder_nfs + "exec";
    std::string file_start = folder_nfs + "start";
    std::string file_stop = folder_nfs + "stop";
    std::ofstream fstream;

    std::cout << "NFS mounted on " << folder_nfs << std::endl;
    std::cout << "Creating folder for output on " << folder_out << std::endl;
    mkdir(folder_out.c_str(), 7777);
    std::cout << "Checking files on " << folder << std::endl;

    std::string auxCmd = "ls " + folder;
    std::string ls_output = exec(auxCmd.c_str());
    trim(ls_output);
    if (!ls_output.empty()) {
        std::vector<std::string> files = splitNl(ls_output);
        std::cout << files.size() << " file(s) found" << std::endl;

        for(auto const& file : files) {
            // Read commands from file
            std::string full_filename = folder + file;
            ifstream ifstart(full_filename);
            std::string command( (std::istreambuf_iterator<char>(ifstart) ),
                                (std::istreambuf_iterator<char>()) );
            trim(command);
            ifstart.close();
            remove(full_filename.c_str()); //Delete file
            std::vector<std::string> commands = splitNl(command);
            std::cout << commands.size() << " commands found in file " << file << std::endl;

            // Read pre-benchmark commands from file
            full_filename = folder + ".pre" + file;
            ifstream ifpre(full_filename);
            std::vector<std::string> precommands;
            if (ifpre.good()) {
                std::string tmpcommand( (std::istreambuf_iterator<char>(ifpre) ),
                                    (std::istreambuf_iterator<char>()) );
                std::string precommand = tmpcommand;
                trim(precommand);

                if (!precommand.empty()) {
                    precommands = splitNl(precommand);
                    std::cout << precommands.size() << " pre-benchmark commands found" << std::endl;
                }
            } else {
                std::cout << "No pre command detected in " << full_filename << std::endl;
            }
            ifpre.close();
            remove(full_filename.c_str()); //Delete file

            // Read clear commands from file
            full_filename = folder + ".clear" + file;
            ifstream ifclear(full_filename);
            std::vector<std::string> clearcommands;
            if (ifclear.good()) {
                std::string tmpcommand( (std::istreambuf_iterator<char>(ifclear) ),
                                    (std::istreambuf_iterator<char>()) );
                std::string clearcommand = tmpcommand;
                trim(clearcommand);

                if (!clearcommand.empty()) {
                    clearcommands = splitNl(clearcommand);
                    std::cout << clearcommands.size() << " clear commands found" << std::endl;
                }
            } else {
                std::cout << "No clear command detected in " << full_filename << std::endl;
            }
            ifclear.close();
            remove(full_filename.c_str()); //Delete file
            
            std::cout << "Starting " << file << " commands:" << std::endl;

            for (int iter = 1; iter <= iterations; iter++) {
                std::string run_id = file; //the name of the file is the name of the folder created with output
                if (iterations > 1) {
                    run_id += "_" + to_string(iter);
                }

                if (precommands.size() > 0) {
                    std::cout << "Executing pre-benchmark commands" << std::endl;
                    for(auto const& precmd : precommands) {
                        std::cout << "Executing '" << precmd << "'" << std::endl;

                        // Pre commands are executed client side
                        std::string cmd_output = exec(precmd.c_str());
                        std::cout << "Output: " << cmd_output << std::endl;

                        // // Tells server to execute this command
                        // fstream.open(file_cmd);
                        // fstream << precmd;
                        // fstream.close(); //flushes

                        // int out_wait = 0;
                        // while (true) {
                        //     out_wait++;
                            
                        //     // Don't notice new files in NFS folder if we don't touch the folder explicitly
                        //     std::string cmdTmp = "ls " + folder_nfs;
                        //     exec(cmdTmp.c_str());
                            
                        //     std::string file_output = folder_nfs + "output";
                        //     std::ifstream ifcmdoutput(file_output);
                        //     if (ifcmdoutput.good()) {
                        //         std::string preoutput( (std::istreambuf_iterator<char>(ifcmdoutput) ),
                        //                             (std::istreambuf_iterator<char>()) );
                        //         std::cout << "Got output: " << preoutput << std::endl;
                        //         ifcmdoutput.close();
                        //         remove(file_output.c_str());

                        //         break;
                        //     } else {
                        //         ifcmdoutput.close();
                        //     }

                        //     if (out_wait >= 600) {
                        //         std::cout << "10 minutes and no output so far. Going to next command." << std::endl;
                        //         break;
                        //     }
                        //     sleep(1);
                        // }
                    }
                }

                //Monitor all iterations again
                // if (iter == 1) {
                    // Create start file for server-side script
                    fstream.open(file_start);
                    fstream << run_id;
                    fstream.close(); //flushes
                    std::cout << "Monitoring triggered" << std::endl;
                    // Waits for server to start monitoring
                    sleep(1); //== sleep time on file check on server.cpp
                // }

                // Run commands
                std::string cmds_all_output = "";
                for(auto const& cmdFile : commands) {
                    std::string cmd_output = exec(cmdFile.c_str());
                    cmds_all_output += "\n\n" + cmd_output;
                }

                // if (iter == 1) {
                    std::cout << "Command executed, stopping monitoring..." << std::endl;
                    // Create stop file for server-side script
                    fstream.open(file_stop);
                    fstream << "\n"; //this file is not readed
                    fstream.close(); //flushes
                // }

                sleep(3 * 3); //== 3 x sleep time on monitoring loop on server.cpp

                std::string cmd = "mv " + folder_nfs + run_id + " " + folder_out;
                std::cout << "Getting output... " << cmd << std::endl;
                exec(cmd.c_str());

                // Writes command output as well
                std::string filename = folder_out + file + "_client_output.txt";
                // appends all output on a single file
                fstream.open(filename, std::ofstream::out | std::ofstream::app);
                fstream << cmds_all_output;
                fstream.close();

                if (iterations > 1) {
                    std::cout << "Iteration " << to_string(iter) << " of ";
                }
                std::cout << file << " done. Output on " << folder_out << run_id << std::endl;

                if (clearcommands.size() > 0) {
                    std::cout << "Executing clear commands" << std::endl;
                    for(auto const& clearcmd : clearcommands) {
                        std::cout << "Executing '" << clearcmd << "'" << std::endl;

                        // Tells server to execute this command
                        fstream.open(file_cmd);
                        fstream << clearcmd;
                        fstream.close(); //flushes

                        int out_wait = 0;
                        while (true) {
                            out_wait++;
                            
                            // Don't notice new files in NFS folder if we don't touch the folder explicitly
                            std::string cmdTmp = "ls " + folder_nfs;
                            exec(cmdTmp.c_str());
                            
                            std::string file_output = folder_nfs + "output";
                            std::ifstream ifcmdoutput(file_output);
                            if (ifcmdoutput.good()) {
                                std::string clearoutput( (std::istreambuf_iterator<char>(ifcmdoutput) ),
                                                    (std::istreambuf_iterator<char>()) );
                                std::cout << "Got output: " << clearoutput << std::endl;
                                ifcmdoutput.close();
                                remove(file_output.c_str());

                                break;
                            } else {
                                ifcmdoutput.close();
                            }

                            if (out_wait >= 600) {
                                std::cout << "10 minutes and no output so far. Going to next command." << std::endl;
                                break;
                            }
                            sleep(1);
                        }
                    }
                }
            } //for iterations loop
        }
    } else {
        std::cout << "No files on " << folder << ". Stopping." << std::endl;
    }

}