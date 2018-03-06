#version 130


attribute vec3 vert_pos;
attribute vec2 texture_coord;
varying vec2 vert_texture_coord;


void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vert_pos,1);

    vert_texture_coord = texture_coord;
}
