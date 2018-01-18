/* ClosestPointQuery
* Copyright (C) 2015 Harold Mei <haroldmei.cn@gmail.com>
*/

#include <array>
#include <vector>
#include <iterator>
#include <algorithm>
#include <limits>
#include <tuple>	
#include <stack>
#include <queue>
#include <limits>
#include <stdlib.h>

#pragma once
using namespace std;

//! kdnode implementation
//! N dimension kdnode creation and nearest point query;
//! 
//! template args:
//!		TYPE: could be int, long, float, depending on different application;
//!		DIM: any value greater than 0, dimension of the space.
//!
//! References:
//! 1, A brief description of kdnode: https://en.wikipedia.org/wiki/K-d_tree (BuildKD, binary search: operator())
//! 2, <Building a Balanced k-d Tree in O(kn log n) Time> by Russell A. Brown (implemented in BuildKDFast)
//! 3, <Approximate Nearest Neighbor Queries in Fixed Dimensions> by Sunil Arya and David M. Mount (todo...)
//! 4, octree (todo...)
template<class TYPE, int DIM>
class ClosestPointQuery
{
private:
	/* a compile error will be generated if dimension is smaller or equal than 0*/
	static_assert(DIM > 0, "error dimision!");

	// to make it easier, disable default/copy/assignment;
	ClosestPointQuery();
	ClosestPointQuery(const ClosestPointQuery&);
	ClosestPointQuery &operator=(const ClosestPointQuery) const;

public:
	// an array is used for n-dimensional points
	using Point = array<TYPE, DIM>;	
	using MyIterator = typename vector<array<TYPE, DIM>>::iterator;
	using MyConstIterator = typename vector<array<TYPE, DIM>>::const_iterator;

	// assumption: a mesh is simply an array of points. kdnode contains only its position here.
	// other information of a point is ignored.
	using Mesh = vector<Point>;
	struct kdnode
	{
		kdnode() :left(0), right(0){}
		Point p;
		kdnode* left;
		kdnode* right;
	};

	//! Constructor;
	//! 1, create a kd tree for use of neareast point in the mesh;
	//!	2, maintain an invalid point in case there is no point in maxDist distance;
	//!
	//! input:
	//!		points: points in the mesh;
	ClosestPointQuery(const Mesh &points): currentFree(0), kdhead(0)
	{
		invalidPoint.fill(std::numeric_limits<TYPE>::max());
		nodepool = new kdnode[points.size()];

		Mesh tempMesh = points;
		BuildKD(tempMesh.begin(), tempMesh.end(), &kdhead);
	}

	//! another binary search tree;
	//! complexity:
	//!		time O(log(n))
	//!		space O(log(n))
	//!
	//! input:
	//!		queryPoint: source point used for the query
	//!		maxDist: max distance in which the nearest point will be.
	//! output:
	//!		the nearest point to source point within a certain distance;
	//£¡		if no point is found in maxDist area, the function returns an invalid point
	Point operator()(const Point& queryPoint, double maxDist) const
	{
		int depth = 0;
		kdnode *curr = kdhead;

		/*  std::priority_queue maintains a heap with O(logn) insertion time,
		*	this is used to sort all potential neareast point.
		*	the head of the queue is the one we are looking for
		*/
		auto cmp = [queryPoint](const Point &left, const Point &right) {
			return distance(queryPoint, left) > distance(queryPoint, right);
		};
		std::priority_queue<Point, std::vector<Point>, decltype(cmp)> candidates(cmp);

		// Binary search in the kd-tree. Compare different axis
		// according to the current node height. For 3d points:
		// height 0 compares x axis, 
		// height 1 compare y axis, 
		// height 2 compares z axis, 
		// height 3 start over from axis x again.
		while (curr)
		{
			candidates.push(curr->p);
			if (queryPoint[depth % DIM] < curr->p[depth % DIM])
				curr = curr->left;
			else
				curr = curr->right;

			depth++;
		}

		// only distance smaller than maxDist is retured.
		if (candidates.size() &&
			distance(queryPoint, candidates.top()) < pow(maxDist, 2.0))
			return candidates.top();
		else
			return invalidPoint;
	}

	//! another binary search tree;
	~ClosestPointQuery()
	{
		delete[] nodepool;
	}

	//! generate a mesh with large amount of points;
	//! input:
	//! n, number of points
	//! mesh, the vector of points in the mesh
	void static generateMesh(long n, vector<Point> &mesh)
	{
		for (int i = 0; i < n; i++)
		{
			Point pt;
			for (TYPE &val : pt) val = rand();
			mesh.push_back(pt);
		}
	}

private:
	//! draft 2: divide by axis in turn, no recursion, 
	//! an improvement from draft 1, call stack will no 
	//! long be broken;
	//! complexity:
	//!		time O(nlog2(n))
	//!		space O(n)
	//!
	//! input:
	//!		beg, end: array range of a given point list;
	//!		depth:	current tree depth being built;
	//! output:
	//!		pNode:	current subtree being built.
	void BuildKD(MyIterator itBeg, MyIterator itEnd, kdnode **pTree)
	{
		// build the tree from root
		// a stack is used to avoid the recursive call;
		// to eliminate the break of the call stack;
		stack<tuple<MyIterator, MyIterator, int, kdnode **>> callStack;
		callStack.push(make_tuple(itBeg, itEnd, 0, pTree));
		while (callStack.size() > 0)
		{
			tuple<MyIterator, MyIterator, int, kdnode **> current = callStack.top();
			callStack.pop();
			auto beg = get<0>(current);
			auto end = get<1>(current);
			auto depth = get<2>(current);
			auto pNode = get<3>(current);

			//skip leaf
			if (end == beg)
				continue;

			// O(nlog(n)), lamba expr used as comparator
			// sort in each division. divide space into two parts depending on current 
			// kd tree height. for a 3d mesh,
			// height 0, divide x axis in the middle;
			// height 1, divide y axis in the middle;
			// height 2, divide z axis in the middle;
			// height 3, start over from x again
			int axis = depth % DIM;
			sort(beg, end,
				[axis](const Point &a, const Point &b) -> bool {
				return a[axis] < b[axis];
			});

			*pNode = &nodepool[currentFree++];
			int mid = (end - beg) / 2;
			(*pNode)->p = *(beg + mid);

			//use stack to avoid recursive call
			if (mid > 0)
				callStack.push(make_tuple(beg, beg + mid, depth + 1, &(*pNode)->left));

			if (end - beg > mid)
				callStack.push(make_tuple(beg + mid + 1, end, depth + 1, &(*pNode)->right));
		}
	}

