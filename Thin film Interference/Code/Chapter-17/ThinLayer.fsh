//
// Thin-layer effect (based on NVidia shader)
//

uniform sampler2D diffuseMap;
uniform sampler1D fringeMap;

varying vec3 diffColor;
varying vec3 specColor;
varying vec2 viewDepth;

void main()
{
    					// diffuse material color
    vec3 diffMaterial = texture2D ( diffuseMap, gl_TexCoord [0].xy ).rgb;

    					// lookup fringe value based on view depth
    vec3 fringeColor = texture1D ( fringeMap, viewDepth.x ).rgb;

    					// modulate specular lighting by fringe color
    					// and combine with regular lighting
    gl_FragColor = vec4 ( diffColor * diffMaterial + fringeColor * specColor, 1.0 );
}
