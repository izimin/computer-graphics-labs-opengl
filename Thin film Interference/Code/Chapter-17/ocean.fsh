varying	vec3 lt;
varying	vec3 ht;

uniform	vec4		time;
uniform	sampler3D	myNoise;
uniform	sampler2D	myNoise2;

void main (void)
{
	const	vec3	ambColor  = vec3 ( 0.0, 0.1, 0.1 );
	const	vec3	diffColor = vec3 ( 0.0, 1.0, 0.9 );
	const	vec3	specColor = vec3 ( 1.0, 0.8, 0.5 );
	const	vec3	up        = vec3 ( 0.0, 1.0, 0.0 );
	const	vec3	h         = vec3 ( 0.5, 0.0, 0.5 );

//	vec2	tm   = 0.1 * texture2D ( myNoise2, vec2 ( 0.3579, time.x * 0.03 ) ).xy;
// 
	vec3	arg  = vec3 ( 4.13 * gl_TexCoord [0].xy + vec2 ( 0.173*time.x, 0.1345*time.x ), time.x * 0.2 );
	vec3	arg1 = arg;
	vec3	arg2 = 4.1235 * arg;
	vec3	arg3 = 8.3173 * arg;
	vec3	n1   = (texture3D ( myNoise, arg1 ).xyz - h) * 2.0;
	vec3	n2   = (texture3D ( myNoise, arg2 ).xyz - h) / 2.0;
	vec3	n3   = (texture3D ( myNoise, arg3 ).xyz - h) / 4.0;
	vec3	n    = normalize ( n1 + n2 + n3 + up );
	vec3	l2   = normalize ( lt );
	vec3	h2   = normalize ( ht );
	float	diff = max ( dot ( n, l2 ), 0.0 );
	float	spec = pow ( max ( dot ( n, h2 ), 0.0 ), 20.0 );

	gl_FragColor = vec4 ( ambColor + diff * diffColor + spec * specColor, 1.0 );
}
