//
//  ImageReader.m
//  CaffeSimple
//
//  Created by Wenbo Yang on 2017/2/3.
//  Copyright © 2017年 com.yangwenbo. All rights reserved.
//
#include "ImageReader.h"

#import <Foundation/Foundation.h>

NSString* FilePathForResourceName(NSString* name, NSString* extension) {
    NSString* file_path = [[NSBundle mainBundle] pathForResource:name ofType:extension];
    if (file_path == NULL) {
        LOG(FATAL) << "Couldn't find '" << [name UTF8String] << "."
	       << [extension UTF8String] << "' in bundle.";
    }
    return file_path;
}

bool ReadImageToBlob(const char* file_name, caffe::Blob<float>* input_layer) {
    // Get file size
    FILE* file_handle = fopen(file_name, "rb");
    fseek(file_handle, 0, SEEK_END);
    const size_t bytes_in_file = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);
    // Read file bytes
    std::vector<uint8_t> file_data(bytes_in_file);
    fread(file_data.data(), 1, bytes_in_file, file_handle);
    fclose(file_handle);
    CFDataRef file_data_ref = CFDataCreateWithBytesNoCopy(NULL, file_data.data(),
                                                          bytes_in_file,
                                                          kCFAllocatorNull);
    CGDataProviderRef image_provider = CGDataProviderCreateWithCFData(file_data_ref);
    // Determine file type
    const char* suffix = strrchr(file_name, '.');
    if (!suffix || suffix == file_name) {
        suffix = "";
    }
    // Read image to file
    CGImageRef image;
    if (strcasecmp(suffix, ".png") == 0) {
        image = CGImageCreateWithPNGDataProvider(image_provider, NULL, true,
                                                 kCGRenderingIntentDefault);
    } else if ((strcasecmp(suffix, ".jpg") == 0) ||
               (strcasecmp(suffix, ".jpeg") == 0)) {
        image = CGImageCreateWithJPEGDataProvider(image_provider, NULL, true,
                                                  kCGRenderingIntentDefault);
    } else {
        CFRelease(image_provider);
        CFRelease(file_data_ref);
        fprintf(stderr, "Unknown suffix for file '%s'\n", file_name);
        return 1;
    }
    // Get Image width and height
    const int width = (int)CGImageGetWidth(image);
    const int height = (int)CGImageGetHeight(image);
    const int channels = 3;
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    const int bytes_per_row = (width * channels);
    const int bytes_in_image = (bytes_per_row * height);
    std::vector<uint8_t> result(bytes_in_image);
    const int bits_per_component = 8;
    // Read Image to bitmap
    CGContextRef context = CGBitmapContextCreate(result.data(), width, height,
                                                 bits_per_component, bytes_per_row, color_space,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(color_space);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    
    // Release resources
    CGContextRelease(context);
    CFRelease(image);
    CFRelease(image_provider);
    CFRelease(file_data_ref);
    
    // Convert Bitmap (channels*width*height) to Matrix (width*height*channels)
    // Convert uint8_t to float
    float *input_data = input_layer->mutable_cpu_data();
    // y in height
    for (int c = 0; c < channels; c++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                input_data[c*width*height + y*width + x] = static_cast<float>(result[y*bytes_per_row + x*channels + c]);
            }
        }
    }
    return 0;
}
