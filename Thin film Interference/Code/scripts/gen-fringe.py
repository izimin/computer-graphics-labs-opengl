#
# Python script to create a fringe map
#

import math, Image, PIL

lamR = 6				# 600 nm
lamG = 5				# 500 nm, should be more like 550
lamB = 4				# 400 nm. should be more like 440

p = 60   				# period of the texture, lcm (6,5,4)

						# vd is the depth of the thin film relative to the texture index
vd = 1.0 / 256.0 * p

size = 256				# size of texture
im   = Image.new ( "RGB", (size, size) )

for i in range ( size ):
	t = 2.0 * math.pi * i * vd
	r = int ( ( 0.5 * ( math.sin ( t/lamR + math.pi/2.0) + 1.0 ) ) * 255 );
	g = int ( ( 0.5 * ( math.sin ( t/lamG + math.pi/2.0) + 1.0 ) ) * 255 );
	b = int ( ( 0.5 * ( math.sin ( t/lamB + math.pi/2.0) + 1.0 ) ) * 255 );

	im.putpixel ( (i,0), (r, g, b) )

im.show ()
im.save ( "fringe.png", "png" )
