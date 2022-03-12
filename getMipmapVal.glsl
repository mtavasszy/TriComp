#version 120

uniform sampler2D mipmapTexture;
uniform int maxMipmapLvl;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.f - coord.y;
    vec4 mipmapVal = texture2D(mipmapTexture,coord , maxMipmapLvl);
    gl_FragColor = mipmapVal;
}