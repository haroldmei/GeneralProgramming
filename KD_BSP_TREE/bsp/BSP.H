/*
BSP.H

Type definitions for BSP code
*/

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
using namespace std;

#define TRUE	1
#define FALSE	0

//typedef unsigned char bool;
typedef unsigned short ushort;
typedef unsigned long ulong;

using namespace std;

class CPoint
{
public:
	double x, y, z;

public:
	CPoint( void );

	void Read( ifstream& Input );
	void Write( ofstream& Output ) const;

	double Magnitude( void ) const;
	void Normalize( void );
	double DotProduct( const CPoint& Pnt ) const;
	CPoint CrossProduct( const CPoint& Pnt ) const;
	bool operator==( const CPoint& Pnt ) const;
	CPoint operator-( const CPoint& Pnt ) const;
	CPoint operator+( const CPoint& Pnt ) const;
	CPoint operator*( double scale ) const;
	void RandomPoint( );
	bool IsValid() const;
};



class BSPNodeList;

class BSPNode
{
private:
	BSPNode *FrontNode, *BackNode;

	short PntCnt;
	vector<CPoint> PntList;

	CPoint Center;
	CPoint Normal;
	double D;

	ulong _SplitPoly( BSPNode *Plane, CPoint *SplitPnts );
	void _ComputeCenter( void );
	void _ComputeNormal( void );
	void _ComputeD( void );
	void _AdjustDirection( void );
	double clamp(double v, double lower, double upper);
	double closesPointOnTriangle(const CPoint &sourcePosition, CPoint &closestPoint);

public:
	BSPNode( void );
	~BSPNode( void );

	void check();
	void RandomPoly();
	short GetPntCnt() const { return PntCnt; }
	void PolyFromPoints(vector<CPoint> const &points);
	void ReadPoly( ifstream& Input );

	void WriteNode( ofstream& Output, short& CurIndex );

	CPoint GetCenter( void )				{ return Center; }
	CPoint GetNormal( void )				{ return Normal; }

	bool Intersects( BSPNode *Plane );
	BSPNode *Split( BSPNode *Plane );

	BSPNode *GetFront( void )			{ return FrontNode; }
	BSPNode *GetBack( void )			{ return BackNode; }

	void SetFront( BSPNode *Node )		{ FrontNode = Node; }
	void SetBack( BSPNode *Node)		{ BackNode = Node; }
	double ClosestPoint(const CPoint &p, CPoint &closestPoint);
	
	void Traverse( const CPoint& CameraLoc);
	void SearchClosestNode( const CPoint& CameraLoc, vector<BSPNode*>& closestNodes);
};


class BSPNodeHeader
{
	friend class BSPListIterator;
	friend class BSPNodeList;

private:
	BSPNodeHeader *Next;
	BSPNode *Data;

	BSPNodeHeader( BSPNode *DataNode )  { Data = DataNode;  Next = NULL; }

	void SetNext( BSPNodeHeader *NextNode )		{ Next = NextNode; }			
	BSPNodeHeader *GetNext( void )				{ return Next; }
	BSPNode *GetData( void )					{ return Data; }
};


class BSPNodeList
{
	friend class BSPListIterator;

private:
	BSPNodeHeader *FirstNode, *LastNode;

public:
	BSPNodeList( void );
	~BSPNodeList( void );

	void ReadPolys( ifstream& Input );
	void RandomPolys(int count);

	bool Empty( void )				{ return FirstNode == NULL; }

	void Insert( BSPNode *Node );
	void Remove( BSPNode *Node );
};


class BSPListIterator
{
private:
	BSPNodeHeader *Current;

public:
	BSPListIterator( void );
	BSPNode *First( const BSPNodeList *List );
	BSPNode *Next( void );
};


class BSPTree
{
private:
	BSPNodeList *Nodes;
	BSPNode *Root;

	BSPNode *_FindRoot( BSPNodeList *List );
	BSPNode *_BuildBSPTree( BSPNodeList *List, int level );

public:
	void SetNodes(BSPNodeList* nodes) { Nodes = nodes ; }
	void ReadPolyList( ifstream& Input );
	void RandomPolyList( int count );

	void WriteTree( ofstream& Output );

	void BuildTree( void );
	void SearchClosestNode( const CPoint& CameraLoc );
};

