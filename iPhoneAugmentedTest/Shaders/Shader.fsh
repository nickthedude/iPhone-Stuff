//
//  Shader.fsh
//  iPhoneAugmentedTest
//
//  Created by Kamen Dimitrov on 12/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
