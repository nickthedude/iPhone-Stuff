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
#import <AVFoundation/AVFoundation.h>


@interface iPhoneAugmentedTestViewController : UIViewController <AVCaptureVideoDataOutputSampleBufferDelegate>
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

	AVCaptureSession *captureSession;
	AVCaptureVideoPreviewLayer *previewLayer;
	AVCaptureConnection *videoConnection;

	
}

@property (nonatomic, retain) UIImageView *testImageView;
@property (nonatomic, retain) CALayer *outputLayer;
@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

@property (retain) AVCaptureVideoPreviewLayer *previewLayer;
@property (retain) AVCaptureSession *captureSession;

- (void) addVideoPreviewLayer;
- (void) addVideoInput;
- (void) addVideoDataOutput;


- (void)startAnimation;
- (void)stopAnimation;
- (void) flushARToolKitMatrix;
- (void) updateTextureAndScanForMarkers: (CGImageRef) img;

@end
