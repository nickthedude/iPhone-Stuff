//
//  iPhoneAugmentedTestViewController.m
//  iPhoneAugmentedTest
//
//  Created by Kamen Dimitrov on 12/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "iPhoneAugmentedTestViewController.h"
#import "EAGLView.h"

// Uniform index.
enum {
    UNIFORM_TRANSLATE,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    NUM_ATTRIBUTES
};

@interface iPhoneAugmentedTestViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;

@end

@implementation iPhoneAugmentedTestViewController

@synthesize captureSession;
@synthesize previewLayer;

@synthesize animating, context, displayLink, outputLayer, testImageView;

- (void)awakeFromNib
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    }
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
    
    if ([context API] == kEAGLRenderingAPIOpenGLES2)
        [self loadShaders];
    
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
}

- (void)dealloc
{
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
	if([self.captureSession isRunning]) {
		[self.captureSession stopRunning];
	}
	
	[artoolkit release];
	
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void) viewDidLoad {
	
	artoolkit = [[NyARToolkitWrapper alloc] init];
	
	// setup Textures
	
	glGenTextures(1, &spriteTexture);
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512,512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	captureSession = [[AVCaptureSession alloc] init];
	
	[self addVideoInput];
	[self addVideoDataOutput];
	[self addVideoPreviewLayer];
	
	self.previewLayer.frame = CGRectMake(0,0,320,480);
	
//	[self.view.layer addSublayer:self.previewLayer];
	
	[self.captureSession startRunning];
	
	
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
   
	// Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer); 
    // Lock the base address of the pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, 0); 
	
    // Get the number of bytes per row for the pixel buffer
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer); 
	
    // Get the number of bytes per row for the pixel buffer
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer); 
    // Get the pixel buffer width and height
    size_t width = CVPixelBufferGetWidth(imageBuffer); 
    size_t height = CVPixelBufferGetHeight(imageBuffer); 
	
    // Create a device-dependent RGB color space
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); 
	
    // Create a bitmap graphics context with the sample buffer data
    CGContextRef ctx = CGBitmapContextCreate(baseAddress, width, height, 8, 
												 bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst); 
    // Create a Quartz image from the pixel data in the bitmap graphics context
    CGImageRef img = CGBitmapContextCreateImage(ctx); 
	CGImageRetain(img);
	[self performSelectorOnMainThread:@selector(updateTextureAndScanForMarkers:) withObject:img waitUntilDone:NO];
	// Unlock the pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
	
    // Free up the context and color space
    CGContextRelease(ctx); 
    CGColorSpaceRelease(colorSpace);
    
	// Release the Quartz image
    CGImageRelease(img);
}

- (void) addVideoInput {
	
	AVCaptureDevice *videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];	
	if ( videoDevice ) {
		
		NSError *error;
		AVCaptureDeviceInput *videoIn = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
		if ( !error ) {
			if ([self.captureSession canAddInput:videoIn])
				[self.captureSession addInput:videoIn];
			else
				NSLog(@"Couldn't add video input");		
		}
		else
			NSLog(@"Couldn't create video input");
	}
	else
		NSLog(@"Couldn't create video capture device");
}


- (void) addVideoDataOutput {
	
	AVCaptureVideoDataOutput *videoOut = [[AVCaptureVideoDataOutput alloc] init];
	[videoOut setAlwaysDiscardsLateVideoFrames:YES];
	[videoOut setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]]; // BGRA is necessary for manual preview
	dispatch_queue_t my_queue = dispatch_queue_create("com.example.subsystem.taskXYZ", NULL);
	[videoOut setSampleBufferDelegate:self queue:my_queue];
	//	[videoOut setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
	if ([self.captureSession canAddOutput:videoOut])
		[self.captureSession addOutput:videoOut];
	else
		NSLog(@"Couldn't add video output");
	[videoOut release];
}

- (void) addVideoPreviewLayer {
	self.previewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:self.captureSession];
	self.previewLayer.frame = CGRectMake(0, 0, 320, 480);
	self.previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
}

