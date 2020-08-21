!!ARBfp1.0
#
# simple specular shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#	  fragment.texcoord [1] - l in tangent space
#     fragment.texcoord [2] - v (eye) in tangent space
#
#	  texture [0] - bump map
#	  texture [1] - decal map
#

ATTRIB	l         = fragment.texcoord [1];
ATTRIB	eye       = fragment.texcoord [2];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 60;
PARAM	scale     = 0.01;
PARAM	specColor = { 1, 1, 1 };
PARAM	k         = 0.3;

TEMP	n, ln, hn, color, e2, l2, n2, temp, diffuse, dist, atten, dot1, dot2, k2;

TEX	n, fragment.texcoord [0], texture [0], 2D;	# get normal perturbation
MAD	n, n, two, -one;
												# normalize n
DP3		n2.w, n, n;
RSQ		n2.w, n2.w;
MUL		n2.xyz, n, n2.w;

												# normalize l
DP3		dist.w, l, l;
RSQ		l2.w, dist.w;
MUL		l2.xyz, l, l2.w;

												# compute distance attenuation
MUL		dist.w, dist.w, scale;
SUB_SAT	atten.w, one.w, dist.w;					# as  clamp ( 1 - (l&l) )

												# normalize eye
DP3		e2.w, eye, eye;
RSQ		e2.w, e2.w;
MUL		e2.xyz, e2, e2.w;

												# compute Minneart lighting
DP3_SAT	dot1, n2, l2;							# dot1 = (n,l)
DP3		dot2, n2, e2;
ADD		dot2, one, -dot2;						# dot2 = 1 - (n,e), allways nonnegative
MOV 	temp, dot1;
POW		dot1, dot1.x, k.x;
MUL 	dot1, dot1, temp.x;						# now we have dot1 = (n,l)*((n,l)^k*(1-(n,e))^(1-k)
ADD		k2, one, -k;
POW		dot2, dot2.x, k2.x;

												# get decal color
TEX		color, fragment.texcoord [0], texture [1], 2D;
MUL		color, color, dot1.x;
MUL		color, color, dot2.x;
MUL		result.color, color, atten.w;					# add distance attenuation

END
