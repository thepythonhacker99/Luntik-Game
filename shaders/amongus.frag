uniform sampler2D texture;
uniform vec3 bodyColor;

void main()
{
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);

    if (color.rgb == vec3(1, 1, 1)) {
        color.rgb = bodyColor;
    }

    // Output the color
    gl_FragColor = vec4(color);
    // gl_FragColor = vec4(0, 0, 0, 1);
}