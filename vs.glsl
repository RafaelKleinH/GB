#version 410

layout (location = 0) in vec3 vp;
layout (location = 1) in vec2 vt;

out vec2 texture_coords;

// X no plano
uniform float tx;
// Y no plano
uniform float ty;
// Z...
uniform float layer_z;

void main () {
	texture_coords = vt;
	gl_Position = vec4 (vp.x + tx, vp.y + ty, layer_z, 1.0);
}
