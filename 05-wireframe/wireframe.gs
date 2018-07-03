#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 gs_eyepos;
out vec3 gs_normal;
noperspective out vec3 gs_edge_dist;

in vec3 vs_eyepos[];
in vec3 vs_normal[];

uniform mat4 viewport_mat;

void main() {
     vec3 p0 = vec3(viewport_mat * (gl_in[0].gl_Position /
                                    gl_in[0].gl_Position.w));
     vec3 p1 = vec3(viewport_mat * (gl_in[1].gl_Position /
                                    gl_in[1].gl_Position.w));
     vec3 p2 = vec3(viewport_mat * (gl_in[2].gl_Position /
                                    gl_in[2].gl_Position.w));
                                    
     float a = length(p1 - p2);
     float b = length(p2 - p0);
     float c = length(p1 - p0);
     float alpha = acos((b * b + c * c - a * a) / (2.0 * b * c));
     float beta  = acos((a * a + c * c - b * b) / (2.0 * a * c));
     float ha = abs(c * sin(beta));
     float hb = abs(c * sin(alpha));
     float hc = abs(b * sin(alpha));

     gs_edge_dist = vec3(ha, 0, 0);
     gs_eyepos = vs_eyepos[0];
     gs_normal = vs_normal[0];
     gl_Position = gl_in[0].gl_Position;
     EmitVertex();

     gs_edge_dist = vec3(0, hb, 0);
     gs_eyepos = vs_eyepos[1];
     gs_normal = vs_normal[1];
     gl_Position = gl_in[1].gl_Position;
     EmitVertex();

     gs_edge_dist = vec3(0, 0, hc);
     gs_eyepos = vs_eyepos[2];
     gs_normal = vs_normal[2];
     gl_Position = gl_in[2].gl_Position;
     EmitVertex();
     EndPrimitive();
}