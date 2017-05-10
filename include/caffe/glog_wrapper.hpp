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
  LogMessage(const char *file, int line, const char * severity,
             bool cond_mode = false, bool cond = true)
  : cond_mode_(cond_mode), cond_(cond), cont_(true) {
    stream() << severity << " " << file << ":" << line << "] ";
  }

  std::iostream &stream() {
    return log_stream_;
  }

  bool cont() {
    return cont_;
  }

  void print() {
    cont_ = false;
    if (cond_mode_ && (!cond_)) {
      return;
    }
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, "caffe", log_stream_.str().c_str());
#else
    std::cout << log_stream_.str() << std::endl;
#endif
  }
private:
  std::stringstream log_stream_;    ///< logging stream
  bool cond_mode_;  ///< cond mode
  bool cond_;       ///< cond
  bool cont_;       ///< Should continue
};

}

#define LOG(severity) \
    for(caffe::LogMessage _logger(__FILE__, __LINE__, #severity); \
        _logger.cont(); _logger.print()) _logger.stream()

#define DLOG(severity)  LOG(severity)
#define VLOG(severity)  LOG(severity)

#define LOG_IF(severity, condition) \
    for(caffe::LogMessage _logger(__FILE__, __LINE__, #severity, #condition, \
                                  static_cast<bool>(condition)); \
        _logger.cont(); _logger.print()) _logger.stream()

#define CHECK(condition) \
    LOG_IF(FATAL, (!(condition))) << "Check failed: " #condition " "

#define DCHECK(x) CHECK(x)

#define   CHECK_EQ(x, y)   CHECK((x) == (y))
#define   CHECK_LT(x, y)   CHECK((x) < (y))
#define   CHECK_GT(x, y)   CHECK((x) > (y))
#define   CHECK_LE(x, y)   CHECK((x) <= (y))
#define   CHECK_GE(x, y)   CHECK((x) >= (y))
#define   CHECK_NE(x, y)   CHECK((x) != (y))

#define   DCHECK_EQ(x, y)   DCHECK((x) == (y))
#define   DCHECK_LT(x, y)   DCHECK((x) < (y))
#define   DCHECK_GT(x, y)   DCHECK((x) > (y))
#define   DCHECK_LE(x, y)   DCHECK((x) <= (y))
#define   DCHECK_GE(x, y)   DCHECK((x) >= (y))
#define   DCHECK_NE(x, y)   DCHECK((x) != (y))

#define LOG_EVERY_N(severity, n) \
  static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0; \
  ++LOG_OCCURRENCES; \
  if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n; \
  if (LOG_OCCURRENCES_MOD_N == 1) \
    LOG(severity) << "REPEAT:" << LOG_OCCURRENCES << " "


#endif // USE_GLOG

#endif
