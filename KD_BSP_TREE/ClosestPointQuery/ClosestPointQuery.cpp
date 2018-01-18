/* ClosestPointQuery
 * Copyright (C) 2015 Harold Mei <haroldmei.cn@gmail.com>
 */

#include <iostream>
#include <chrono>
#include <ctime>
#include "ClosestPointQuery.hpp"

// test 0, 1, more than 1 points;
void test1D(int count)
{
	using KDTree = ClosestPointQuery<float, 1>;
	using Mesh = KDTree::Mesh;
	using Point = KDTree::Point;
	vector<Point> points1d;

	if (count == 1)
		points1d.push_back(Point({ 2 }));
	else if (count > 1){
		points1d.push_back(Point({ 2 }));
		points1d.push_back(Point({ 5 }));
		points1d.push_back(Point({ 9 }));
		points1d.push_back(Point({ 4 }));
		points1d.push_back(Point({ 8 }));
		points1d.push_back(Point({ 7 }));
		points1d.push_back(Point({ 3 }));
		points1d.push_back(Point({ 4 }));
		points1d.push_back(Point({ 6 }));
		points1d.push_back(Point({ 7 }));
		points1d.push_back(Point({ 1 }));
		points1d.push_back(Point({ 2 }));
	}

	KDTree cpq1d(points1d);
	Point neib1d = cpq1d(Point({ 4.1 }), 100);	// within max distance
	cout << neib1d[0] << endl;

	neib1d = cpq1d(Point({ 4.9 }), 0.01);			// beyond max distance
	cout << neib1d[0] << endl;
}

// test 0, 1, more than 1 points;
void test2D(int count)
{
	using KDTree = ClosestPointQuery<int, 2>;
	using Mesh = KDTree::Mesh;
	using Point = KDTree::Point;
	vector<Point> points;
	if (count == 1)
		points.push_back(Point({ -2, 3 }));
	else if (count > 1){
		points.push_back(Point({ -2,3 }));
		points.push_back(Point({ -5,4 }));
		points.push_back(Point({ -9,6 }));
		points.push_back(Point({ -4,7 }));
		points.push_back(Point({ -8,1 }));
		points.push_back(Point({ -7,2 }));
		points.push_back(Point({ 2, 3 }));
		points.push_back(Point({ 5, 4 }));
		points.push_back(Point({ 9, 6 }));
		points.push_back(Point({ 4, 7 }));
		points.push_back(Point({ 8, 1 }));
		points.push_back(Point({ 7, 2 }));
	}
	
	KDTree cpq(points);
	Point neib = cpq(Point({ 5, 5 }), 100);		// within max distance
	cout << neib[0] << "," << neib[1] << endl;

	neib = cpq(Point({ 0, 0 }), 0.1);				// beyond max distance
	cout << neib[0] << "," << neib[1] << endl;
}

// test 0, 1, more than 1 points;
void test3D(int count)
{
	using KDTree = ClosestPointQuery<int, 3>;
	using Mesh = KDTree::Mesh;
	using Point = KDTree::Point;
	vector<Point> points3D;
	if (count == 1)
	{
		points3D.push_back(Point({ 2, 3, 3 }));
	}
	else if (count > 1)
	{
		points3D.push_back(Point({ 2, 3, 3 }));
		points3D.push_back(Point({ 5, 4, 2 }));
		points3D.push_back(Point({ 9, 6, 7 }));
		points3D.push_back(Point({ 4, 7, 9 }));
		points3D.push_back(Point({ 8, 1, 5 }));
		points3D.push_back(Point({ 7, 2, 6 }));
		points3D.push_back(Point({ 9, 4, 1 }));
		points3D.push_back(Point({ 8, 4, 2 }));
		points3D.push_back(Point({ 9, 7, 8 }));
		points3D.push_back(Point({ 6, 3, 1 }));
		points3D.push_back(Point({ 3, 4, 5 }));
		points3D.push_back(Point({ 1, 6, 8 }));
		points3D.push_back(Point({ 9, 5, 3 }));
		points3D.push_back(Point({ 2, 1, 3 }));
		points3D.push_back(Point({ 8, 7, 6 }));
	}
	KDTree cpq3d(points3D);
	Point neib3d = cpq3d(Point({ 5, 5, 1 }), 300);		// within max distance
	cout << neib3d[0] << "," << neib3d[1] << "," << neib3d[2] << endl;

	neib3d = cpq3d(Point({ 0, 0, 0 }), 0.1);				// beyond max distance
	cout << neib3d[0] << "," << neib3d[1] << "," << neib3d[2] << endl;
}

// the following 3 test is to verify the correctness of ClosestPointQuery class.
// the following test cases should be performed:
// 0. illegal dimention test (compile error)
// 1. 0 or 1 1d/2d/3d points; (could be done by commenting the above hardcoded points)
// 2. small number of 1d/2d/3d points as hardcoded above should generate correct closest point (test1D(), test2D, test3D call);
// 3. large amount of points is tested by randomly generated points as done below.
int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "query 1d mesh:" << endl;
		test1D(0);
		test1D(1);
		test1D(2);
		cout << endl;

		cout << "query 2d mesh:" << endl;
		test2D(0);
		test2D(1);
		test2D(2);
		cout << endl;

		cout << "query 3d mesh:" << endl;
		test3D(0);
		test3D(1);
		test3D(2);
		cout << endl;
		return 0;
	}

	if (argc != 3)
	{
		cout << "Usage: ClosestPointQuery.exe <number target> <number source>" << endl;
		return 0;
	}

	/* a compile error will be generated
	using KDTreeIllegal = ClosestPointQuery<int, 0>;
	KDTreeIllegal::Mesh illegarMesh;
	KDTreeIllegal::generateMesh(1000, illegarMesh);
	*/

	using KDTree = ClosestPointQuery<int, 3>;
	using Mesh = KDTree::Mesh;
	using Point = KDTree::Point;

	int target = atoi(argv[1]);
	int source = atoi(argv[2]);

	// create the target image, used for target mesh
	Mesh targetMesh;
	KDTree::generateMesh(target, targetMesh);
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	KDTree cpq(targetMesh);
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "mesh creation: " << elapsed_seconds.count() << " seconds" << endl;

	// create the source image, perform repeated query for each of the point in the image
	Mesh sourceMesh;
	KDTree::generateMesh(source, sourceMesh);
	start = std::chrono::system_clock::now();
	for (size_t i = 0; i < sourceMesh.size(); i++)
	{
		Point curr = sourceMesh.at(i);
		Point neib3d = cpq(curr, 30000);
	}
	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
	cout << "query " << source << " points: " << elapsed_seconds.count() << " seconds" << endl;

	return 0;
}
