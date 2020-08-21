//
// Vertex shader as a post-processing effect like the one in "Third Space"
//

uniform float	time;

uniform	sampler2D	mainTex;		// image to be processed
uniform sampler2D	backTex;		// additional texture to modulate image (background texture)
uniform sampler2D	burnTex;		// texture to specify fire moving through the image

void main (void)
{
	const	vec3	luminance  = vec3 ( 0.3, 0.59, 0.11 );
	const	vec3	sepia      = vec3 ( 1.0, 0.89, 0.71 );
	const	vec3	fireColor1 = vec3 ( 0.5, 0.0,  0.0  );
	const	vec3	fireColor2 = vec3 ( 1.0, 1.0,  0.0  );
	const	float	delta     = 0.02;

	vec3	srcColor = texture2D ( mainTex, gl_TexCoord [0].xy ).rgb;
	float	burn     = texture2D ( burnTex, gl_TexCoord [0].xy ).r;
	float	t        = 1.0 - fract ( 0.075 * time );		// make it slod

	if ( t < burn )
		gl_FragColor = vec4 ( srcColor, 1.0 );
	else
	if ( t < burn + delta )
		gl_FragColor = vec4 ( mix ( fireColor1, fireColor2, (t-burn)/delta ), 1.0 );
	else
	{
		vec3	backColor  = texture2D ( backTex, 5.0 * gl_TexCoord [0].xy ).rgb;
		vec3	sepiaColor = dot ( srcColor, luminance ) * dot ( backColor, luminance ) * sepia;

		gl_FragColor = vec4 ( sepiaColor, 1.0 );
	}
}

