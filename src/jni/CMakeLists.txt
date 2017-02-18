cmake_minimum_required(VERSION 2.8)

if(ANDROID)
  add_library(caffe-jni SHARED caffe_jni.cpp caffe_mobile.cpp)
  target_link_libraries(caffe-jni ${Caffe_LINK})
  target_link_libraries(caffe-jni log)
  caffe_default_properties(caffe-jni)

  # install
  install(TARGETS caffe-jni DESTINATION lib)
endif()
