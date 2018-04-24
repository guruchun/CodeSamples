#include "StringUtil.h"
#include "ConfigManager.h"
#include "McpException.h"

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

std::string StringUtil::convertTagName(const std::string& oldName, std::vector<std::string> swapNames) {
    string newName = oldName;
    for (auto r : swapNames) {
        newName = StringUtil::convertTagName(newName, r);
    }
    return newName;
}

std::string StringUtil::convertTagName(const std::string& oldname, std::string swapName)
{
	std::string name = oldname;

	std::string keyLocal = "%L";
	std::string keyPosition = "%P";
	std::string keyMaster = "%M";
	std::string keySwap = "%N";

	ConfigManager& cfgmgr = ConfigManager::getInstance();

	std::string localName = cfgmgr.getPcnName();
	std::string positionName = cfgmgr.getPcnPosition();
	std::string masterName = cfgmgr.getPcnMaster();

	auto pos = name.find(keyLocal);
	while(pos != name.npos) {
		name.replace(pos, keyLocal.length(), localName);
		pos = name.find(keyLocal);
	}

	pos = name.find(keyPosition);
	while(pos != name.npos) {
		name.replace(pos, keyPosition.length(), positionName);
		pos = name.find(keyPosition);
	}

	pos = name.find(keyMaster);
	while(pos != name.npos) {
		name.replace(pos, keyMaster.length(), masterName);
		pos = name.find(keyMaster);
	}

	if(!swapName.empty()){
		pos = name.find(keySwap);
		// replace only first matching "%N"
		if(pos != name.npos) {
			name.replace(pos, keySwap.length(), swapName);
			pos = name.find(keySwap);
		}
	}

	return name;
}

/// @brief Tag 이름으로 부터 Node 정보를 축출한다.( "{mpms}/pms/xx.f32" => "{mpms}" )
/// @param tag 태그 명
/// @return 노드 이름
/// @detail return 값에 대한 empty() 확인 후 처리할 것
// __PRETTY_FUNCTION__ 사용시 메시지 출력 안됨( 원인 불명 ) => __func__ 로 사용
std::string StringUtil::findPcnName(const std::string& tag)
{
	auto foundPos = tag.find("/");
	if ( foundPos == string::npos ) {
	    //throw McpException( "can't find node name, invalid tag=\"" + tag + "\"", McpError::InvalidValue );
		return "";
	} else {
		return tag.substr(0, foundPos); // index 0 ~ size
	}
}

RedisDataType StringUtil::GetRedisType(const string& tag) {

    if (tag.empty()) {
        throw MCP_EXCEPTION("tag is invalid, empty, " + tag, McpError::InvalidValue);
    }
    size_t pos = tag.rfind(".");
    if (pos == string::npos) {
        throw MCP_EXCEPTION("no data type in tag, " + tag, McpError::InvalidValue);
    }
    if (tag.length() < pos + 4) {
        throw MCP_EXCEPTION("invalid tag specifier size, " + tag, McpError::InvalidValue);
    }

    switch (tag.at(pos+1)) {
    case 'b':
        return RedisDataType::B01;
    case 'u':
        switch (tag.at(pos+2)) {
        case '0':
            return RedisDataType::U08;
        case '1':
            return RedisDataType::U16;
        case '3':
            return RedisDataType::U32;
        case '6':
            return RedisDataType::U64;
        default:
            throw MCP_EXCEPTION("invalid tag type, " + tag, McpError::InvalidValue);
        }
        break;
    case 'i':
        switch (tag.at(pos+2)) {
        case '0':
            return RedisDataType::I08;
        case '1':
            return RedisDataType::I16;
        case '3':
            return RedisDataType::I32;
        case '6':
            return RedisDataType::I64;
        default:
            throw MCP_EXCEPTION("invalid tag type, " + tag, McpError::InvalidValue);
        }
        break;
    case 'f':
        switch (tag.at(pos+2)) {
        case '3':
            return RedisDataType::F32;
        case '6':
            return RedisDataType::F64;
        default:
            throw MCP_EXCEPTION("invalid tag type, " + tag, McpError::InvalidValue);
        }
        break;
    case 's':
        return RedisDataType::STR;
        break;
    default:
        throw MCP_EXCEPTION("invalid tag type specifier, " + tag, McpError::InvalidValue);
    }

    return RedisDataType::STR;
}

double StringUtil::Bin2Double(const string& tag, const BinString& value) {
    RedisDataType redisType = StringUtil::GetRedisType(tag);
    return StringUtil::Bin2Double(redisType, value);
}

