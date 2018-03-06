#version 130

varying vec2 vert_texture_coord;
uniform sampler2D input_texture;

void main()
{
    vec4 color = texture2D(input_texture, vert_texture_coord);

    //vec4(1,1,1,1);
    gl_FragColor = vec4(vert_texture_coord.x, vert_texture_coord.y, 1, 1);
}
