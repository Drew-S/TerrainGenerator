attribute mediump vec4 vertex;
attribute mediump vec2 uv;
uniform mediump mat4 camera;
uniform mediump mat4 model;

void main(void) { gl_Position = camera * model * vertex; }