// created by Andrey Vasiliev
//? #version 400

uniform sampler2D RT;
uniform int prevMipLevel;

out vec4 FragColor;
void main()
{
	ivec2 loc = ivec2(gl_FragCoord.xy);
	ivec2 size = textureSize(RT, prevMipLevel);
	
	ivec2 coordinate = clamp(loc * 2 + ivec2(loc.x & 1, loc.y & 1), ivec2(0, 0), size - ivec2(1, 1));

    FragColor = texelFetch(RT, coordinate, prevMipLevel);
}
