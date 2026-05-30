#version 330

in vec2 fragTexCoord;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform vec2 texelSize;
uniform float edgeDepthThreshold;
uniform float edgeNormalThreshold;

out vec4 finalColor;

float sampleDepth(vec2 offset)
{
    return texture(depthTexture, fragTexCoord + offset * texelSize).r;
}

vec3 sampleNormal(vec2 offset)
{
    return texture(normalTexture, fragTexCoord + offset * texelSize).xyz;
}

void main()
{
    vec4 baseColor = texture(colorTexture, fragTexCoord);
    float depthCenter = sampleDepth(vec2(0.0, 0.0));
    vec3 normalCenter = sampleNormal(vec2(0.0, 0.0));

    float depthDelta = 0.0;
    depthDelta = max(depthDelta, abs(depthCenter - sampleDepth(vec2(1.0, 0.0))));
    depthDelta = max(depthDelta, abs(depthCenter - sampleDepth(vec2(-1.0, 0.0))));
    depthDelta = max(depthDelta, abs(depthCenter - sampleDepth(vec2(0.0, 1.0))));
    depthDelta = max(depthDelta, abs(depthCenter - sampleDepth(vec2(0.0, -1.0))));

    float normalDelta = 0.0;
    normalDelta = max(normalDelta, length(normalCenter - sampleNormal(vec2(1.0, 0.0))));
    normalDelta = max(normalDelta, length(normalCenter - sampleNormal(vec2(-1.0, 0.0))));
    normalDelta = max(normalDelta, length(normalCenter - sampleNormal(vec2(0.0, 1.0))));
    normalDelta = max(normalDelta, length(normalCenter - sampleNormal(vec2(0.0, -1.0))));

    float edgeMask = step(edgeDepthThreshold, depthDelta) + step(edgeNormalThreshold, normalDelta);
    edgeMask = clamp(edgeMask, 0.0, 1.0);

    vec3 edgeColor = vec3(0.18, 0.18, 0.22);
    finalColor = vec4(mix(baseColor.rgb, edgeColor, edgeMask * 0.35), baseColor.a);
}
