#ifdef USE_NNPACK
#include <cerrno>
#include <cstdint>
#include <cstdlib>

#include "nnpack.h"

#include "caffe/layers/nnpack_convolution_layer.hpp"

namespace caffe {

// NOTE: FAST BUT NOT WORK
//static char workspace_buffer[buffer_size];
//static const size_t buffer_size = 64*10*1024;

namespace {

void caffe_nnp_convolution_forward(
    nnp_convolution_algorithm algorithm,
    nnp_convolution_transform_strategy transform_strategy,
    const Blob<float>& bottom,
    const Blob<float>& weights,
    const Blob<float>& bias,
    const Blob<int>& pad,
    Blob<float>* top) {
  const size_t batch_size = bottom.num();
  const size_t input_channels = bottom.channels();
  const size_t output_channels = top->channels();
  const nnp_size input_size = {static_cast<size_t>(bottom.width()),
                               static_cast<size_t>(bottom.height())};
  const nnp_size kernel_size = {
      .width = static_cast<size_t>(weights.width()),
      .height = static_cast<size_t>(weights.height())};
  const nnp_padding padding = {.top = static_cast<size_t>(pad.cpu_data()[0]),
                               .right = static_cast<size_t>(pad.cpu_data()[1]),
                               .bottom = static_cast<size_t>(pad.cpu_data()[0]),
                               .left = static_cast<size_t>(pad.cpu_data()[1])};

  //size_t workspace_size = buffer_size;
  const nnp_size stride = {1, 1};

  if (batch_size == 1) {
    LOG(INFO) << "Running inference mode";
    const auto status = nnp_convolution_inference(
        algorithm,
        transform_strategy,
        input_channels,
        output_channels,
        input_size,
        padding,
        kernel_size,
        stride,
        bottom.cpu_data(),
        weights.cpu_data(),
        bias.cpu_data(),
        top->mutable_cpu_data(),
        NULL,
        NULL,
        //workspace_buffer,
        //&workspace_size,
        nnp_activation_identity,
        NULL,
        NULL,
        NULL);
    CHECK_EQ(nnp_status_success, status);
  } else {
    LOG(INFO) << "Running batched mode";
    const auto status = nnp_convolution_output(
        algorithm,
        batch_size,
        input_channels,
        output_channels,
        input_size,
        padding,
        kernel_size,
        bottom.cpu_data(),
        weights.cpu_data(),
        bias.cpu_data(),
        top->mutable_cpu_data(),
        NULL,
        nullptr);
    CHECK_EQ(nnp_status_success, status);
  }
}

}


template <typename Dtype>
void NNPackConvolutionLayer<Dtype>::Forward_cpu(
    const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  return ConvolutionLayer<Dtype>::Forward_cpu(bottom, top);
}

template <>
void NNPackConvolutionLayer<float>::Forward_cpu(
    const vector<Blob<float>*>& bottom,
    const vector<Blob<float>*>& top) {
  if (!this->bias_term_) {
    LOG(INFO) << "NNPACK Convolution requires a bias term, falling back";
    return ConvolutionLayer<float>::Forward_cpu(bottom, top);
  }

  bool is_stride_1 = true;
  for (auto i = 0; i < num_spatial_axes_; ++i) {
    if (this->stride_.cpu_data()[i] != 1) {
      is_stride_1 = false;
    }
  }

  if (!is_stride_1) {
    LOG(INFO) << "NNPACK Convolution requires strdie 1, falling back";
    return ConvolutionLayer<float>::Forward_cpu(bottom, top);
  }

  CHECK(this->bias_term_);
  CHECK(is_stride_1);
  for (int i = 0; i < bottom.size(); ++i) {
    caffe_nnp_convolution_forward(
        static_cast<nnp_convolution_algorithm>(this->layer_param_.convolution_param().nnpack_algorithm()),
        static_cast<nnp_convolution_transform_strategy>(this->layer_param_.convolution_param()
            .nnpack_transform_strategy()),
        *(bottom[i]),
        *(this->blobs_[0]),
        *(this->blobs_[1]),
        this->pad_,
        top[i]);
  }
}

template <typename Dtype>
void NNPackConvolutionLayer<Dtype>::Backward_cpu(
    const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down,
    const vector<Blob<Dtype>*>& bottom) {
  LOG(ERROR) << "Not implemented";
}

INSTANTIATE_CLASS(NNPackConvolutionLayer);

} // namespace caffe
#endif
