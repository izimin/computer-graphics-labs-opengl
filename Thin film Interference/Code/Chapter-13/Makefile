#
# Makefile to build examples for chapter 11 for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>
#

include ../make.inc

ALL = room room-2 room-3 room-4

all: $(ALL)

room: room.o boxes.o SceneObject.o $(OBJS)
	cc $(LDFLAGS) room.o boxes.o SceneObject.o $(OBJS) -o room $(LIBS) -lc

room-2: room-2.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o $(OBJS)
	cc $(LDFLAGS) room-2.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o $(OBJS) -o room-2 $(LIBS) -lc

room-3: room-3.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o Cell.o $(OBJS)
	cc $(LDFLAGS) room-3.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o Cell.o $(OBJS) -o room-3 $(LIBS) -lc

room-4: room-4.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o Cell.o $(OBJS)
	cc $(LDFLAGS) room-4.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o Cell.o $(OBJS) -o room-4 $(LIBS) -lc

clean:
	rm -r -f $(ALL) $(OBJS) room.o room-2.o room-3.o room-4.o boxes.o SceneObject.o Frustum.o Plane.o Math3D.o Cell.o 2> /dev/nul


include ../rules.linux