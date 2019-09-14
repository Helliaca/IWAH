#version 450 core

out vec4 FragColor;

#define RULES_CAP 100
uniform int rules_size;
uniform int dim_x;
uniform int[RULES_CAP] rules;
uniform int[RULES_CAP] flips;

layout(RGBA8) uniform image2D tex2D;

bool check(int rule, int flip, int alloc) {
	alloc = alloc ^ flip;
	alloc = rule & alloc;
	return alloc != 0;
}

void main()
{
	int ycoord = int(gl_FragCoord.y);
	int xcoord = int(gl_FragCoord.x);

	int val = ycoord * dim_x + xcoord;

	for(int i=0; i<rules_size; i++) {
		if(!check(rules[i], flips[i], val)) return;
	}
	imageStore(tex2D, ivec2(0.0f), vec4(val/255.0f));
}