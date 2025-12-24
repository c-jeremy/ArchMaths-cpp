#version 330 core
uniform vec4 color;
uniform vec3 lightDir;
in vec3 vNormal;
in vec3 vPos;
out vec4 FragColor;

void main() {
    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, normalize(lightDir)), 0.0);
    float ambient = 0.3;
    float lighting = ambient + (1.0 - ambient) * diff;
    FragColor = vec4(color.rgb * lighting, color.a);
}
