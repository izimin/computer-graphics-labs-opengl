//
// Simple sky vertex shader
//
// Author: Alex V. Boreskoff
//

varying	vec3 l;
varying	vec3 v;

uniform	vec4		time;
uniform	sampler3D	myNoise;
uniform	sampler2D	myNoise2;

void main (void)
{
	const	vec4	skyColorLight  = vec4 ( 0.5, 0.9, 1.0, 1.0 );
	const	vec4	skyColorNormal = vec4 ( 0.0, 0.6, 0.6, 1.0 );
	const	vec4	cloudColor     = vec4 ( 1.0, 1.0, 1.0, 1.0 );
	const	vec3	maxLight       = vec3 ( 1.0, 0.7, 2.1 );

	float	t    = 0.01 * time.x;
	vec3	tm   = vec3 ( t ) + 0.3 * texture2D ( myNoise2, vec2 ( 0.3579, 0.001 * time.x ) ).rgb;
	vec3	arg  = vec3 ( gl_TexCoord [0].xy + vec2 ( 0.246, 0.7531 ), 0.777 ) + tm;
	vec3	arg1 = 2.1397  * ( arg + vec3 ( 0.0, time.x * 0.0075, 0.22221 ) );
	vec3	arg2 = 7.9793  * ( arg + vec3 ( -time.x * 0.0058, 0.1, 0.33 ) );
	vec3	arg3 = 15.9191 * ( arg + vec3 ( 0.1313, time.x * 0.004, 0.777 ) );
	vec3	n1   = texture3D ( myNoise, arg1 ).xyz / 1.0;
	vec3	n2   = texture3D ( myNoise, arg2 ).xyz / 4.0;
	vec3	n3   = texture3D ( myNoise, arg3 ).xyz / 8.0;
	vec3	n    = n1 + n2 + n3;

	float	f        = normalize ( l ).y;
	vec4	skyColor = mix ( skyColorNormal, skyColorLight, f );
//	vec4	newCloudColor = vec4 ( cloudColor.r, (1-f*f)*cloudColor.g, (1-f*f*f)*cloudColor.b, 1.0 );

										// now compute clouds based on n
//	float	factor = 2.0 * smoothstep ( 0.32, 0.39, n.x );
	float	factor = step ( 0.67, n.x );
//	float	factor = smoothstep ( 0.35, 0.37, n.x );

	vec4	cloud = mix ( skyColor, cloudColor, factor );

//	float	f = normalize ( l ).y - normalize ( v ).y;
//	float	f = pow ( abs ( dot ( normalize ( l ), -normalize ( v ) ) ), 20 );
//	vec4	sky = mix ( skyColorNormal, skyColorLight, f );

	
	gl_FragColor = cloud;
}

