#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cblas.h>

#include "caffe/caffe.hpp"
#include "caffe_mobile.hpp"

#ifdef __cplusplus
extern "C" {
#endif

using std::string;
using std::vector;
using caffe::CaffeMobile;

int getTimeSec() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (int)now.tv_sec;
}

string jstring2string(JNIEnv *env, jstring jstr) {
  const char *cstr = env->GetStringUTFChars(jstr, 0);
  string str(cstr);
  env->ReleaseStringUTFChars(jstr, cstr);
  return str;
}

/**
 * NOTE: byte[] buf = str.getBytes("US-ASCII")
 */
string bytes2string(JNIEnv *env, jbyteArray buf) {
  jbyte *ptr = env->GetByteArrayElements(buf, 0);
  string s((char *)ptr, env->GetArrayLength(buf));
  env->ReleaseByteArrayElements(buf, ptr, 0);
  return s;
}

JNIEXPORT void JNICALL
Java_com_ooolab_caffemobile_CaffeMobile_setNumThreads(JNIEnv *env,
                                                             jobject thiz,
                                                             jint numThreads) {
  int num_threads = numThreads;
  openblas_set_num_threads(num_threads);
}

JNIEXPORT jint JNICALL Java_com_yangwenbo_caffemobile_CaffeMobile_loadModel(
    JNIEnv *env, jobject thiz, jstring modelPath, jstring weightsPath) {
  CaffeMobile::get(jstring2string(env, modelPath),
                   jstring2string(env, weightsPath));
  return 0;
}

/**
 * NOTE: when width == 0 && height == 0, buf is a byte array
 * (str.getBytes("US-ASCII")) which contains the img path
 */
JNIEXPORT jfloatArray JNICALL
Java_com_yangwenbo_caffemobile_CaffeMobile_predict(
    JNIEnv *env, jobject thiz, jbyteArray bitmap, jint width, jint height,
    jint channels, jint k) {
  // Get matrix pointer
  jbyte *ptr = env->GetByteArrayElements(bitmap, 0);
  // Predict
  CaffeMobile *caffe_mobile = CaffeMobile::get();
  vector<float> predict = caffe_mobile->predictImage(ptr, width, height, channels);
  // Handle result
  jfloatArray result = env->NewFloatArray(k);
  if (result == NULL) {
    return NULL; /* out of memory error thrown */
  }
  // move from the temp structure to the java structure
  env->SetFloatArrayRegion(result, 0, k, predict.data());
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
