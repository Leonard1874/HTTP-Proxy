#include <cstdio>
#include <cstdlib>
#include <string>
class Timer{
public:
  Timer() = default;
  std::string getCurrentDateTime(std::string s){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
      strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
      strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
  }

  double getCurrentSec(){
    return time(0);
  }

  std::string getlocalTimeStr(long timeNum){
    time_t rawtime = timeNum;
    struct tm * timeinfo;
    timeinfo = localtime (&rawtime);
    std::string timeStr(asctime(timeinfo));
    return timeStr;
  }
  
};
