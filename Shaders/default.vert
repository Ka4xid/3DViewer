#version 130


in vec3 Vert_Pos;
in vec3 Vert_Normal;
in vec2 Vert_Texel;

out vec2 Vert_uv;
out vec3 Normal;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(Vert_Pos, 1);

    Vert_uv = vec2(Vert_Texel);

    Normal = Vert_Normal;
}
