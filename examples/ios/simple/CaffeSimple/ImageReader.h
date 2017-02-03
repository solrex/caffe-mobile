//
//  ImageReader.h
//  CaffeSimple
//
//  Created by Wenbo Yang on 2017/2/3.
//  Copyright © 2017年 com.yangwenbo. All rights reserved.
//

#ifndef ImageReader_h
#define ImageReader_h

#include "caffe/caffe.hpp"

bool ReadImageToBlob(const char* file_name, caffe::Blob<float>* input_layer);

#endif /* ImageReader_h */
