#pragma once

#include "caffe/layers/conv_layer.hpp"

namespace caffe {
#ifdef USE_NNPACK
template <typename Dtype>
class NNPackConvolutionLayer : public ConvolutionLayer<Dtype> {
 public:
  explicit NNPackConvolutionLayer(const LayerParameter& param)
      : ConvolutionLayer<Dtype>(param) {}
  virtual inline const char* type() const {
    return "NNPackConvolution";
  }
  virtual void Forward_cpu(
      const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top);
  virtual void Backward_cpu(
      const vector<Blob<Dtype>*>& top,
      const vector<bool>& propagate_down,
      const vector<Blob<Dtype>*>& bottom);
};
#endif
}
