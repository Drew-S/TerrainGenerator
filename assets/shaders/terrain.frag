#version 330

uniform highp sampler2D normal_map;

// Object color, terrain color
// TODO: Long term, add albedo color map
uniform highp vec3 color;

// Lighting information
uniform highp vec3 light_color;
uniform highp vec3 light_pos;
uniform highp float light_intensity;
uniform highp vec3 ambient_color;

uniform highp vec3 camera_pos;

// Updated vertex position
in highp vec3 frag_pos;
in highp vec2 frag_uv;

// Model transformation
in highp mat4 frag_model;

// in highp float height_value;

// Lighting constants
float ambient_strength = 0.25;

float diffuse_strength = 1.0;

float specular_strength = 0.1;

void main(void) {
  // Build tangent space to object space normal transform
  vec3 plane_normal = vec3(0.0, 1.0, 0.0);
  vec3 plane_tangent = vec3(1.0, 0.0, 0.0);
  vec3 plane_bi_tangent = cross(plane_tangent, plane_normal);

  plane_normal = normalize(vec3(frag_model * vec4(plane_normal, 0.0)));
  plane_tangent = normalize(vec3(frag_model * vec4(plane_tangent, 0.0)));
  plane_bi_tangent = normalize(vec3(frag_model * vec4(plane_bi_tangent, 0.0)));

  mat3 TBN = transpose(mat3(plane_tangent, plane_bi_tangent, plane_normal));

  // get normal
  vec3 normal =
      normalize(TBN * (texture2D(normal_map, frag_uv).xyz * 2.0 - 1.0));

  // Get light vector to current frag pos
  vec3 light_dir = normalize(light_pos - frag_pos);

  // Calculate ambient contribution
  vec3 ambient = ambient_strength * ambient_color;

  // Calculate diffuse contribution
  float diff = max(dot(normal, light_dir), 0.0f);
  vec3 diffuse = diffuse_strength * diff * light_color * light_intensity;

  // Calculate specular contribution
  vec3 camera_dir = normalize(camera_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 256);
  vec3 specular = specular_strength * spec * light_color * light_intensity;

  // Apply lighting
  vec3 result = (specular + diffuse + ambient) * color;
  gl_FragColor = vec4(result, 1.0);
}