varying vec3 L;
varying vec3 N;
varying vec3 P;

void main (void)
{
  vec3 N2    = normalize(N);
  vec3 L2    = normalize(L);
  vec4 Idiff = vec4 ( 0.5, 0.0, 0.0, 1.0 ) * max(dot(N2,L2), 0.0);

  gl_FragColor = /*gl_FrontLightModelProduct.sceneColor; + */Idiff;
}

