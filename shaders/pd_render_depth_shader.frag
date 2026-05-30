#version 330

out vec4 finalColor;

void main()
{
    float depthValue = gl_FragCoord.z;
    finalColor = vec4(depthValue, depthValue, depthValue, 1.0);
}
