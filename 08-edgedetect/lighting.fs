#version 400

in vec3 vs_eyepos;
in vec3 vs_normal;

layout(location = 0) out vec4 fs_color;

uniform vec3 light_pos;
uniform vec3 diffuse_param;
uniform vec3 ambient_param;
uniform vec3 specular_param;
uniform float shininess;

uniform mat4 view_mat;

vec3 calc_light(vec3 light_pos, vec3 position, vec3 normal) {
     vec3 light_unit = normalize(light_pos - position);
     vec3 normal_unit = normalize(normal);
     vec3 position_unit = normalize(-position);
     return (0.1 * ambient_param  +
             diffuse_param  * max(dot(position_unit, normal_unit), 0.0) +
             specular_param * pow(max(dot(light_unit, normal_unit), 0.0),
                            shininess));
}

void main() {
     vec3 light_eyepos = (view_mat * vec4(light_pos, 1.0)).xyz;
     vec3 light_color  = calc_light(light_eyepos, vs_eyepos, vs_normal);

     fs_color = vec4(light_color, 1.0);
}