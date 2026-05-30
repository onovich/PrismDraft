#version 330

in vec3 fragNormal;

out vec4 finalColor;

void main()
{
    vec3 encodedNormal = normalize(fragNormal) * 0.5 + 0.5;
    finalColor = vec4(encodedNormal, 1.0);
}
