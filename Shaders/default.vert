#version 130

void main()
{
    vec4 v = vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z, 1);
    gl_Position = gl_ModelViewProjectionMatrix * v;
}

