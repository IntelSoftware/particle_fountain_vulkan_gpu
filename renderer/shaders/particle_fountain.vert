#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

layout (std140, binding =0) uniform EnvModifiers {
	mat4 mvp;
} env;

void main()
{
   gl_Position = env.mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   outColor = vec3(1.0, 1.0, 1.0);
}
