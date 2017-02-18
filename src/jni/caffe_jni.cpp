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
                                                   jbyteArray img_buf, jint width, jint height, jint channels) {
  // Get matrix pointer
  uint8_t *ptr = (uint8_t *)env->GetByteArrayElements(img_buf, 0);
  // Predict
  caffe::CaffeMobile *caffe_mobile = caffe::CaffeMobile::get();
  if (NULL == caffe_mobile) {
      return NULL;  // not initialized
  }
  std::vector<float> predict;
  if (!caffe_mobile->predictImage(ptr, width, height, channels, predict)) {
    return NULL; // predict error
  }
  __android_log_print(ANDROID_LOG_INFO, "caffe-jni", "CaffeMobile::predictImage result size=%zd", predict.size());
  for (size_t i=0; i<predict.size(); i++) {
    __android_log_print(ANDROID_LOG_INFO, "caffe-jni", "result[%zd]=%f", i, predict[i]);
  }
  // Handle result
  jfloatArray result = env->NewFloatArray(predict.size());
  if (result == NULL) {
    return NULL; // out of memory error thrown
  }
  // move from the temp structure to the java structure
  env->SetFloatArrayRegion(result, 0, predict.size(), predict.data());
  return result;
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
