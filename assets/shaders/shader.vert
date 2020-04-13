attribute mediump vec4 vertex;
uniform mediump mat4 camera;

void main(void) { gl_Position = camera * vertex; }