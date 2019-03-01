#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 oColor;
void main(void)
{
  float sine = sin(10*texcoord.x+cos(texcoord.y * 10))*sin(10*texcoord.y+cos(texcoord.x * 10));
  oColor = vec4(sine, sine, 1.0, 1.0);
}
