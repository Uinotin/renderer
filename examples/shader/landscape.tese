#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(quads, equal_spacing) in;
layout(push_constant) uniform Push {
  layout(offset = 0)mat4 matrix;
  layout(offset = 64)vec3 position;
  layout(offset = 80)vec4 rotation;
} push;

layout(location = 0) out float oHeightOffset;

void main(void)
{
  vec2 mixA = mix(gl_in[0].gl_Position.xy, gl_in[1].gl_Position.xy, gl_TessCoord.x); 
  vec2 mixB = mix(gl_in[3].gl_Position.xy, gl_in[2].gl_Position.xy, gl_TessCoord.x); 
  vec2 interpolCoord = mix(mixA, mixB, gl_TessCoord.y);
  vec2 posPlusTessCoord = vec2(push.rotation.x*interpolCoord.x
                          -push.rotation.y*interpolCoord.y, 
			  push.rotation.y*interpolCoord.x+
			  push.rotation.x*interpolCoord.y);
  posPlusTessCoord += push.position.xy;
  float heightOffset = (sin(13*(posPlusTessCoord.y))*cos(13*posPlusTessCoord.x)+sin(10*(posPlusTessCoord.x))*cos(10*posPlusTessCoord.y)); ///Magic numbers (tm)
  oHeightOffset = heightOffset;
  float height = push.position.z + 0.1*heightOffset; 
  gl_Position = push.matrix *
                vec4(interpolCoord.x,
                     push.rotation.w * height - push.rotation.z * interpolCoord.y,
                     push.rotation.z * height + push.rotation.w * interpolCoord.y,
                     1.0);
}