#version 330
uniform mediump vec4 color;

// uniform mediump vec3 light_dir;
// uniform mediump float light_intensity;

vec3 normal = vec3(0.0, 1.0, 0.0);

void main(void) { gl_FragColor = color; }