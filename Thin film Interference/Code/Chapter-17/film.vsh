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

void main(void)
{
	shudder      = 0.0015 * vec2 ( sin ( time * 111.0 ), cos ( time * 157.3 ) );
	brightness   = clamp ( 3.0 * sin ( time * 5.7 ), 0.7, 1.0 );
	blipCoord    = 3.0 * gl_MultiTexCoord0.xy + vec2 ( sin ( 20.0 * time ) + cos ( 7.0 * time ), cos ( 35.0 * time ) + sin ( 13.0 * time ) );
	scratchCoord = gl_MultiTexCoord0.xy + vec2 ( 0.01 * cos ( 1234.0 * time ), 0.2 * sin ( 157.3 * time ) );

	gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord [0] = gl_MultiTexCoord0;
}

