#include "TimeUtil.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

/// get time stamp as string like "2016-07-27 20:55:38.789"
/// @link http://man7.org/linux/man-pages/man3/strftime.3.html
string TimeUtil::TPoint2String(const chrono::system_clock::time_point& timepoint, const string& fmtstr, bool fmtMs) {
	time_t epoch_time = chrono::system_clock::to_time_t(timepoint);

	char buf[128];
	strftime(buf, sizeof(buf), fmtstr.c_str(), localtime(&epoch_time));

	auto duration = timepoint.time_since_epoch();
	auto millis = chrono::duration_cast<chrono::milliseconds>(
			duration).count() % 1000;

	stringstream ss;
	ss << buf;
	if (fmtMs) {
	    ss << "." << setfill('0') << setw(3) << to_string(millis);
	}
	return ss.str();
}

/// convert total milliseconds(uint64_t) of time to string
string TimeUtil::Milli2String(const uint64_t totalMillis) {

	auto timepoint = TimeUtil::Milli2TPoint(totalMillis);
	return TimeUtil::TPoint2String(timepoint);
}

/// convert boost:posix_time to string
/// http://www.boost.org/doc/libs/1_65_1/doc/html/date_time/date_time_io.html#date_time.format_flags
string TimeUtil::PTime2String(const boost::posix_time::ptime timestamp, const string& fmtstr) {
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
	facet->format(fmtstr.c_str());

	stringstream stream;
	stream.imbue(locale(locale::classic(), facet));
	stream << timestamp;

	return stream.str();
}

/// convert chrono::time_point to uint64_t
uint64_t TimeUtil::TPoint2Millis(const chrono::system_clock::time_point& tp) {
	auto duration = tp.time_since_epoch();
	auto totalMillis = chrono::duration_cast<chrono::milliseconds>(duration);
//	cout << "sec:" << chrono::duration_cast<chrono::seconds>(duration).count()
//			<< ", milli:" << chrono::duration_cast<chrono::milliseconds>(duration).count()
//			<< ", hours:" << chrono::duration_cast<chrono::hours>(duration).count() << endl;

	return totalMillis.count();
}

/// convert total milliseconds(uint64_t) of time to chrono::time_point
chrono::system_clock::time_point TimeUtil::Milli2TPoint(uint64_t totalMillis) {
	auto duration = chrono::milliseconds(totalMillis);
	chrono::system_clock::time_point timepoint(duration);

	return timepoint;
}

/// convert timeval to chrono::time_point
void TimeUtil::TPoint2Timeval(const chrono::system_clock::time_point &timepoint, timeval &timevalue) {
	// timepoint -> duration
    auto duration = timepoint.time_since_epoch();

	// duration -> timevalue
	chrono::microseconds usec = chrono::duration_cast<chrono::microseconds>(duration);
	if (usec <= chrono::microseconds(0))
		timevalue.tv_sec = timevalue.tv_usec = 0;
	else {
		timevalue.tv_sec = usec.count() / 1000000;
		timevalue.tv_usec = usec.count() % 1000000;
	}
}
