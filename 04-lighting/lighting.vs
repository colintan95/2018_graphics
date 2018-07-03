#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vert_color;

uniform vec3 light_position;

uniform vec3 diffuse_param;
uniform vec3 ambient_param;
uniform vec3 specular_param;
uniform float shininess;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 normal_mat;

vec3 calc_light(vec3 light_position, vec3 position, vec3 normal) {
     vec3 light_unit = normalize(light_position - position);
     vec3 normal_unit = normalize(normal);
     vec3 position_unit = normalize(-position);
     return (0.1 * ambient_param  +
           diffuse_param  * max(dot(position_unit, normal_unit), 0.0) +
         specular_param * pow(max(dot(light_unit, normal_unit), 0.0),
                            shininess));
}

void main() {
     vec4 mesh_eye_pos = view_mat * model_mat * vec4(position, 1.0);
     vec4 light_eye_pos = view_mat * vec4(light_position, 1.0);

     vec3 eye_normal = normalize((normal_mat * vec4(normal,0.0)).xyz);

     vert_color = calc_light(light_eye_pos.xyz,
                             mesh_eye_pos.xyz,
                             eye_normal);

     gl_Position = proj_mat * mesh_eye_pos;
}