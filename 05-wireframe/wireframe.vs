#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vs_eyepos;
out vec3 vs_normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 normal_mat;

void main() {
     vs_eyepos = (view_mat * model_mat * vec4(position, 1.0)).xyz;
     vs_normal = normalize((normal_mat * vec4(normal, 1.0)).xyz);

     gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0);
}