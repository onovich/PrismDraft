#version 330

in vec3 fragNormal;
in vec4 fragColor;

uniform vec3 lightDirection;
uniform float darkIntensity;

out vec4 finalColor;

void main()
{
    float lightValue = dot(normalize(fragNormal), normalize(-lightDirection));
    float litBand = smoothstep(-0.08, 0.36, lightValue);
    float litIntensity = 0.82 + ((1.0 - darkIntensity) * 0.25);
    float intensity = mix(darkIntensity, litIntensity, litBand);
    finalColor = vec4(fragColor.rgb * intensity, fragColor.a);
}
