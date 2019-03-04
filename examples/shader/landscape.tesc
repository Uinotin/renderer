#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(vertices = 64) out;

void main(void)
{
  if (gl_InvocationID == 0) {
    gl_TessLevelOuter[2] = 64;
    gl_TessLevelOuter[3] = 64;
    gl_TessLevelOuter[0] = 64;
    gl_TessLevelOuter[1] = 64;

    gl_TessLevelInner[0] = 64;
    gl_TessLevelInner[1] = 64;
  }

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}