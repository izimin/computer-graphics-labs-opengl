varying	vec3 lt;
varying	vec3 ht;

uniform sampler2D bumpMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

void main (void)
{
	vec3	n    = texture2D ( bumpMap, gl_TexCoord [0].xy ).rgb;		// get normal perturbation
	vec3	nt   = normalize ( 2.0*n - 1.0 );
	vec3	l2   = normalize ( lt );
	vec3	h2   = normalize ( ht );
	float	diff = max ( dot ( nt, l2 ), 0.0 );
	float	spec = pow ( max ( dot ( nt, h2 ), 0.0 ), 30.0 );

	gl_FragColor = diff * texture2D ( diffuseMap, gl_TexCoord [0].xy ) + spec * texture2D ( specularMap, gl_TexCoord [0].xy );
}
