#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vPos;
//layout (location = 1) in vec3 vColor;

layout (location = 0) out vec3 fragColor;

layout (std140, binding =0) uniform EnvModifiers {
	mat4 mvp;
} env;

void main()
{
   gl_Position = env.mvp * vec4(vPos.x, vPos.y, vPos.z, 1.0);
   fragColor = vec3(1.0, 1.0, 1.0);
}
