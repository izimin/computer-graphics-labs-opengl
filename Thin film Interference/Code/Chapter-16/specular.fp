!!ARBfp1.0
#
# simple specular shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#     fragment.texcoord [1] - h
#
#	  texture [0] - bump map
#

ATTRIB	h         = fragment.texcoord [1];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 24;
PARAM	specColor = { 1, 1, 0 };

TEMP	n, ln, hn, color, h2, l2, n2;

TEX	n, fragment.texcoord [0], texture [0], 2D;	# get normal perturbation
MAD	n, n, two, -one;
												# normalize n
DP3		n2.w, n, n;
RSQ		n2.w, n2.w;
MUL		n2.xyz, n, n2.w;

												# normalize h
DP3		h2.w, h, h;
RSQ		h2.w, h2.w;
MUL		h2.xyz, h, h2.w;

			# compute (n,h) ^ shininess
DP3_SAT	hn.a, n2, h2;							# compute max ( (n,h), 0 )
LG2		hn.a, hn.a;
MUL		hn.a, hn.a, shininess;
EX2		hn.a, hn.a;								# compute max ((h,n), 0) ^ shininess

			# return color
MUL		color, specColor, hn.a;					# return amb + specColor*max((h,n),0)^shininess
ADD_SAT	result.color, color, amb;

END
