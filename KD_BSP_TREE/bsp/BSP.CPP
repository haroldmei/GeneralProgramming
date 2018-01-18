/*
bsp.cpp

A short testbed for BSP code
*/


#include <stdlib.h>
#include "bsp.h"

#include <ctime>
#include <chrono>
#include <stdio.h> 
#include <string.h> 
#include <string> 
#include "ply\rply.h"

//using namespace std;
vector<CPoint> pointPool;
static int vertex_cb(p_ply_argument argument) {
	static int axis = 0;
	static CPoint pnt;
    long eol;
    ply_get_argument_user_data(argument, NULL, &eol);

	if (axis == 0)
	    pnt.x = ply_get_argument_value(argument);
	else if (axis == 1)
	    pnt.y = ply_get_argument_value(argument);
	else
	    pnt.z = ply_get_argument_value(argument);
	axis++;
	
    if (eol){
		axis = 0;
		pointPool.push_back(pnt);
	}
    return 1;
}

BSPNodeList nodes;
static int face_cb(p_ply_argument argument) {
    long length, value_index;
	static vector<CPoint> poly;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    switch (value_index) {
        case 0:
        case 1: 
			poly.push_back( pointPool[ply_get_argument_value(argument)]);
			break;
			
        case 2:
			{
			poly.push_back( pointPool[ply_get_argument_value(argument)]);
			BSPNode *bspNode = new BSPNode;
			bspNode->PolyFromPoints(poly);
			nodes.Insert(bspNode);
			poly.clear();
			break;
        	}
			
        default: 
            break;
    }
    return 1;
}


int plyInput(char *name) {
    long nvertices, ntriangles;
    p_ply ply = ply_open(name, NULL, 0, NULL);
    if (!ply) return 1;
    if (!ply_read_header(ply)) return 1;
    nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, 0);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, 1);
    ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
	
    printf("%ld vertices, %ld triangels loaded, constructing BSP tree...\n", nvertices, ntriangles);
    if (!ply_read(ply)) return 1;
    ply_close(ply);
    return 0;
}


int main( short argc, char *argv[] )
{
	BSPTree Tree;

	if( argc != 3 )
	{
		cout << "Build BSP tree randomly: BSP -c 2000" << endl;
		cout << "or build BSP tree from *.ply file: BSP -f filename" << endl;
		return 0;
	}

	Tree.SetNodes(&nodes);
	if (!strcmp(argv[1], "-c"))
	{
		int faces = atoi(argv[2]);
		Tree.RandomPolyList(faces);
		cout << faces << " random polygons are created, with all points inside 0-100 range" << endl;
	}
	else if (!strcmp(argv[1], "-f"))
	{

		plyInput(argv[2]);

	}
	else
	{
		cout << "Unknown options. Build a BSP tree randomly: BSP -c 2000" << endl;
		cout << "or build BSP tree from *.ply file: BSP -f filename" << endl;
		return 0;
	}

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	Tree.BuildTree();

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "BSP creation: " << elapsed_seconds.count() << " seconds" << endl;

	while (1) {
		CPoint CamLoc;
		std::string str1, str2, str3;
		cout << "please input a query point: " << endl;
		cin >> str1 >> str2 >> str3;
		CamLoc.x = atoi(str1.c_str());
		CamLoc.y = atoi(str2.c_str());
		CamLoc.z = atoi(str3.c_str());
		cout << "target point: " << CamLoc.x << ", " << CamLoc.y << ", " << CamLoc.z << endl;

		start = std::chrono::system_clock::now();

		Tree.SearchClosestNode(CamLoc);

		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		cout << "query time: " << elapsed_seconds.count() << " seconds" << endl;
	}
	return 0;
}	
