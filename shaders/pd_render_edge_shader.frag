#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform vec2 texelSize;
uniform float edgeSampleRadius;
uniform float edgeDepthThreshold;
uniform float edgeNormalThreshold;

out vec4 finalColor;

float sampleDepth(vec2 offset)
{
    return texture(depthTexture, fragTexCoord + offset * texelSize * edgeSampleRadius).r;
}

vec3 sampleNormal(vec2 offset)
{
    return texture(normalTexture, fragTexCoord + offset * texelSize * edgeSampleRadius).xyz;
}

void main()
{
    vec4 baseColor = texture(texture0, fragTexCoord);
    float depthLeft = sampleDepth(vec2(-1.0, 0.0));
    float depthRight = sampleDepth(vec2(1.0, 0.0));
    float depthUp = sampleDepth(vec2(0.0, -1.0));
    float depthDown = sampleDepth(vec2(0.0, 1.0));
    float depthUpLeft = sampleDepth(vec2(-1.0, -1.0));
    float depthUpRight = sampleDepth(vec2(1.0, -1.0));
    float depthDownLeft = sampleDepth(vec2(-1.0, 1.0));
    float depthDownRight = sampleDepth(vec2(1.0, 1.0));

    vec3 normalLeft = sampleNormal(vec2(-1.0, 0.0));
    vec3 normalRight = sampleNormal(vec2(1.0, 0.0));
    vec3 normalUp = sampleNormal(vec2(0.0, -1.0));
    vec3 normalDown = sampleNormal(vec2(0.0, 1.0));
    vec3 normalUpLeft = sampleNormal(vec2(-1.0, -1.0));
    vec3 normalUpRight = sampleNormal(vec2(1.0, -1.0));
    vec3 normalDownLeft = sampleNormal(vec2(-1.0, 1.0));
    vec3 normalDownRight = sampleNormal(vec2(1.0, 1.0));

    float depthGradientX = (depthUpRight + 2.0 * depthRight + depthDownRight) -
                           (depthUpLeft + 2.0 * depthLeft + depthDownLeft);
    float depthGradientY = (depthDownLeft + 2.0 * depthDown + depthDownRight) -
                           (depthUpLeft + 2.0 * depthUp + depthUpRight);

    vec3 normalGradientX = (normalUpRight + 2.0 * normalRight + normalDownRight) -
                           (normalUpLeft + 2.0 * normalLeft + normalDownLeft);
    vec3 normalGradientY = (normalDownLeft + 2.0 * normalDown + normalDownRight) -
                           (normalUpLeft + 2.0 * normalUp + normalUpRight);

    float depthEdge = length(vec2(depthGradientX, depthGradientY));
    float normalEdge = length(normalGradientX) + length(normalGradientY);
    float depthMask = smoothstep(edgeDepthThreshold * 0.75, edgeDepthThreshold * 1.85, depthEdge);
    float normalMask = smoothstep(edgeNormalThreshold * 0.75, edgeNormalThreshold * 1.85, normalEdge);
    float edgeMask = clamp(max(depthMask, normalMask), 0.0, 1.0);
    float depthSensitivity = clamp((0.08 - edgeDepthThreshold) / 0.079, 0.0, 1.0);
    float normalSensitivity = clamp((1.0 - edgeNormalThreshold) / 0.98, 0.0, 1.0);
    float edgeStrength = mix(0.05, 1.0, max(depthSensitivity, normalSensitivity));

    vec3 edgeColor = vec3(0.11, 0.12, 0.16);
    finalColor = vec4(mix(baseColor.rgb, edgeColor, edgeMask * edgeStrength), baseColor.a);
}
