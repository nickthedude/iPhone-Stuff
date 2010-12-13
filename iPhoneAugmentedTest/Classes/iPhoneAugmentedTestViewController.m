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
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation iPhoneAugmentedTestViewController

@synthesize animating, context, displayLink;

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
	
	[self loadTextureFromImageAndScan];
	

}

- (void) loadTextureFromImageAndScan {
	// load Image Data into a Texture
	
	CGImageRef img = [[UIImage imageNamed:@"scaledImg.png"] CGImage];
	
	int width = CGImageGetWidth(img);
	int height = CGImageGetHeight(img);
	
	GLubyte* brushData = (GLubyte *) malloc(width*height*4);
	
	CGContextRef cgctx = CGBitmapContextCreate(brushData, width, height, 8, width*4, CGImageGetColorSpace(img), kCGImageAlphaPremultipliedLast);
	CGContextDrawImage(cgctx, CGRectMake(0,0,width,height), img);
	CGContextRelease(cgctx);
		
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, brushData);
		
	// Scan the CGIImage for the marker
	
	if(![artoolkit wasInit]) {
		[artoolkit initNyARTwithWidth:width andHeight:height];
		[artoolkit getProjectionMatrix:ogl_projection_matrix];
	}
	
	[artoolkit detectMarkerWithIamge:img intoMatrix:ogl_camera_matrix];
	free(brushData);
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
    
    // Replace the implementation of this method to do your own custom drawing.
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
		-1.0f,1.0f,
        1.0f, 1.0f
        
    };
    
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
    
    if ([context API] == kEAGLRenderingAPIOpenGLES2)
    {
	     // Use shader program.
        glUseProgram(program);
        
          // Update attribute values.
	
        glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);
		
        glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, 0, squareColors);
        glEnableVertexAttribArray(ATTRIB_COLOR);
        
        // Validate program before drawing. This is a good check, but only really necessary in a debug build.
        // DEBUG macro must be defined in your debug configurations if that's not already the case.
#if defined(DEBUG)
        if (![self validateProgram:program])
        {
            NSLog(@"Failed to validate program: %d", program);
            return;
        }
#endif
    }
    else
    {
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
				
    
	}
    

    
    [(EAGLView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Attach vertex shader to program.
    glAttachShader(program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_COLOR, "color");
    
    // Link program.
    if (![self linkProgram:program])
    {
        NSLog(@"Failed to link program: %d", program);
        
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }
        
        return FALSE;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}

@end
