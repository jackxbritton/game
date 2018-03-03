#version 330

layout (location = 0) in vec2 point;
layout (location = 1) in vec2 uv;
uniform mat3 transform;

out vec2 uv_frag;

void main() {

    vec3 transformed_point = vec3(point, 1.0) * transform;
    gl_Position = vec4(transformed_point.xy, 0.0, 1.0);

    uv_frag = uv;
    uv_frag.y = 1.0 - uv_frag.y;
}
