#version 330

uniform sampler2D texture;

in vec2 uv_frag;

void main() {
    float alpha = texture2D(texture, uv_frag).r;
    gl_FragColor = vec4(0, 0, 0, alpha);
    //gl_FragColor = vec4(0, 0.3, 0.5, 1);
}
