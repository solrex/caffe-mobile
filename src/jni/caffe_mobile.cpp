#include "caffe_mobile.hpp"

namespace caffe {

CaffeMobile *CaffeMobile::caffe_mobile_ = NULL;

CaffeMobile *CaffeMobile::get() {
  return caffe_mobile_;
}

CaffeMobile *CaffeMobile::get(const string &param_file,
                              const string &trained_file) {
  if (!caffe_mobile_) {
    try {
      caffe_mobile_ = new CaffeMobile(param_file, trained_file);
    } catch (std::invalid_argument &e) {
      // TODO
    }
  }
  return caffe_mobile_;
}

CaffeMobile::CaffeMobile(const string &param_file, const string &trained_file) {
  // Load Caffe model
  Caffe::set_mode(Caffe::CPU);
  CPUTimer timer;
  timer.Start();
  net_.reset(new Net<float>(param_file, caffe::TEST));
  if (net_.get() == NULL) {
    throw std::invalid_argument("Invalid arg: param_file=" + param_file);
  }
  net_->CopyTrainedLayersFrom(trained_file);
  timer.Stop();
  LOG(INFO) << "Load (" << param_file << "," << trained_file << "), time:"
            << timer.MilliSeconds() << " ms.";

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  // Get input_layer info
  Blob<float> *input_layer = net_->input_blobs()[0];
  input_channels_ = input_layer->channels();
  CHECK(input_channels_ == 3 || input_channels_ == 1)
      << "Input layer should have 1 or 3 channels.";
  input_width_  = input_layer->width();
  input_height_ = input_layer->height();
}

CaffeMobile::~CaffeMobile() {
    net_.reset();
}

bool CaffeMobile::predictImage(uint8_t* img_buf, int width, int height, int channels,
                               std::vector<float> &result) {
  if ((img_buf == NULL) || net_.get() == NULL) {
    LOG(ERROR) << "Invalid arguments: img_buf=" << img_buf
        << ",net_=" << net_.get();
    return false;
  }
  LOG(INFO) << "image_channels:" << channels << " input_channels:" << input_channels_;
  if (input_channels_ != channels) {
    LOG(ERROR) << "image_channels input_channels not match.";
    return false;
  }
  CPUTimer timer;
  timer.Start();
  // Write input
  Blob<float> *input_layer = net_->input_blobs()[0];
  float *input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_channels_*input_height_*input_width_; i++) {
    // Cast *uint8_t* to float
    input_data[i] = static_cast<float>(img_buf[i]);
  }
  // Do Inference
  net_->Forward();
  timer.Stop();
  LOG(INFO) << "Inference use " << timer.MilliSeconds() << " ms.";
  Blob<float> *output_layer = net_->output_blobs()[0];
  const float *begin = output_layer->cpu_data();
  const float *end = begin + output_layer->shape(1);
  result.assign(begin, end);
  return true;
}

} // namespace caffe
