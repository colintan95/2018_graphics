#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

out vec3 vert_color;

uniform mat4x4 model_mat;
uniform mat4x4 view_mat;
uniform mat4x4 proj_mat;

void main() {
     gl_Position = proj_mat * view_mat * model_mat * position;
     vert_color = color;
}