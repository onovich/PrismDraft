#version 330

in vec3 fragNormal;
in vec4 fragColor;

uniform vec3 lightDirection;
uniform float darkIntensity;

out vec4 finalColor;

void main()
{
    float lightValue = dot(normalize(fragNormal), normalize(-lightDirection));
    float intensity = lightValue > 0.0 ? 1.0 : darkIntensity;
    finalColor = vec4(fragColor.rgb * intensity, fragColor.a);
}