UIImage *scaleAndRotateImage(CGImageRef imgRef)
{
	
	CGFloat width = CGImageGetWidth(imgRef);
	CGFloat height = CGImageGetHeight(imgRef);
	
	CGAffineTransform transform = CGAffineTransformIdentity;
	CGRect bounds = CGRectMake(0, 0, width, height);

	bounds.size.height = 320;
	bounds.size.width = 480;
	
	CGFloat scaleRatio = bounds.size.width / width;
	CGSize imageSize = CGSizeMake(CGImageGetWidth(imgRef), CGImageGetHeight(imgRef));

	bounds.size.height = 480;
	bounds.size.width = 320;
	transform = CGAffineTransformMakeTranslation(imageSize.height, 0.0);
	transform = CGAffineTransformRotate(transform, M_PI / 2.0);
	
	UIGraphicsBeginImageContext(bounds.size);
	
	CGContextRef context = UIGraphicsGetCurrentContext();
	
	CGContextScaleCTM(context, -scaleRatio, scaleRatio);
	CGContextTranslateCTM(context, -height, 0);
	
	CGContextConcatCTM(context, transform);
	
	CGContextDrawImage(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height), imgRef);
	
	UIImage *imageCopy = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
	
	return imageCopy;
}


- (void) updateTextureAndScanForMarkers: (CGImageRef) img {
		
	UIImage *tempUIImage = scaleAndRotateImage(img);
	
	CGImageRef scaledImgRef = [tempUIImage CGImage];
		
	int image_width = CGImageGetWidth(scaledImgRef);
	int image_height = CGImageGetHeight(scaledImgRef);
	
	GLubyte* brushData = (GLubyte *) malloc(image_width*image_height*4);
	
	CGContextRef cgctx = CGBitmapContextCreate(brushData, image_width, image_height, 8, image_width*4, CGImageGetColorSpace(scaledImgRef), kCGImageAlphaPremultipliedLast);
	CGContextDrawImage(cgctx, CGRectMake(0,0,image_width,image_height), scaledImgRef);
	CGContextRelease(cgctx);
	
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, brushData);
	

	if(![artoolkit wasInit]) {
		[artoolkit initNyARTwithWidth:image_width andHeight:image_height];
		[artoolkit getProjectionMatrix:ogl_projection_matrix];
	}
	
	[artoolkit detectMarkerWithIamge:scaledImgRef intoMatrix:ogl_camera_matrix];	
	
	free(brushData);
	CGImageRelease(img);
}

- (void) flushARToolKitMatrix {
	for(int i=0;i<16;i++) {
		ogl_projection_matrix[i] = 0;
		ogl_camera_matrix[i] = 0;
	}
}


- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
	
    [(EAGLView *)self.view setFramebuffer];
   
	static const GLfloat spriteVertices[] =  {0,0,0,   1,0,0,   0,1,0   ,1,1,0};	
	
	static const GLfloat textureCoords[] = {
			0,0.9735f,   0.625f,0.9735f,   0,0,   0.625f,0,	
	};
	
    static const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	

	const GLfloat polygonArray[] = {-1,-1,0, 1,-1,0, -1,1,0, 1,1,0 };
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
        glMatrixMode(GL_PROJECTION);
		glPushMatrix();
        glLoadIdentity();
		glOrthof(0, 1, 0, 1, -1000, 1);
        glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
        glLoadIdentity();
				
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, spriteTexture);
		
		glTranslatef(0.0f, 0.0f, 0.0f);
           
        glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
        glEnableClientState(GL_VERTEX_ARRAY);
		
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
				
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
				
				
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(ogl_projection_matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(ogl_camera_matrix);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, squareColors);
		glVertexPointer(3 ,GL_FLOAT, 0, polygonArray);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	    
    [(EAGLView *)self.view presentFramebuffer];
//	[self flushARToolKitMatrix];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


@end
