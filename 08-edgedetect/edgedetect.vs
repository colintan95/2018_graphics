#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

out vec2 vs_texcoord;

void main() {
     vs_texcoord = texcoord;

     gl_Position = vec4(position, 1.0);
}