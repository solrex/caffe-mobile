#include "caffe_mobile.hpp"

namespace caffe {

CaffeMobile *CaffeMobile::caffe_mobile_ = NULL;

CaffeMobile *CaffeMobile::get() {
  return caffe_mobile_;
}

CaffeMobile *CaffeMobile::get(const string &model_path,
                              const string &weights_path) {
  if (!caffe_mobile_) {
    caffe_mobile_ = new CaffeMobile(model_path, weights_path);
  }
  return caffe_mobile_;
}

CaffeMobile::CaffeMobile(const string &model_path, const string &weights_path) {
  CHECK_GT(model_path.size(), 0) << "Need a model definition to score.";
  CHECK_GT(weights_path.size(), 0) << "Need model weights to score.";

  Caffe::set_mode(Caffe::CPU);

  clock_t t_start = clock();
  net_.reset(new Net<float>(model_path, caffe::TEST));
  net_->CopyTrainedLayersFrom(weights_path);
  clock_t t_end = clock();
  LOG(INFO) << "Loading time: " << 1000.0 * (t_end - t_start) / CLOCKS_PER_SEC
            << " ms.";

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float> *input_layer = net_->input_blobs()[0];
  input_channels_ = input_layer->channels();
  CHECK(input_channels_ == 3 || input_channels_ == 1)
      << "Input layer should have 1 or 3 channels.";
  input_width_  = input_layer->width();
  input_height_ = input_layer->height();
}

CaffeMobile::~CaffeMobile() { net_.reset(); }

std::vector<float>* CaffeMobile::predict(signed char *bgr_mat,
                             int width, int height, int channels) {
}


} // namespace caffe
