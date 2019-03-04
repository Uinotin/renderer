#version 450
#extension GL_ARB_separate_shader_objects : enable
vec2 positions[4] = vec2[](
  vec2(-1, -0.5),
  vec2(-1, 1),
  vec2(1, 1),
  vec2(1, -0.5)
);

void main(void)
{
  gl_Position = vec4(positions[gl_VertexIndex], 0, 1.0);
}
