#version 400

out vec4 fs_color;

in vec2 vs_texcoord;

uniform sampler2D render_texture;
uniform int texture_width;
uniform int texture_height;

uniform float edge_threshold;

// Approximates brightness of RGB value
float luma(vec3 color) {
      return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main() {
     float dx = 1.0 / float(texture_width);
     float dy = 1.0 / float(texture_height);

     float s00 = luma(texture(render_texture,
           vs_texcoord + vec2(-dx, dy)).rgb);
     float s01 = luma(texture(render_texture,
           vs_texcoord + vec2(0, dy)).rgb);
     float s02 = luma(texture(render_texture,
           vs_texcoord + vec2(dx, dy)).rgb);
     float s10 = luma(texture(render_texture,
           vs_texcoord + vec2(-dx, 0.0)).rgb);
     float s11 = luma(texture(render_texture,
           vs_texcoord + vec2(0.0, 0.0)).rgb);
     float s12 = luma(texture(render_texture,
           vs_texcoord + vec2(dx, 0.0)).rgb);
     float s20 = luma(texture(render_texture,
           vs_texcoord + vec2(-dx, -dy)).rgb);
     float s21 = luma(texture(render_texture,
           vs_texcoord + vec2(0.0, -dy)).rgb);
     float s22 = luma(texture(render_texture,
           vs_texcoord + vec2(dx, dy)).rgb);

     float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
     float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

     float dist = sx * sx + sy * sy;

     if (dist > edge_threshold) {
          fs_color = vec4(1.0);
     }
     else {
          fs_color = vec4(0.0, 0.0, 0.0, 1.0);
     }
}