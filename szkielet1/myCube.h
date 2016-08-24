#ifndef MYCUBE_H
#define MYCUBE_H

unsigned int myCubeVertexCount=24;

float myCubeVertices[]={
	-1,-1,-1,
	1,-1,-1,
	1,1,-1,
	-1,1,-1,
	
	-1,-1,1,
	1,-1,1,
	1,1,1,
	-1,1,1,
	
	-1,-1,-1,
	1,-1,-1,
	1,-1,1,
	-1,-1,1,
	
	-1,1,-1,
	1,1,-1,
	1,1,1,
	-1,1,1,
	
	-1,-1,-1,
	-1,-1,1,
	-1,1,1,
	-1,1,-1,
	
	1,-1,-1,
	1,-1,1,
	1,1,1,
	1,1,-1,
};

float myCubeColors[]={
	1,0,0, 1,0,0, 1,0,0, 1,0,0,
	0,1,0, 0,1,0, 0,1,0, 0,1,0,
	0,0,1, 0,0,1, 0,0,1, 0,0,1,
	1,1,0, 1,1,0, 1,1,0, 1,1,0,
	0,1,1, 0,1,1, 0,1,1, 0,1,1,
	1,1,1, 1,1,1, 1,1,1, 1,1,1
};


float myCubeTexCoords[] = {
	0,0,1,0,1,1,0,1,
	0,0,1,0,1,1,0,1,
	0,0,1,0,1,1,0,1,
	0,0,1,0,1,1,0,1,
	0,0,1,0,1,1,0,1,
	0,0,1,0,1,1,0,1
};

float myCubeNormals[] = {
	0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,
	0,0,1, 0,0,1, 0,0,1, 0,0,1,
	0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
	0,1,0, 0,1,0, 0,1,0, 0,1,0,
	-1,0,0, -1,0,0, -1,0,0, -1,0,0,
	1,0,0, 1,0,0, 1,0,0, 1,0,0
};

#endif