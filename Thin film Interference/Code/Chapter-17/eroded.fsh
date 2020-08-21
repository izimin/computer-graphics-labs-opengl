//
// Eroded fragment shader (a'la RenderMan's)
//

varying float		diffuse;
varying float		specular;
varying	vec2		tex;
uniform	sampler2D	myNoise;

void main (void)
{
	const	vec4	diffColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
	const	vec4	specColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );

								// compute turbulence
	vec2	arg1 = tex + vec2 ( 0.123, 0.345 );
	vec2	arg2 = 2.0 * arg1 + vec2 ( 0.537, 0.953 );
	vec2	arg3 = 4.0 * arg1 + vec2 ( 0.463, 0.543 );
	float	n1   = 2.0*abs ( texture2D ( myNoise, arg1 ).x - 0.5 ) / 1.0;
	float	n2   = 2.0*abs ( texture2D ( myNoise, arg2 ).x - 0.5 ) / 2.0;
	float	n3   = 2.0*abs ( texture2D ( myNoise, arg3 ).x - 0.5 ) / 4.0;
	float	turb = n1 + n2 + n3;

								// compute f value
	float	f     = smoothstep ( 0.1, 0.5, turb );
	float	Ka    = 0.2 * f;
	float	Kd    = 0.7 * f;
	float	Ks    = 0.7 * f;

	gl_FragColor   = Ka* diffColor + diffuse * Kd * diffColor + specular * Ks * specColor;
	gl_FragColor.a = 1.0 - f;
}
