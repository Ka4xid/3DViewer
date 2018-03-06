#version 130


uniform sampler2D texture;
in vec2 Vert_uv;
out vec4 outColor;


void main()
{
    outColor = texture2D(texture, Vert_uv);
}
