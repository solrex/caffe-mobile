//
//  ViewController.h
//  CaffeSimple
//
//  Created by Wenbo Yang on 2017/2/3.
//  Copyright © 2017年 com.yangwenbo. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

#pragma mark Console
@property (nonatomic,strong) IBOutlet UITextView *console;

@property (nonatomic,strong) IBOutlet UIImageView *test_image;

#pragma mark ClickEvent
- (IBAction)RunCaffeModel:(UIButton *)btn;

@end
