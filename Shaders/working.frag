#version 130


varying vec3 normal;
uniform sampler2D texture;
uniform vec2 texture_coord;

void main()
{
    //vec3 normal_normal = normalize(normal);

    //gl_FragColor = vec4(normal_normal, 1.f);

    gl_FragColor = texture2D(texture, texture_coord.st);

    /*gl_FragColor = vec4(1 * (0.3 + sin(x*2)*0.2),
                        0.7 * (0.3 + sin(x*2)*0.2),
                        0.7 * (0.3 + sin(x*2)*0.2),
                        1);*/

}

