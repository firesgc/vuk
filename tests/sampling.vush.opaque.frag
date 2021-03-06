// file generated by vush compiler, from ../../tests/sampling.vush
#version 460
#pragma shader_stage(fragment)
#extension GL_GOOGLE_cpp_style_line_directive : require
#extension GL_GOOGLE_include_directive : require

struct VS_IN {
	vec3 position;
	vec2 texcoord;
};

struct VS_OUT {
	vec2 texcoord;
	vec3 col;
};


struct VP {
	mat4 view;
	mat4 projection;
};



struct FS_OUT {
	vec4 color_out;
};

layout(location = 0) out vec4 _color_out_out;

layout(std140, binding = 0) uniform _aspect_ {
	VP vp;
} _aspect;
layout(std140, binding = 1) uniform _user_ {
	mat4 model_matrix;
	vec3 col;
	vec2 ting;
} _user;
layout(binding = 1 + 1 + 0) uniform sampler2D t1;
layout(location = 0) in VS_OUT vin;

#line 29 "../../tests/sampling.vush"
FS_OUT opaque_fragment(VS_OUT vin, sampler2D t1, vec2 ting) {
	FS_OUT fout;
	fout.color_out = vec4(texture(t1, vin.texcoord).rgb, 1);
	return fout;
}

void main() {
		vec2 ting = _user.ting;
	FS_OUT _out = opaque_fragment(vin, t1, ting);
	_color_out_out = _out.color_out;
}