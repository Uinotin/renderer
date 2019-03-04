#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in float height;
layout(location = 0) out vec4 oColor;
void main(void)
{
  oColor = vec4(max(-height, 0), min(1.0 - height, 1.0), height*height, 1.0);
}
