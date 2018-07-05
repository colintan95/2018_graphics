#version 400

noperspective in vec3 gs_edge_dist;

layout(location = 0) out vec4 fs_color;

uniform float line_width;
uniform vec4 line_color;

uniform vec4 quad_color;

void main() {
     float d = min(gs_edge_dist.x, gs_edge_dist.y);
     d = min(d, gs_edge_dist.y);
     float mix_val = smoothstep(line_width - 1, line_width + 1, d);

     fs_color = mix(quad_color, line_color, mix_val);
}