double StringUtil::Bin2Double(RedisDataType redisType, const BinString& value) {
    double realValue = 0.0;

    if (value.getSize() == 1) {
        if (redisType == RedisDataType::B01) {
            uint8_t flag = (uint8_t)value;
            realValue = (flag == 0) ? 0 : 1;
        } else if (redisType == RedisDataType::U08) {
            realValue = (uint8_t)value;
        } else if (redisType == RedisDataType::I08) {
            realValue = (int8_t)value;
        } else {
            stringstream ss;
            ss << "invalid redis type " << redisType;
            throw MCP_EXCEPTION(ss.str(), McpError::InvalidValue);
        }
    } else if (value.getSize() == 2) {
        if (redisType == RedisDataType::U16) {
            realValue = (uint16_t)value;
        } else if (redisType == RedisDataType::I16) {
            realValue = (int16_t)value;
        } else {
            stringstream ss;
            ss << "invalid redis type " << redisType;
            throw MCP_EXCEPTION(ss.str(), McpError::InvalidValue);
        }
    } else if (value.getSize() == 4) {
        if (redisType == RedisDataType::I32) {
            realValue = (int32_t)value;
        } else if (redisType == RedisDataType::U32) {
            realValue = (uint32_t)value;
        } else if (redisType == RedisDataType::F32) {
            realValue = (float)value;
        } else {
            stringstream ss;
            ss << "invalid redis type " << redisType;
            throw MCP_EXCEPTION(ss.str(), McpError::InvalidValue);
        }
    } else if (value.getSize() == 8) {
        if (redisType == RedisDataType::I64) {
            realValue = (int64_t)value;
        } else if (redisType == RedisDataType::U64) {
            realValue = (uint64_t)value;
        } else if (redisType == RedisDataType::F64) {
            realValue = (double)value;
        } else {
            stringstream ss;
            ss << "invalid redis type " << redisType;
            throw MCP_EXCEPTION(ss.str(), McpError::InvalidValue);
        }
    } else {
        stringstream ss;
        ss << "invalid data size, redis type=" << redisType << ", size=" << value.getSize() ;
        throw MCP_EXCEPTION(ss.str(), McpError::InvalidValue);
    }
    return realValue;
}

BinString StringUtil::Double2Bin(const string& tag, double value) {
    RedisDataType redisType = StringUtil::GetRedisType(tag);
    return StringUtil::Double2Bin(redisType, value);
}

BinString StringUtil::Double2Bin(RedisDataType redisType, double value) {

    if (redisType == RedisDataType::B01) {
        return BinString(static_cast<uint8_t>(round(value)));
    }

    if (redisType == RedisDataType::U08) {
        return BinString(static_cast<uint8_t>(round(value)));
    }

    if (redisType == RedisDataType::I08) {
        return BinString(static_cast<int8_t>(round(value)));
    }

    if (redisType == RedisDataType::U16) {
        return BinString(static_cast<uint16_t>(round(value)));
    }

    if (redisType == RedisDataType::I16) {
        return BinString(static_cast<int16_t>(round(value)));
    }

    if (redisType == RedisDataType::I32) {
        return BinString(static_cast<int32_t>(round(value)));
    }

    if (redisType == RedisDataType::U32) {
        return BinString(static_cast<uint32_t>(round(value)));
    }

    if (redisType == RedisDataType::F32) {
        return BinString((float)value);
    }

    if (redisType == RedisDataType::I64) {
        return BinString(static_cast<int64_t>(round(value)));
    }

    if (redisType == RedisDataType::U64) {
        return BinString(static_cast<uint64_t>(round(value)));
    }

    if (redisType == RedisDataType::F64) {
        return BinString(value);
    }

    return BinString();
}

/// @brief tag type에 따라 BinSting을 string으로 변환한다
/// @param tag tag name
/// @param value BinString 형태의 tag value
/// @return value를 string을 변환한 값
string StringUtil::Bin2String(const string& tag, const BinString& value) {

	RedisDataType type;
	try {
		type = StringUtil::GetRedisType(tag);
	} catch (exception& e1) {
		return "";
	}

	if (value.getSize() == 0) {
	    return "";
	}

	// float number conversion 용도
	stringstream ss;

    // return string
	string str;
	switch (type) {
	case RedisDataType::B01:
		str =  to_string((uint8_t)value == 0 ? 0 : 1);
		break;
	case RedisDataType::I08:
		str =  to_string((char) value);
		break;
	case RedisDataType::U08:
		str =  to_string((unsigned int) value);
		break;
	case RedisDataType::I16:
		str =  to_string((int16_t) value);
		break;
	case RedisDataType::U16:
		str =  to_string((uint16_t) value);
		break;
	case RedisDataType::I32:
		str =  to_string((int32_t) value);
		break;
	case RedisDataType::U32:
		str =  to_string((uint32_t) value);
		break;
	case RedisDataType::I64:
		str =  to_string((int64_t) value);
		break;
	case RedisDataType::U64:
		str =  to_string((uint64_t) value);
		break;
	case RedisDataType::F32:
	    ss.setf( ios::fixed );
	    ss.precision(4);
	    ss << (float) value;
		str =  ss.str();
		break;
	case RedisDataType::F64:
	    ss.setf( ios::fixed );
	    ss.precision(4);
        ss << (float) value;
        str =  ss.str();
		break;
	case RedisDataType::STR:
		str =  value.toString();
		break;
	default:
		break;
	}

	return str;
}

