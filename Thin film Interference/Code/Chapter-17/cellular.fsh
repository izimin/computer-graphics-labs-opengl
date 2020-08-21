varying vec3	lt;
varying	vec3	ht;

uniform sampler2D cellMap;

const vec3  one   = vec3 ( 1.0 );

vec2	dcell ( in vec2 tex )
{
	float	vx1 = texture2D ( cellMap, tex + vec2 ( 0.01, 0.0 ) ).x;
	float	vx2 = texture2D ( cellMap, tex - vec2 ( 0.01, 0.0 ) ).x;
	float	vy1 = texture2D ( cellMap, tex + vec2 ( 0.0, 0.01 ) ).x;
	float	vy2 = texture2D ( cellMap, tex - vec2 ( 0.0, 0.01 ) ).x;

	vec2	res = 4.0*vec2 ( vx1*vx1 - vx2*vx2, vy1*vy1 - vy2*vy2 );

	return res;
}

void main(void)
{

    vec2     tex     = 2.0*vec2 ( 0.5, 1.0 )*gl_TexCoord [0].xy;
    vec3	 cel     = texture2D ( cellMap, tex ).xyz;
    vec3	 cel2    = 0.5*texture2D ( cellMap, 2.0*tex ).xyz;

cel += cel2;

    vec2     dn      = dcell ( tex );
	vec3	 n       = vec3 ( 0, 0, 1 );
	vec3	 t       = vec3 ( 1, 0, 0 );
	vec3	 b       = vec3 ( 0, 1, 0 );
    vec3     nn      = normalize ( n + dn.x * t + dn.y * b );
    vec3     l2      = normalize ( lt );
    vec3     h2      = normalize ( ht );
    float    diffuse = 0.4 + max ( 0.0, dot ( nn, l2 ) );
    float    spec    = pow ( max ( 0.0, dot ( nn, h2 ) ), 30.0 );

	gl_FragColor = pow(1.0-cel.x,2.0)*diffuse * vec4 ( 1, 0, 0, 1 ) + vec4 ( 1, 1, 1, 1 ) * spec;
}