#version 330 core

layout(location = 0) in vec3 position;

uniform vec4 light_pos;

uniform mat4x4 model_mat;
uniform mat4x4 view_mat;
uniform mat4x4 proj_mat;

void main() {
     gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0);
}