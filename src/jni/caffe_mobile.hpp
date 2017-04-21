#ifndef JNI_CAFFE_MOBILE_HPP_
#define JNI_CAFFE_MOBILE_HPP_

#include <string>
#include <vector>
#include "caffe/caffe.hpp"

namespace caffe {

/**
 * @brief Wrap caffe::net to a simpler interface; singleton
 */
class CaffeMobile {
public:
  /**
   * @brief Destructor
   */
  ~CaffeMobile();

  /**
   * @brief Get the CaffeMobile singleton from the param file (*.prototxt)
   * and the tained file (*.caffemodel)
   * @return NULL: failed; not NULL: succeeded
   */
  static CaffeMobile *get(const std::string &param_file,
                          const std::string &trained_file);

  /**
   * @brief Get the exist CaffeMobile singleton pointer
   * @return NULL: no exist; not NULL: exist
   */
  static CaffeMobile *get();

  /**
   * @brief Use loaded model to classify a Image
   * @param rgba: Grayscale(1 channel) or BGR(3 channels) pixels array
   */
  bool predictImage(const uint8_t *rgba,
                    int channels,
                    const std::vector<float> &mean,
                    std::vector<float> &result);

  int input_channels() {
    return input_channels_;
  }

  int input_width() {
    return input_width_;
  }

  int input_height() {
      return input_height_;
  }
private:
  /**
   * @brief Construct a caffe net from the param file (*.prototxt)
   * and the tained file (*.caffemodel)
   */
  CaffeMobile(const string &param_file, const string &trained_file);


  /// @brief
  static CaffeMobile *caffe_mobile_;
  /// @brief
  shared_ptr<Net<float>> net_;
  /// @brief
  int input_channels_;
  int input_width_;
  int input_height_;
};

} // namespace caffe

#endif
