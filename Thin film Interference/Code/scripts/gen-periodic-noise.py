#
# script to build 2D periodic noise texture
#
# Author: Alex V. Boreskoff
#

import noise
import Image
import math

def n2D ( x, y, offs ):
	return 127*noise.noise ( offs [0] + x, offs [1] + y, offs [2] ) + 128

def drop ( x ):
	return 1.0 - math.pow ( x, 40 )

size = 256
inv  = 1.0 / size

				# create matrix for filling with noise values
noiseTable = range ( size )

for i in range ( size ):
	noiseTable [i] = range ( size )

im = Image.new ( "RGB", (size, size) )

redOffs   = ( 1.3579, 0.25672, 0.4567 )
greenOffs = ( 2.4786, 1.54567, 2.654  )
blueOffs  = ( 3.5795, 2.16713, 1.0912 )

					# fill in the noise table
for x in range ( size ):
	for y in range ( size ):
		x1 = inv * x
		y1 = inv * y
		x2 = 20 * x1
		y2 = 20 * y1

		noiseTable [x][y] = ( n2D (x2, y2, redOffs), n2D (x2, y2, greenOffs), n2D ( x2, y2, blueOffs ) )


					# now build the texture
for x in range ( size ):
	for y in range ( size ):
		x1 = inv * x
		y1 = inv * y


		xDrop = drop ( x1 )
		yDrop = drop ( y1 )


		v1 = xDrop*yDrop*noiseTable [x][y][0] + (1-xDrop)*yDrop*noiseTable [size-1-x][y][0] + xDrop*(1-yDrop)*noiseTable [x][size-1-y][0] + (1-xDrop)*(1-yDrop)*noiseTable [size-1-x][size-1-y][0]
		v2 = xDrop*yDrop*noiseTable [x][y][1] + (1-xDrop)*yDrop*noiseTable [size-1-x][y][1] + xDrop*(1-yDrop)*noiseTable [x][size-1-y][1] + (1-xDrop)*(1-yDrop)*noiseTable [size-1-x][size-1-y][1]
		v3 = xDrop*yDrop*noiseTable [x][y][2] + (1-xDrop)*yDrop*noiseTable [size-1-x][y][2] + xDrop*(1-yDrop)*noiseTable [x][size-1-y][2] + (1-xDrop)*(1-yDrop)*noiseTable [size-1-x][size-1-y][2]

		im.putpixel ( (x,y), (v1, v2, v3) )
		
im.show ()
im.save ( "noise-2D.png", "png" )
