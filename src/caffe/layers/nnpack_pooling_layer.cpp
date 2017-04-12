#ifdef USE_NNPACK
#include "caffe/layers/nnpack_pooling_layer.hpp"
#include "nnpack.h"

namespace caffe {

template <typename Dtype>
void NNPackPoolingLayer<Dtype>::Forward_cpu(
    const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  return PoolingLayer<Dtype>::Forward_cpu(bottom, top);
}

template <>
void NNPackPoolingLayer<float>::Forward_cpu(
    const vector<Blob<float>*>& bottom,
    const vector<Blob<float>*>& top) {
  CHECK_EQ(top.size(), 1);
  CHECK_EQ(bottom.size(), 1);

  // NNPACK pooling is a bit unstable at the moment. For now, just
  // fall back to the reference implementation except for 2x2s2p0
  if (this->layer_param_.pooling_param().pool() !=
          PoolingParameter_PoolMethod_MAX ||
      this->kernel_w_ != 2 || this->kernel_h_ != 2 || this->pad_h_ != 0 ||
      this->pad_w_ != 0 || this->stride_w_ != 2 || this->stride_h_ != 2) {
    LOG(INFO) << "Falling back to PoolingLayer";
    return PoolingLayer<float>::Forward_cpu(bottom, top);
  }

  LOG(INFO) << "Using NNPACKPoolingLayer";
  CHECK_EQ(this->kernel_w_, 2);
  CHECK_EQ(this->kernel_h_, 2);
  CHECK_EQ(this->stride_w_, 2);
  CHECK_EQ(this->stride_h_, 2);
  CHECK_EQ(this->pad_w_, 0);
  CHECK_EQ(this->pad_h_, 0);
  const nnp_size input_size = {.width = static_cast<size_t>(bottom[0]->width()),
                               .height =
                               static_cast<size_t>(bottom[0]->height())};
  LOG(INFO) << "Input: " << input_size.width << ", " << input_size.height;
  const nnp_padding input_padding = {
      .top = static_cast<size_t>(this->pad_h_),
      .right = static_cast<size_t>(this->pad_w_),
      .bottom = static_cast<size_t>(this->pad_h_),
      .left = static_cast<size_t>(this->pad_w_)};
  LOG(INFO) << "Input Padding: " << input_padding.top << ", "
          << input_padding.right;
  const nnp_size pooling_size = {.width = static_cast<size_t>(this->kernel_w_),
                                 .height = static_cast<size_t>(this->kernel_h_)};
  LOG(INFO) << "Pooling: " << pooling_size.width << ", " << pooling_size.height;
  const nnp_size pooling_stride = {
      .width = static_cast<size_t>(this->stride_w_),
      .height = static_cast<size_t>(this->stride_h_)};
  LOG(INFO) << "Pooling Stride: " << pooling_stride.width << ", "
          << pooling_stride.height;
  const auto status = nnp_max_pooling_output(
      bottom[0]->num(),
      bottom[0]->channels(),
      input_size,
      input_padding,
      pooling_size,
      pooling_stride,
      bottom[0]->cpu_data(),
      top[0]->mutable_cpu_data(),
      nullptr);
  CHECK_EQ(status, nnp_status_success);
}


INSTANTIATE_CLASS(NNPackPoolingLayer);

}
#endif
