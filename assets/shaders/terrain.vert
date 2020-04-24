#version 330

attribute highp vec4 vertex;

// Combined projection and view matrix
uniform highp mat4 camera;

// Object transform matrix
uniform highp mat4 model;

// Height scale factor
// TODO: Include UI option for this
float scale = 0.5;

uniform highp sampler2D height_map;

// Outputs for the fragment shader pipeline
out highp vec3 frag_pos;
out highp vec2 frag_uv;
out highp mat4 frag_model;

void main(void) {
  // Get the uv position, (since the vertices are between 0 and 1 in the x and z
  // directions, the uv is generatable from the x and z components)
  vec2 uv = vec2(vertex.x, vertex.z);

  // Get the height value from the height map
  vec4 height = texture2D(height_map, uv);
  vec4 vert = vec4(vertex.x, height.z * scale, vertex.z, 1.0);

  // Apply the transformations to the vertex
  gl_Position = camera * model * vert;

  // Apply fragment outputs
  frag_uv = uv;
  frag_pos = vec3(model * vert);
  frag_model = model;
}