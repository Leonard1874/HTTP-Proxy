#include <climits>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <map>
#include <list>

class ResponseParser{
private:
  std::map<std::string,int> month;
public:
  ResponseParser(){
    month["Jan"] = 0;
    month["Feb"] = 1;
    month["Mar"] = 2;
    month["Apr"] = 3;
    month["May"] = 4;
    month["Jun"] = 5;
    month["Jul"] = 6;
    month["Aug"] = 7;
    month["Sept"] = 8;
    month["Oct"] = 9;
    month["Nov"] = 10;
    month["Dec"] = 11;
  }
  
  bool needValidate(const std::string& responseInfo){
    bool need = false;
    if(responseInfo.find("cache-control") != std::string::npos){
      if(responseInfo.find("must-revalidate") != std::string::npos || responseInfo.find("proxy-revalidate") != std::string::npos){
        need = true;
      }
    }
    return need;
  }

  bool canCache(const std::string& responseInfo){
    bool cache = true;
    if(responseInfo.find("cache-control") != std::string::npos){
      if(responseInfo.find("no-cache")!=std::string::npos || responseInfo.find("no-share")!=std::string::npos || responseInfo.find("private")!=std::string::npos){
        cache = false;
    }
  }
    return cache;
 }

  double parseExpire(const std::string& responseInfo){
    double freshtime = INT_MAX;
    if(responseInfo.find("Cache-Control: ") != std::string::npos){
      std::cout << "cache-contrl found!" << std::endl;
      freshtime = freshExpireControl(responseInfo);
    }
    else if(responseInfo.find("Expires: ") != std::string::npos){
      std::cout << "expires found!" << std::endl;
      freshtime = freshExpireLastModifyExp(responseInfo,true);
    }
    else if(responseInfo.find("Last-Modified: ") != std::string::npos){
      std::cout << "last modified found!" << std::endl;
      freshtime = freshExpireLastModifyExp(responseInfo,false);
    }
    else{
      std::cout << "not found expire signs!" << std::endl;
    }
    return freshtime;
  }
  
private:
 
  double freshExpireControl(std::string strToParse){
    if(strToParse.find("max-age")!=std::string::npos || strToParse.find("s-maxage")!=std::string::npos){
      if(strToParse.find("max-age")!=std::string::npos){
        return std::stoi(getMarkedLine("max-age=",strToParse));
      }
      else{
        return std::stoi(getMarkedLine("s-maxage=",strToParse));
      }
    }
    else{
      std::cout << "other cache control!" << std::endl;
      return -1;
    }
  }

  double freshExpireLastModifyExp(std::string strToParse, bool isExp){
    double freshTime;
    std::string date = getMarkedLine("Date: ", strToParse);
    std::string last = getMarkedLine("Last-Modified: ", strToParse);
    std::vector<std::vector<std::string> > dateTimes;
    std::vector<std::string> times;
    if(!date.empty() && !last.empty()){
      dateTimes.push_back(getDateTime(date));
      dateTimes.push_back(getDateTime(last));
    }
    struct tm dateTM = toStructTM(dateTimes[0]);
    struct tm lastTM = toStructTM(dateTimes[1]);
    freshTime = difftime(mktime(&dateTM),mktime(&lastTM));
    if(!isExp){
      freshTime /= 10; 
    }
    return freshTime;
  }

  std::string getMarkedLine(std::string mark, std::string strToParse){
    std::string markedStr;
    size_t pos = strToParse.find(mark);
    if(pos != std::string::npos){
      strToParse = strToParse.substr(pos,mark.size());
      pos += mark.size();
      while(strToParse[pos]!='\r'){
        markedStr += strToParse[pos];
        pos ++;
      }
      std::cout << markedStr << std::endl;
      return markedStr;
    }
    else{
      std::cerr << "can't find mark" << std::endl;
      return "";
    }
  }

  struct tm toStructTM(std::vector<std::string> times){
    struct tm res;
    res.tm_mday = std::stoi(times[0]);
    res.tm_mon = month[times[1]];
    res.tm_year = std::stoi(times[2]) - 1900;
    res.tm_hour = std::stoi(times[3]);
    res.tm_min = std::stoi(times[4]);
    res.tm_sec = std::stoi(times[5]);
    return res;
  }

  std::vector<std::string> getDateTime(std::string time){
    std::vector<std::string> res;
    std::string day;
    std::string mon;
    std::string year;
    std::string hour;
    std::string min;
    std::string sec;
    
    size_t daySpc = time.find_first_of(" ");

    daySpc ++;
    while(isdigit(time[daySpc])){
      day += time[daySpc];
      daySpc ++;
    }
    res.push_back(day);

    daySpc ++;
    while(isalpha(time[daySpc])){
      mon += time[daySpc];
      daySpc ++;
    }
    res.push_back(mon);

    daySpc ++;
    while(isdigit(time[daySpc])){
      year += time[daySpc];
      daySpc ++;
    }
    res.push_back(year);

    daySpc ++;
    while(isdigit(time[daySpc])){
      hour += time[daySpc];
      daySpc ++;
    }
    res.push_back(hour);

    daySpc ++;
    while(isdigit(time[daySpc])){
      min += time[daySpc];
      daySpc ++;
    }
    res.push_back(min);

    daySpc ++;
    while(isdigit(time[daySpc])){
      sec += time[daySpc];
      daySpc ++;
    }
    res.push_back(sec);
    //std::cout << day << mon << year << hour << min << sec << std::endl;
    return res;
  }

};
