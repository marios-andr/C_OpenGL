#version 460 core
out vec4 FragColor;

in vec3 outColor;
in vec2 texCoord;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

void main()
{
    //FragColor = texture(uTexture, texCoord);
    FragColor = mix(texture(uTexture0, texCoord), texture(uTexture1, texCoord), 0.2);
}