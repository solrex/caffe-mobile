#include <jni.h>
#include <android/log.h>

#include "caffe_mobile.hpp"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_setBlasThreadNum(JNIEnv *env, jobject instance,
                                                            jint numThreads) {
  openblas_set_num_threads(numThreads);
}

JNIEXPORT jboolean JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_loadModel(JNIEnv *env, jobject instance,
                                                     jstring modelPath_, jstring weightPath_) {
    jboolean ret = true;
    const char *modelPath = env->GetStringUTFChars(modelPath_, 0);
    const char *weightPath = env->GetStringUTFChars(weightPath_, 0);

    if (caffe::CaffeMobile::get(modelPath, weightPath) == NULL) {
        ret = false;
    }

    env->ReleaseStringUTFChars(modelPath_, modelPath);
    env->ReleaseStringUTFChars(weightPath_, weightPath);
    return ret;
}

JNIEXPORT jfloatArray JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_predict(JNIEnv *env, jobject instance,
                                                   jbyteArray jrgba, jint jchannels, jfloatArray jmean) {
  uint8_t *rgba = NULL;
  // Get matrix pointer
  if (NULL != jrgba) {
    rgba = (uint8_t *)env->GetByteArrayElements(jrgba, 0);
  } else {
    __android_log_print(ANDROID_LOG_WARN, "caffe-jni", "predict(): invalid args: jrgba(NULL)");
    return NULL;
  }
  std::vector<float> mean;
  if (NULL != jmean) {
    float * mean_arr = (float *)env->GetFloatArrayElements(jmean, 0);
    int mean_size = env->GetArrayLength(jmean);
    mean.assign(mean_arr, mean_arr+mean_size);
  } else {
    __android_log_print(ANDROID_LOG_INFO, "caffe-jni", "predict(): args: jmean(NULL)");
  }
  // Predict
  caffe::CaffeMobile *caffe_mobile = caffe::CaffeMobile::get();
  if (NULL == caffe_mobile) {
    __android_log_print(ANDROID_LOG_WARN, "caffe-jni", "predict(): CaffeMobile failed to initialize");
    return NULL;  // not initialized
  }
  int rgba_len = env->GetArrayLength(jrgba);
  if (rgba_len != jchannels * caffe_mobile->input_width() * caffe_mobile->input_height()) {
    __android_log_print(ANDROID_LOG_WARN, "caffe-jni", "predict(): invalid rgba length(%d) expect(%d)",
                        rgba_len, jchannels * caffe_mobile->input_width() * caffe_mobile->input_height());
    return NULL;  // not initialized
  }
  std::vector<float> predict;
  if (!caffe_mobile->predictImage(rgba, jchannels, mean, predict)) {
    __android_log_print(ANDROID_LOG_WARN, "caffe-jni", "predict(): CaffeMobile failed to predict");
    return NULL; // predict error
  }
  //__android_log_print(ANDROID_LOG_INFO, "caffe-jni", "CaffeMobile::predictImage result size=%zd", predict.size());
  //for (size_t i=0; i<predict.size(); i++) {
  //  __android_log_print(ANDROID_LOG_INFO, "caffe-jni", "result[%zd]=%f", i, predict[i]);
  //}
  // Handle result
  jfloatArray result = env->NewFloatArray(predict.size());
  if (result == NULL) {
    return NULL; // out of memory error thrown
  }
  // move from the temp structure to the java structure
  env->SetFloatArrayRegion(result, 0, predict.size(), predict.data());
  return result;
}

JNIEXPORT jint JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_inputChannels(JNIEnv *env, jobject instance) {
  // Predict
  caffe::CaffeMobile *caffe_mobile = caffe::CaffeMobile::get();
  if (NULL == caffe_mobile) {
      return -1;  // not initialized
  }
  return caffe_mobile->input_channels();
}

JNIEXPORT jint JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_inputWidth(JNIEnv *env, jobject instance) {
  // Predict
  caffe::CaffeMobile *caffe_mobile = caffe::CaffeMobile::get();
  if (NULL == caffe_mobile) {
      return -1;  // not initialized
  }
  return caffe_mobile->input_width();
}

JNIEXPORT jint JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_inputHeight(JNIEnv *env, jobject instance) {
  // Predict
  caffe::CaffeMobile *caffe_mobile = caffe::CaffeMobile::get();
  if (NULL == caffe_mobile) {
    return -1;  // not initialized
  }
  return caffe_mobile->input_height();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env = NULL;
  jint result = -1;

  if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
    LOG(FATAL) << "GetEnv failed!";
    return result;
  }
  return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
