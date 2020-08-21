//
// Simple anisotropic lighting vertex shader
//
// Using lookup table for diffuse and specular coefficients
//

varying	vec3 lt;
varying	vec3 ht;

uniform sampler2D tangentMap;
uniform sampler2D decalMap;
uniform sampler2D anisoTable;

void main (void)
{
	const vec4	specColor = vec4 ( 1.0 );

	vec3	tang  = normalize ( 2.0 * texture2D ( tangentMap, gl_TexCoord [0].xy ).xyz - 1.0 );
	float	dot1  = dot  ( normalize ( lt ), tang );
	float	dot2  = dot  ( normalize ( ht ), tang );
	vec2	arg   = vec2 ( dot1, dot2 );
	vec2	ds    = texture2D ( anisoTable, arg*arg ).rg;
	vec4	color = texture2D ( decalMap, gl_TexCoord [0].xy );

//	gl_FragColor = color * pow ( 1.0 - dot1*dot1, 1.0 ) + specColor * pow ( 1.0 - dot2*dot2, 30.0 );
	gl_FragColor   = color * ds.x + specColor * ds.y;
//	gl_FragColor.a = 1.0;
//	gl_FragColor = vec4 ( ds, 0.0, 1.0 );
//	gl_FragColor = vec4 ( pow ( 1.0 - dot1*dot1, 1.0 ), pow ( 1.0 - dot2*dot2, 30.0 ), 0.0, 1.0 );
}
