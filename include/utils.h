#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <array>
//for file_exists
#include <sys/stat.h>
//for trim
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
//for split
// #include <iostream>
// #include <string>
#include <sstream>
// #include <algorithm>
#include <iterator>

using namespace std;

std::string get_prefix() {
    return to_string(time(nullptr)) + "\t";
}

//from http://stackoverflow.com/a/478960/3136474
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

//from http://stackoverflow.com/a/12774387/3136474
inline bool file_exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

//from http://stackoverflow.com/a/217605/3136474
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

//adapted from http://stackoverflow.com/a/236803/3136474
template<typename Out>
void splitNl(const std::string &s, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item)) {
        *(result++) = item;
    }
}
std::vector<std::string> splitNl(const std::string &s) {
    std::vector<std::string> elems;
    splitNl(s, std::back_inserter(elems));
    return elems;
}

#endif