#version 450
#extension GL_ARB_separate_shader_objects : enable
vec2 positions[4] = vec2[](
  vec2(-0.5, -0.5),
  vec2(0.5, -0.5),
  vec2(-0.5, 0.5),
  vec2(0.5, 0.5)
);
layout(push_constant) uniform Push {
  mat4 matrix;
} push;

layout(location = 0)out vec2 oTexcoord;

void main(void)
{
  oTexcoord = positions[gl_VertexIndex] + vec2(0.5, 0.5);
  gl_Position = push.matrix * vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
