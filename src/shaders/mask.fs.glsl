#version 330

uniform vec4 color;
uniform sampler2D texture;

in vec2 uv_frag;

void main() {
    float alpha = texture2D(texture, uv_frag).r;
    gl_FragColor = vec4(color.rgb, color.a*alpha);
}
