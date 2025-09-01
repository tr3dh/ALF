#include "__timeStamp.h"

std::string getTimestamp() {

    // aktuelle Zeit
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    // lokales Datum und Zeit
    std::tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &t); // Windows
#else
    localtime_r(&t, &localTime); // POSIX
#endif

    // Formatieren als YYYY-MM-DD-HH-MM-SS
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d-%H-%M-%S");
    return oss.str();
}