#version 120

#define ERROR_MULTIPLIER 10.f

uniform sampler2D targetImgTexture;
uniform sampler2D triangleImgTexture;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.f - coord.y;

    vec4 targetImgVal = texture2D(targetImgTexture, coord);
    vec4 triangleImgVal = texture2D(triangleImgTexture, coord);

    vec3 error = targetImgVal.xyz * targetImgVal.w - triangleImgVal.xyz * triangleImgVal.w;

    vec3 squaredError = error*error;

    gl_FragColor = vec4(squaredError * ERROR_MULTIPLIER, 1.f);
}