string StringUtil::Bin2StringDesc(const string& tag, const BinString& value, bool showType) {

    RedisDataType type;
    try {
        type = StringUtil::GetRedisType(tag);
    } catch (exception& e1) {
        return "";
    }

    // float number conversion 용도
    stringstream ss;

    // return string
    string str;

    switch (type) {
    case RedisDataType::B01:
        str =  (showType ? "(bool) ": "") + string(((uint8_t)value == 0 ? "false" : "true"));
        break;
    case RedisDataType::I08:
        str =  (showType ? "(int8) ": "") + to_string((char) value);
        break;
    case RedisDataType::U08:
        str =  (showType ? "(uint8) ": "") + to_string((unsigned int) value);
        break;
    case RedisDataType::I16:
        str =  (showType ? "(bool) ": "") + to_string((int16_t) value);
        break;
    case RedisDataType::U16:
        str =  (showType ? "(bool) ": "") + to_string((uint16_t) value);
        break;
    case RedisDataType::I32:
        str =  (showType ? "(int32) ": "") + to_string((int32_t) value);
        break;
    case RedisDataType::U32:
        str =  (showType ? "(uint32) ": "") + to_string((uint32_t) value);
        break;
    case RedisDataType::I64:
        str =  (showType ? "(int64) ": "") + to_string((int64_t) value);
        break;
    case RedisDataType::U64:
        str =  (showType ? "(uint64) ": "") + to_string((uint64_t) value);
        break;
    case RedisDataType::F32:
        //ss.precision(8);
        //str =  (showType ? "(float) ": "") + to_string(round( (float) value ));
        ss.setf( ios::fixed );
        ss.precision(4);
        ss << (float) value;
        str =  (showType ? "(float) ": "") + ss.str();
        break;
    case RedisDataType::F64:
        //ss.precision(17);
        //str =  (showType ? "(double) ": "") + to_string((double) value);
        ss.setf( ios::fixed );
        ss.precision(4);
        ss << (float) value;
        str =  (showType ? "(float) ": "") + ss.str();
        break;
    case RedisDataType::STR:
        str =  (showType ? "(string) ": "") + value.toString();
        break;
    default:
        break;
    }

    return str;
}

BinString StringUtil::String2Bin(const std::string& tag, const string& value) {

    RedisDataType type;
    try {
        type = StringUtil::GetRedisType(tag);
    } catch (exception& e1) {
        throw MCP_EXCEPTION("redis type invalid, " + tag, McpError::InvalidValue);
    }

    BinString bstr;
    try {
        switch (type) {
        case RedisDataType::B01:
            if (boost::iequals(value, "true") || value != "0")
                bstr =  BinString((uint8_t)1);
            else
                bstr =  BinString((uint8_t)0);
            break;
        case RedisDataType::I08:
            bstr =  BinString(boost::lexical_cast<int8_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::U08:
            bstr =  BinString(boost::lexical_cast<uint16_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::I16:
            bstr =  BinString(boost::lexical_cast<int16_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::U16:
            bstr =  BinString(boost::lexical_cast<uint16_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::I32:
            bstr =  BinString(boost::lexical_cast<int32_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::U32:
            bstr =  BinString(boost::lexical_cast<uint32_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::I64:
            bstr =  BinString(boost::lexical_cast<int64_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::U64:
            bstr =  BinString(boost::lexical_cast<uint64_t>(value.c_str(), value.length()));
            break;
        case RedisDataType::F32:
            bstr =  BinString(boost::lexical_cast<float>(value.c_str(), value.length()));
            break;
        case RedisDataType::F64:
            bstr =  BinString(boost::lexical_cast<double>(value.c_str(), value.length()));
            break;
        case RedisDataType::STR:
            bstr =  BinString(value.c_str(), value.length());
            break;
        default:
            break;
        }
    } catch (exception& e1) {
        throw MCP_EXCEPTION("tag value invalid, " + value, McpError::InvalidValue);
    }

    return bstr;
}


/// bool to string
string StringUtil::Bool2String(const bool value) {
    return value ? "true" : "false";
}

/// string to bool
bool StringUtil::String2Bool(const string& value) {
    return boost::algorithm::iequals(value, "true") ? true : false;
}

/// return basename ( "target/bin/lgcns_sample_svc" => "lgcns_sample_svc" )
string StringUtil::basename( const char* param ) {
    const char *basename = strrchr(param, '/');
    if ( basename == nullptr ) {
        return string(param);
    } else {
        return string(++basename);
    }
}
