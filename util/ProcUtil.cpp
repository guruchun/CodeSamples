#include "ProcUtil.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>

using namespace std;

string ProcUtil::execCommand(const string& execCmd, const string params) {
    string command = execCmd;
    if (!params.empty()) {
        command += " ";
        command += params;
    }

    shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen : incorrect parameters or too many files. exec=" + command);
    }

    stringstream ss;
    char buffer[128];
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            ss << buffer;
    }
    return ss.str();
}
