varying	vec3 lt;
varying	vec3 ht;

void main (void)
{
	vec3	nt   = vec3 ( 0.0, 0.0, 1.0 );
	vec3	l2   = normalize ( lt );
	vec3	h2   = normalize ( ht );
	vec4	diff = vec4 ( 0.5, 0.0, 0.0, 1.0 ) * max ( dot ( nt, l2 ), 0.0 );
	vec4	spec = vec4 ( 0.7, 0.7, 0.0, 1.0 ) * pow ( max ( dot ( nt, h2 ), 0.0 ), 20.0 );

	gl_FragColor = diff + spec;
}
