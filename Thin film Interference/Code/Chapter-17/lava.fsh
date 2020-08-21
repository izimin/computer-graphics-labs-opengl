//
// Lava fragment shader
//

uniform	sampler3D	noiseMap;
uniform	sampler1D	fireMap;
varying	vec3		texCoord;
uniform	vec4		time;

void main (void)
{
	const	vec3	stoneColor1 = vec3 ( 0.05, 0.03, 0.0 );
	const	vec3	stoneColor2 = vec3 ( 0.3,  0.0,  0.0 );

								// compute turbulence
	vec3	arg  = 0.1 * texCoord + vec3 ( 0.346, 0.745, 0.918 )*0.002*time.x;
	vec3	arg1 = 0.2 * texCoord + vec3 ( 0.137, 0.357, 0.813 );

	vec3	arg2 = 4.0 * arg + vec3 ( 0.537, 0.953, 0.131 );
	vec3	arg3 = 12.0 * arg + vec3 ( 0.463, 0.377, 0.543 );
	vec3	n1   = texture3D ( noiseMap, arg1 ).rgb / 1.0;
	vec3	n2   = texture3D ( noiseMap, arg2 ).rgb / 2.0;
	vec3	n3   = texture3D ( noiseMap, arg3 ).rgb / 4.0;
	vec3	turb = n1 + n2 + n3;

	float	f          = smoothstep ( 0.0, 1.0, turb.y - 0.5 );
//	float	g          = smoothstep ( 0.5, 0.53, texture3D ( noiseMap, arg1 ).z );
	float	g          = step       ( 0.51, texture3D ( noiseMap, arg1 ).z );
	vec3	lavaColor  = texture1D  ( fireMap, smoothstep ( 0.0, 1.0, turb.x - 0.4 ) ).rgb;
	vec3	stoneColor = mix        ( stoneColor1, stoneColor2, f );

//	gl_FragColor = vec4 ( stoneColor, 1.0 );
//	gl_FragColor = vec4 ( lavaColor, 1.0 );
	gl_FragColor = vec4 ( mix ( stoneColor, lavaColor, g ), 1.0 );
}
