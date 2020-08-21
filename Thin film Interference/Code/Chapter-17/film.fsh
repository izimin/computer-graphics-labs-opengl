//
// Old-film vertex shader as a post-processing effect
//
// Effects:
//     1. shudder of source texture with the time
//     2. brightness changes
//     3. applying texture with scratches
//     4. applying texture with blips
//

uniform float	time;
varying	vec2	shudder;
varying	float	brightness;
varying	vec2	blipCoord;
varying vec2	scratchCoord;

uniform	sampler2D	mainTex;
uniform sampler2D	scratchTex;
uniform sampler2D	blipTex;

void main (void)
{
	const	vec3	luminance = vec3 ( 0.3, 0.59, 0.11 );

	vec4	color   = texture2D ( mainTex,    gl_TexCoord [0].xy + shudder );
	vec4	scratch = texture2D ( scratchTex, 2.0*scratchCoord );
	vec4	blip    = texture2D ( blipTex,    blipCoord    );

	gl_FragColor   = vec4 ( dot ( color.rgb, luminance ) ) * brightness * scratch * blip;
	gl_FragColor.a = 1.0;
}
