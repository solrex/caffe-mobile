#ifndef JNI_CAFFE_MOBILE_HPP_
#define JNI_CAFFE_MOBILE_HPP_

#include <string>
#include <vector>
#include "caffe/caffe.hpp"

namespace caffe {

class CaffeMobile {
public:
  ~CaffeMobile();

  static CaffeMobile *get(const std::string &model_path, const std::string &weights_path);

  static CaffeMobile *get();

  std::vector<float>* predict(signed char *bgr_mat,
                             int width, int height, int channels);
private:
  CaffeMobile(const string &model_path, const string &weights_path);


  static CaffeMobile *caffe_mobile_;
  shared_ptr<Net<float>> net_;
  int input_channels_;
  int input_width_;
  int input_height_;
};

} // namespace caffe

#endif
