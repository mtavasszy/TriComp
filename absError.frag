#version 460

#define ERROR_MULTIPLIER 10.f


uniform sampler2D targetImgTexture;
uniform sampler2D triangleImgTexture;
uniform vec2 screenDim;

void main()
{
    vec4 targetImgVal = texture2D(targetImgTexture, gl_FragCoord.xy / screenDim);
    vec4 triangleImgVal = texture2D(triangleImgTexture, gl_FragCoord.xy / screenDim);

    vec3 error = targetImgVal.xyz - triangleImgVal.xyz * triangleImgVal.w;

    //vec3 squaredError = error*error;

//    gl_FragColor = vec4(squaredError * ERROR_MULTIPLIER, 1.f);
    gl_FragColor = vec4(abs(error), 1.f);
    //gl_FragColor = vec4(targetImgVal.xyz, 1.f);
}

//#version 460
//
//uniform sampler2D targetImgTexture;
//
//void main()
//{
//    // lookup the pixel in the texture
//    vec4 pixel = texture2D(targetImgTexture, gl_FragCoord.xy / vec2(200,200));
//
//    // multiply it by the color
//    gl_FragColor = pixel;
//}