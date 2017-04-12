#ifdef USE_NNPACK
#include "caffe/layers/nnpack_inner_product_layer.hpp"
#include "nnpack.h"

namespace caffe {

template <typename Dtype>
void NNPackInnerProductLayer<Dtype>::Forward_cpu(
    const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  return InnerProductLayer<Dtype>::Forward_cpu(bottom, top);
}

template <>
void NNPackInnerProductLayer<float>::Forward_cpu(
    const vector<Blob<float>*>& bottom,
    const vector<Blob<float>*>& top) {
  LOG(INFO) << "Using NNPACKInnerProduct";
  CHECK_EQ(top.size(), 1);
  CHECK_EQ(bottom.size(), 1);
  if (M_ == 1) {
    const auto status = nnp_fully_connected_inference(
        K_,
        N_,
        bottom[0]->cpu_data(),
        this->blobs_[0]->cpu_data(),
        top[0]->mutable_cpu_data(),
        NULL);
    CHECK_EQ(status, nnp_status_success);
  } else {
    const auto status = nnp_fully_connected_output(
        M_,
        K_,
        N_,
        bottom[0]->cpu_data(),
        this->blobs_[0]->cpu_data(),
        top[0]->mutable_cpu_data(),
        NULL,
        nullptr);
    CHECK_EQ(status, nnp_status_success);
  }
  if (bias_term_) {
    caffe_cpu_gemm<float>(
        CblasNoTrans,
        CblasNoTrans,
        M_,
        N_,
        1,
        (float)1.,
        bias_multiplier_.cpu_data(),
        this->blobs_[1]->cpu_data(),
        (float)1.,
        top[0]->mutable_cpu_data());
  }
}

INSTANTIATE_CLASS(NNPackInnerProductLayer);
}
#endif