	//! draft 4: another kdnode creation, no recursion, 
	//! <Building a Balanced k-d Tree in O(kn log n) Time> by Russell A. Brown
	//! 
	//! <Harold: the solution is a bit slower than draft 2, for it copies a lot>
	//! complexity:
	//!		time O(knlog(n))
	//!		space O(n)
	//!
	//! input:
	//!		beg, end: array range of a given point list;
	//!		depth:	current tree depth being built;
	//! output:
	//!		pNode:	current subtree being built.
	void BuildKDFast(MyConstIterator itBeg, MyConstIterator itEnd, kdnode **pTree)
	{
		// initialize DIM lists;
		// O(nlog(n)), lamba expr used as comparator
		// sort each of them arrays first, based on combination of the coordinates;
		// for a 3D mesh, the tree arrays is sorted by:
		// references[0], comparing values composed in xyz order;
		// references[1], comparing values composed in yzx order;
		// references[2], comparing values composed in zxy order;
		vector<vector<int>> references;
		for (int i = 0; i < DIM; i++)
		{
			vector<int> cur;
			for (int j = 0; j < itEnd - itBeg; j++)
				cur.push_back(j);

			sort(cur.begin(), cur.end(), [itBeg, i](int a, int b) -> bool {
				return superKeyCompare(*(itBeg + a), *(itBeg + b), i) < 0;
			});
			references.push_back(cur);
		}

		// build the tree from root
		// a stack is used to avoid the recursive call;
		// to eliminate the break of the call stack;
		stack<tuple<int, int, int, kdnode **>> callStack;
		callStack.push(make_tuple(0, itEnd - itBeg, 0, pTree));
		while (callStack.size() > 0)
		{
			tuple<int,int,int,kdnode **> current = callStack.top();
			callStack.pop();
			auto idx1 = get<0>(current);
			auto idx2 = get<1>(current);
			auto depth = get<2>(current);
			auto pNode = get<3>(current);

			// skip leaf
			if (idx1 == idx2)
				continue;

			int axis = depth % DIM;
			*pNode = &nodepool[currentFree++];
			int mid = (idx1 + idx2) / 2;
			(*pNode)->p = *(itBeg + references[axis][mid]);
			for (int i = axis + 1; i < axis + DIM; i++)
			{
				// copied one by one, the original array is reordered in O(n) time.
				// scan each of the reference array, compare super keys with the 
				// current mid; smaller points will be moved to lower sub array,
				// greater ones will be moved to the upper sub array;
				// sounds perfect, but the copy of the sub arrays makes 
				// it even slower than draft 2!
				vector<int> temp(references[i % DIM].begin() + idx1, references[i % DIM].begin() + idx2);
				int lower = idx1;
				int upper = (idx1 + idx2) / 2 + 1;
				for (int j = idx1; j < idx2; j++)
				{
					const Point &curP = itBeg[temp[j - idx1]];
					TYPE result = superKeyCompare(curP, (*pNode)->p, axis);
					if (result < 0)
						references[i % DIM][lower++] = temp[j - idx1];
					else if (result > 0)
						references[i % DIM][upper++] = temp[j - idx1];
				}
			}

			//use stack to avoid recursive call
			if (mid > idx1)
				callStack.push(make_tuple(idx1, mid, depth + 1, &(*pNode)->left));

			if (idx2 > mid)
				callStack.push(make_tuple(mid + 1, idx2, depth + 1, &(*pNode)->right));
		}
	}

	//! The superKeyCompare method compares two points in all dimensions, 
	//! and uses the sorting or partition coordinate as the most significant dimension.
	//!
	//! calling parameters:
	//! a - a Point
	//! b - a Point
	//! p - the most significant dimension
	//! returns: a TYPE result of comparing two long arrays
	static TYPE superKeyCompare(const Point &a, const Point &b, int p)
	{
		TYPE diff = 0;
		for (int i = 0; i < DIM; i++)
		{
			// A fast alternative to the modulus operator for (i + p) < 2 * DIM.
			int r = i + p;
			r = (r < DIM) ? r : r - DIM;
			diff = a[r] - b[r];

			// if the current two are the same, compare the next.
			if (diff != 0)
				break;
		}
		return diff;
	}

	//! power of distance returned from the function
	//! in order to make it fast, square calculation is not used
	static double distance(const Point &queryPoint, const Point &pt)
	{
		double dist = 0;
		for (int i = 0; i < DIM; i++)
			dist += pow(queryPoint[i] - pt[i], 2.0);

		return dist;
	}

private:
	kdnode *kdhead;	//root node

	// bulk memory allocation; stores all the points in continuous space,
	// avoid the need to allocate each node one by one, which is very slow.
	kdnode *nodepool;
	int currentFree = 0;
	Point invalidPoint;	// store an 'invalid' point
};
