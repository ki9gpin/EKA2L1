#version 330

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

out vec2 r_texcoord;
out vec4 r_color;

uniform mat4 u_proj;
uniform mat4 u_model;
uniform vec4 u_color;
uniform float u_flip;

void main() {
    gl_Position = u_proj * u_model * vec4(in_position, 0.0, 1.0);
    gl_Position.y *= u_flip;
    r_texcoord = in_texcoord;
    r_color = u_color;
}