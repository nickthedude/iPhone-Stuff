//
//  Shader.vsh
//  iPhoneAugmentedTest
//
//  Created by Kamen Dimitrov on 12/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

attribute vec4 position;
attribute vec4 color;

varying vec4 colorVarying;

uniform float translate;

void main()
{
    gl_Position = position;
	colorVarying = color;
}
