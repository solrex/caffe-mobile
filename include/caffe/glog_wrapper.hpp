#ifndef GLOG_WRAPPER_HPP
#define GLOG_WRAPPER_HPP

#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

namespace caffe{

struct nullstream: std::ostream{
  nullstream(): std::ostream(0){}
};

template <typename T>
nullstream &operator<<(nullstream &o, T const & x) { return o;}
extern nullstream __nullstream;

class LogMessage{
  std::string level;
  std::ostream &ofs;
  static bool enable;
  public:
    LogMessage(const std::string &l)
      :level(l), ofs(enable ? std::cerr : __nullstream){
      stream() << "[" << level << "]\t";
    }
    LogMessage(std::ostream &o)
      :level("ERROR"), ofs(o){
      stream() << "[" << level << "]\t";
    }
    inline std::ostream &stream(){
      return ofs;
    }
    ~LogMessage() {
      stream() << std::endl;
    }

    static void Enable(bool _enable){
      enable = _enable;
    }
};
}

#define   LOG(type)   caffe::LogMessage(#type).stream()
#define   DLOG(type)   caffe::LogMessage(#type).stream()
#define   VLOG(level)   if ((level) <= FLAGS_v) LOG(INFO)

#define CHECK(x) if(x) {} else LOG(ERROR) << #x
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

#endif
