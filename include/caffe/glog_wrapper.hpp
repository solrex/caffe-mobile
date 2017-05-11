#ifndef GLOG_WRAPPER_HPP
#define GLOG_WRAPPER_HPP

#ifndef USE_GLOG

#include <sstream>

#ifdef __ANDROID__
#include <android/log.h>
#else
#include <iostream>
#endif

namespace caffe{

class LogMessage {
public:
  LogMessage(const char *file, int line, const char * severity)
  : cont_(true) {
    stream() << severity << " " << file << ":" << line << "] ";
  }

  inline std::iostream & stream() {
    return log_stream_;
  }

  inline bool cont() {
    return cont_;
  }

  inline void print() {
    cont_ = false;
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, "caffe", log_stream_.str().c_str());
#else
    std::cout << log_stream_.str() << std::endl;
#endif
  }
private:
  std::stringstream log_stream_;    ///< logging stream
  bool cont_;       ///< Should continue
};

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".

class LogMessageVoidify {
public:
  LogMessageVoidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) { }
};

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(const char *file, int line, const char *names, T* t) {
  if (t == NULL) {
    LogMessage _logger(file, line, "FATAL");
    _logger.stream() << names;
    _logger.print();
  }
  return t;
}

}

#define LOG(severity) \
    for(caffe::LogMessage _logger(__FILE__, __LINE__, #severity); \
        _logger.cont(); _logger.print()) _logger.stream()

#define LOG_IF(severity, condition) \
  if(!(condition)) LOG(severity) << #condition " "

#define LOG_EVERY_N(severity, n) \
  static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0; \
  ++LOG_OCCURRENCES; \
  if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n; \
  if (LOG_OCCURRENCES_MOD_N == 1) \
    LOG(severity) << "REPEAT:" << LOG_OCCURRENCES << " "

#define CHECK(condition) LOG_IF(ERROR, condition)
#define CHECK_EQ(x, y)   CHECK((x) == (y))
#define CHECK_LT(x, y)   CHECK((x) < (y))
#define CHECK_GT(x, y)   CHECK((x) > (y))
#define CHECK_LE(x, y)   CHECK((x) <= (y))
#define CHECK_GE(x, y)   CHECK((x) >= (y))
#define CHECK_NE(x, y)   CHECK((x) != (y))
#define CHECK_NOTNULL(val) \
  caffe::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

#ifdef NDEBUG

#define DLOG(severity) \
  if(false) LOG(severity)

#define DCHECK(x)  \
  if(false) CHECK(x)

#else

#define DLOG(severity)    LOG(severity)
#define DCHECK(x)         CHECK(x)

#endif

#define DCHECK_EQ(x, y)   DCHECK((x) == (y))
#define DCHECK_LT(x, y)   DCHECK((x) < (y))
#define DCHECK_GT(x, y)   DCHECK((x) > (y))
#define DCHECK_LE(x, y)   DCHECK((x) <= (y))
#define DCHECK_GE(x, y)   DCHECK((x) >= (y))
#define DCHECK_NE(x, y)   DCHECK((x) != (y))

#define VLOG(severity)  LOG(severity)

#endif // USE_GLOG

#endif
