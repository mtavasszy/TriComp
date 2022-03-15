#version 330

uniform sampler2D mipmapTexture;
uniform int maxMipmapLvl;

void main()
{
    vec3 mipmapVal = texture2D(mipmapTexture, gl_FragCoord.xy, maxMipmapLvl).xyz;
    gl_FragColor = vec4(mipmapVal, 1);
}