#include "StringUtil.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <cstring>

// 2016-09-07 jicho 2자리씩 표시되도록 변경
string StringUtil::toHexString(const unsigned char* data, int len, bool msbFirst) {
    stringstream ss;
    ss << hex;
    if (msbFirst) {
        for(int i=len-1; i>=0; --i)
            ss << setfill ('0') << setw(2)  << (int)data[i] << " ";
    } else {
        for(int i=0; i<len; ++i)
            ss << setfill ('0') << setw(2)  << (int)data[i] << " ";
    }

    string s = ss.str();
    return s.erase(s.find_last_not_of(" \n\r\t")+1);
}

string StringUtil::Char2Hex(const unsigned char data) {
    stringstream ss;
    ss << hex << setfill ('0') << setw(2)  << (int)data;

    return ss.str();
}

unsigned char StringUtil::Hex2Char(const string& data) {
//    cout << "hex2chr " << data << endl;
    try {
        int c = stoi(data, nullptr, 16);
        return (unsigned char)c;
    } catch (exception& e) {
        cout << "hex2chr error! " << e.what();
        return 0;
    }
}

bool StringUtil::IsNumber(const string& str) {
    return !str.empty() &&
            str.find_first_not_of("0123456789") == string::npos;
}

string StringUtil::Trim(string& s) {
    // ltrim
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));

    // rtrim
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

string StringUtil::map2String(map<string, string>& kv) {
	stringstream ss;

	for (const auto& m : kv) {
	    ss << m.first << "=" << m.second << ",";
	}

	string s = ss.str();
	if (s.length() > 1)
		s.erase(s.length()-1);	// remove last ','

	return s;
}

void StringUtil::string2Map(const string& input, map<string, string>& output) {

	vector<string> kvList;
	boost::split(kvList, input, boost::is_any_of(", "),boost::algorithm::token_compress_on);
	for (const string& kv : kvList) {
		vector<string> kvi;
		boost::split(kvi, kv, boost::is_any_of("= "),boost::algorithm::token_compress_on);
		output.insert(make_pair(kvi[0], kvi[1]));
	}
}

string StringUtil::vectorToString(vector<string>& v, const string& delimeter) {

	stringstream ss;
	for (const string& s : v) {
		ss << s << delimeter;
	}

	string s = ss.str();
	if (s.length() > 1)
		s.erase(s.length()-1);	// remove last ','

	return s;
}

// delimeter like ", "
vector<string> StringUtil::vectorFromString(const string& input, const string& delimeter) {
    vector<string> output;
	boost::split(output, input, boost::is_any_of(delimeter),boost::algorithm::token_compress_on);
	return output;
}

string  StringUtil::toLower(const string& str) {
    string instr = str;
    boost::algorithm::to_lower(instr);
    return instr;
}

string StringUtil::getStringFromFile(const string& path) {

	stringstream ss;
	string line;
	ifstream inStream(path);

	if (!inStream.is_open()) {
		return "";
	}

	while(!inStream.eof()){
		getline(inStream, line);
		ss << line;
	}
	inStream.close();

	string result = ss.str();
	return Trim(result);
}

vector<string> StringUtil::getStringsFromFile(const std::string& tagFile) {
    vector<string> lines;
    try {
        string line;
        // open file
        ifstream infile(tagFile);
        while (std::getline(infile, line)) {
            // get one line, comment line or inline comment are ignored
            boost::trim(line);
            if (line.empty() || line.at(0) == '#')
                continue;
            size_t pos = line.find_first_of('#');
            if (pos != string::npos) {
                line = line.substr(0, pos);
                boost::trim(line);
            }
            lines.push_back(line);
        }
    } catch (std::exception& e) {
        cout << "error, loading tag list from file: " << tagFile << ", " << e.what();
        throw e;
    }

    return lines;
}

/// bool to string
string StringUtil::Bool2String(const bool value) {
    return value ? "true" : "false";
}

/// string to bool
bool StringUtil::String2Bool(const string& value) {
    return boost::algorithm::iequals(value, "true") ? true : false;
}
