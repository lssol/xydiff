#ifdef __SunOS
#include <sys/varargs.h>
#endif

#include "DeltaException.hpp"
#include <errno.h>

#include <stdarg.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};


int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}
#endif

MessageEngine::MessageEngine(const char *filename, int line, const char *method) {
  struct tm tm_time;
  struct timeval tval;
  time_t sec_time;

  time(&sec_time);
  gettimeofday(&tval, 0);
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_time, &sec_time);
#else
    localtime_r(&sec_time, &tm_time);
#endif
  char err[32];
  *err= 0;
  if(errno){
    snprintf(err, sizeof(err), " (%s)", strerror(errno));
    errno= 0;
  }

  snprintf(s, 499, 
	  "%02d/%02d/%02d %02d:%02d:%02d.%03ld %s:%d %s() : ",
	  tm_time.tm_mday, tm_time.tm_mon+1, tm_time.tm_year%100, 
	  tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
	  tval.tv_usec/1000,
    filename, line, method
	  );
	};

void MessageEngine::add(const char *format, ...) {
	int len=strlen(s);
#ifdef __SunOS
  snprintf(s+len, 499-len, "Sorry, Exception Text disabled on SunOS/Solaris");
#else
	va_list var_arg ;
	va_start (var_arg, format);
	vsnprintf(s+len, 499-len, format, var_arg);
	va_end(var_arg);
#endif
	};
	
char* MessageEngine::getStr(void) {
	return s;
	};
