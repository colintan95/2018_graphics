#version 400

layout(location = 0) out vec4 frag_color;

uniform vec4 line_color;

void main() {
     frag_color = line_color;
}