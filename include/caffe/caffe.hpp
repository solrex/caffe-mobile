// caffe.hpp is the header file that you need to include in your code. It wraps
// all the internal caffe header files into one for simpler inclusion.

#ifndef CAFFE_CAFFE_HPP_
#define CAFFE_CAFFE_HPP_

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/filler.hpp"
#include "caffe/layer.hpp"
#include "caffe/layer_factory.hpp"
#include "caffe/net.hpp"
#ifdef NO_CAFFE_MOBILE
#include "caffe/parallel.hpp"
#endif
#include "caffe/proto/caffe.pb.h"
#ifdef NO_CAFFE_MOBILE
#include "caffe/solver.hpp"
#include "caffe/solver_factory.hpp"
#endif
#include "caffe/util/benchmark.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/upgrade_proto.hpp"

#endif  // CAFFE_CAFFE_HPP_
