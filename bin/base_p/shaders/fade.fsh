#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D sampler;
uniform float multiplier;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    vec4 color = v_color * texture2D(sampler, v_texcoord);
    color.r *= multiplier;
    color.g *= multiplier;
    color.b *= multiplier;
    gl_FragColor = color;
}
