#version 450 core

out vec4 FragColor;

uniform int rules_size;
uniform int dim_x;
uniform usamplerBuffer rules;
uniform usamplerBuffer flips;

layout(RGBA8) uniform image2D ret;

bool check(uint rule, uint flip, uint alloc) {
	alloc = rule & (alloc ^ flip);
	return alloc > 0;
}

void main()
{
	int ycoord = int(gl_FragCoord.y);
	int xcoord = int(gl_FragCoord.x);

	uint val = ycoord * dim_x + xcoord;

	for(int i=0; i<rules_size; i++) {
		if(!check(texelFetch( rules, i ).r, texelFetch( flips, i ).r, val)) return;
	}
	imageStore(ret, ivec2(0.0f), vec4(1.0f));
}