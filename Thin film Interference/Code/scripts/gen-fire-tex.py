#
# Python script to create a fire (lava) map
#

import math, Image, PIL

colors = ((64, 0, 0), (220, 0, 0), (220, 220, 0), (255, 255,0))

size = 256				# size of texture
im   = Image.new ( "RGB", (size, 1) )
n    = len ( colors )

for i in range ( size ):
	j0 = (i*(n-1)) / size		# start color index
	j1 = j0 + 1			# end color index
	i0 = (j0 * size) / (n-1)
	i1 = (j1 * size) / (n-1)
	c0 = colors [j0]
	c1 = colors [j1]

	t = float ( i - i0 ) / float ( i1 - i0 )
	r = (1-t) * c0 [0] + t * c1 [0]
	g = (1-t) * c0 [1] + t * c1 [1]
	b = (1-t) * c0 [2] + t * c1 [2]

	im.putpixel ( (i,0), (r, g, b) )

im.show ()
im.save ( "fire.png", "png" )
