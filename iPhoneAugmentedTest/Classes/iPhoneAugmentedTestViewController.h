//
//  iPhoneAugmentedTestViewController.h
//  iPhoneAugmentedTest
//
//  Created by Kamen Dimitrov on 12/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import "NyARToolkitWrapper.h"

@interface iPhoneAugmentedTestViewController : UIViewController
{
    EAGLContext *context;
    GLuint program;
	
	// OpenGL name for the texture
	
	GLuint spriteTexture;
	
	NyARToolkitWrapper* artoolkit;
	
	float ogl_camera_matrix[16];
	float ogl_projection_matrix[16];		
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (void) tempScanImage;
- (void) loadTextureFromImageAndScan;

@end
