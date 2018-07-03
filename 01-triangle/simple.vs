#version 330 core

layout(location = 0) in vec3 in_position;

void main() {
     gl_Position.xyz = in_position;
     gl_Position.w = 1.0;
}