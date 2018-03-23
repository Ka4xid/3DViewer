#version 130


in vec3 Vert_Pos;
in vec3 Vert_Normal;
in vec2 Vert_Texel;
in vec3 Obj_Pos;
in float time;

out vec2 Vert_uv;
out vec3 Normal;

out vec3 Pos;
out vec3 V_Pos;


void main()
{
    //gl_ProjectionMatrix
    gl_Position = gl_ModelViewProjectionMatrix * vec4(Vert_Pos,1);

    V_Pos = Vert_Pos;
    Pos = Obj_Pos;

    Vert_uv = vec2(Vert_Texel);

    Normal = Vert_Normal;
}
