//
//  iPhoneAugmentedTestAppDelegate.h
//  iPhoneAugmentedTest
//
//  Created by Kamen Dimitrov on 12/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>



@class iPhoneAugmentedTestViewController;

@interface iPhoneAugmentedTestAppDelegate : NSObject <UIApplicationDelegate> {
   UIWindow *window;
   iPhoneAugmentedTestViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet iPhoneAugmentedTestViewController *viewController;

@end

