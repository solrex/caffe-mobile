//
//  ViewController.m
//  CaffeSimple
//
//  Created by Wenbo Yang on 2017/2/3.
//  Copyright © 2017年 com.yangwenbo. All rights reserved.
//

#import "ViewController.h"

#include <numeric>
#include "caffe/caffe.hpp"
#include "ImageReader.h"

@interface ViewController ()

@end

@implementation ViewController

caffe::Net<float> *_net;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSString *test_file_path = FilePathForResourceName(@"test_image", @"jpg");
    UIImage *image = [UIImage imageWithContentsOfFile:test_file_path];
    [_test_image setImage:image];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated{
}

- (void)viewDidAppear:(BOOL)animated {
    caffe::CPUTimer timer;
    timer.Start();
    NSString *modle_path = FilePathForResourceName(@"net", @"protobin");
    _net = new caffe::Net<float>([modle_path UTF8String], caffe::TEST);
    NSString *weight_path = FilePathForResourceName(@"weight", @"caffemodel");
    _net->CopyTrainedLayersFrom([weight_path UTF8String]);
    caffe::Blob<float> *input_layer = _net->input_blobs()[0];
    timer.Stop();
    [_console insertText:[NSString stringWithFormat:@"%fms\n", timer.MilliSeconds()]];
    LOG(INFO) << "Input layer info: channels:" << input_layer->channels()
    << " width: " << input_layer->width() << " Height:" << input_layer->height();

}

- (void)RunCaffeModel:(UIButton *)btn {
    caffe::CPUTimer timer;
    [_console insertText:@"\nCaffe inferring...\n"];
    caffe::Blob<float> *input_layer = _net->input_blobs()[0];
    NSString *test_file_path = FilePathForResourceName(@"test_image", @"jpg");
    timer.Start();
    std::vector<float> mean({81.3, 107.3, 105.3});
    if(! ReadImageToBlob(test_file_path, mean, input_layer)) {
        LOG(INFO) << "ReadImageToBlob failed";
        [_console insertText:@"ReadImageToBlob failed"];
        return;
    }
    _net->Forward();
    timer.Stop();
    int topN = 10;
    [_console insertText:[NSString stringWithFormat:@"Top%d Results (%fms): \n", topN, timer.MilliSeconds()]];
    
    caffe::Blob<float> *output_layer = _net->output_blobs()[0];
    const float *begin = output_layer->cpu_data();
    const float *end = begin + output_layer->shape(1);
    std::vector<float> result(begin, end);
    std::vector<size_t> result_idx(result.size());
    std::iota(result_idx.begin(), result_idx.end(), 0);
    std::sort(result_idx.begin(), result_idx.end(),
              [&result](size_t l, size_t r){return result[l] > result[r];});
    for (int i=0; i<topN; i++) {
        LOG(INFO) << "output[" << result_idx[i] << "]=" << result[result_idx[i]];
        [_console insertText:[NSString stringWithFormat:@"output[%lu] \t= %f\n", result_idx[i], result[result_idx[i]] ] ];
    }
    
}


@end
