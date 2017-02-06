//
//  ViewController.m
//  CaffeSimple
//
//  Created by Wenbo Yang on 2017/2/3.
//  Copyright © 2017年 com.yangwenbo. All rights reserved.
//

#import "ViewController.h"

#include "caffe/caffe.hpp"
#include "ImageReader.h"

@interface ViewController ()

@end

@implementation ViewController

caffe::Net<float> *_net;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSString *test_file_path = FilePathForResourceName(@"61", @"png");
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
    NSString *modle_path = FilePathForResourceName(@"lenet", @"prototxt");
    _net = new caffe::Net<float>([modle_path UTF8String], caffe::TEST);
    NSString *weight_path = FilePathForResourceName(@"lenet_iter_10000", @"caffemodel");
    _net->CopyTrainedLayersFrom([weight_path UTF8String]);
    caffe::Blob<float> *input_layer = _net->input_blobs()[0];
    timer.Stop();
    [_console insertText:[NSString stringWithFormat:@"%fms\n", timer.MilliSeconds()]];
    LOG(INFO) << "Input layer info: channels:" << input_layer->channels()
    << " width: " << input_layer->width() << " Height:" << input_layer->height();

}

- (void)RunCaffeModel:(UIButton *)btn {
    caffe::CPUTimer timer;
    [_console insertText:@"\nCaffe infering..."];
    caffe::Blob<float> *input_layer = _net->input_blobs()[0];
    NSString *test_file_path = FilePathForResourceName(@"61", @"png");
    timer.Start();
    ReadImageToBlob(test_file_path, input_layer);
    _net->Forward();
    timer.Stop();
    [_console insertText:[NSString stringWithFormat:@"%fms\n", timer.MilliSeconds()]];
    
    [_console insertText:@"Inference result:\n"];
    caffe::Blob<float> *output_layer = _net->output_blobs()[0];
    const float *begin = output_layer->cpu_data();
    const float *end = begin + output_layer->channels();
    std::vector<float> result(begin, end);
    for (int i=0; i<result.size(); i++) {
        LOG(INFO) << "result[" << i << "]=" << result[i];
        [_console insertText:[NSString stringWithFormat:@"result[%d]=%f\n", i, result[i]]];
    }
}


@end
