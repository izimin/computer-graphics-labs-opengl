!!ARBfp1.0
#
# simple glow shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#     fragment.texcoord [1] - v
#
#	  texture [0] - bump map
#

ATTRIB	v         = fragment.texcoord [1];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 40;
PARAM	specColor = { 1, 1, 0 };

TEMP	n, hn, color, v2, n2, temp;

TEX	n, fragment.texcoord [0], texture [0], 2D;	# get normal perturbation
MAD	n, n, two, -one;
												# normalize n
DP3		n2.w, n, n;
RSQ		n2.w, n2.w;
MUL		n2.xyz, n, n2.w;

												# normalize h
DP3		v2.w, v, v;
RSQ		v2.w, v2.w;
MUL		v2.xyz, v, v2.w;

			# compute 1 - (n,v) ^ 2
DP3		hn.a, n2, v2;
MUL		hn.a, hn.a, hn.a;
ADD		hn.a, one, -hn.a;
MUL_SAT	hn.a, hn.a, 1.1;						# overbright it a bit

LG2		hn.a, hn.a;
MUL		hn.a, hn.a, shininess;
EX2		hn.a, hn.a;								# compute max ((h,n), 0) ^ shininess

			# return color
MUL		color, specColor, hn.a;					
MOV		result.color, color;

END
