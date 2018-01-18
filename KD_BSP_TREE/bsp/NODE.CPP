/*
BSPNode.cpp

Implementation of BSPNode class
*/

#include <assert.h>
#include <limits>
#include "bsp.h"

//-------------- Private Functions

ulong BSPNode::_SplitPoly( BSPNode *Plane, CPoint *SplitPnts )
	// This is limited to convex polygons with no more than 32 sides
{
	ulong Sides = 0;
	bool LastSideParallel = FALSE;

	if( !( Normal == Plane->Normal ) )
	{
		CPoint EdgeDelta;
		double numer, denom, t;

		for( ushort vertex=0; vertex<PntCnt; vertex++ )
		{
			ushort prevVertex = vertex ? vertex - 1 : PntCnt - 1;

			EdgeDelta = PntList[ vertex ] - PntList[ prevVertex ];
			denom = EdgeDelta.DotProduct( Plane->Normal );

			if( denom )
			{
				numer = PntList[ prevVertex ].DotProduct( Plane->Normal ) + Plane->D;
				t = - numer / denom;

				if( !( LastSideParallel && t == 0.0 ) )
				{
					if( t > 0.0 && t < 0.999999 )
					{
						Sides |= 1 << vertex;

						if( SplitPnts )
						{
							SplitPnts[ vertex ].x = PntList[ prevVertex ].x + t * EdgeDelta.x;
							SplitPnts[ vertex ].y = PntList[ prevVertex ].y + t * EdgeDelta.y;
							SplitPnts[ vertex ].z = PntList[ prevVertex ].z + t * EdgeDelta.z;
						}
					}
				}
			}

			LastSideParallel = ( denom == 0 );
		}
	}

	return Sides;
}

double BSPNode::ClosestPoint(const CPoint &p, CPoint &closestPoint )
{
	if (PntCnt < 3)
		return -1.0;	//illegal value

	const CPoint &first = PntList[0];
	double dist = std::numeric_limits<double>::max();
	
	for (int i = 1; i < PntCnt - 1; i++)
	{
		CPoint p1 = PntList[i];
		CPoint p2 = PntList[i + 1];
		
		BSPNode triangle;
		triangle.PntCnt = 3;
		triangle.PntList.push_back(first);
		triangle.PntList.push_back(p1);
		triangle.PntList.push_back(p2);
		triangle._ComputeCenter();
		triangle._ComputeNormal();
		triangle._ComputeD();
		
		double tmp = triangle.closesPointOnTriangle(p, closestPoint);
		if (tmp < dist)
			dist = tmp;
	}
	//return Normal.DotProduct(PntList[0] - p);
	return dist;
}
double BSPNode::clamp(double v, double lower, double upper)
{
	if (v < lower)
		return lower;
	else if (v > upper)
		return upper;
	else
		return v;
}

double BSPNode::closesPointOnTriangle(const CPoint &sourcePosition, CPoint &closestPoint )
{
	double dist = 0.0;

	const double ZERO = 0.0;
	const double ONE = 1.0;
    CPoint edge0 = PntList[1] - PntList[0];
    CPoint edge1 = PntList[2] - PntList[0];
    CPoint v0 = PntList[0] - sourcePosition;

    double a = edge0.DotProduct( edge0 );
    double b = edge0.DotProduct( edge1 );
    double c = edge1.DotProduct( edge1 );
    double d = edge0.DotProduct( v0 );
    double e = edge1.DotProduct( v0 );
    double det = a*c - b*b;
    double s = b*e - c*d;
    double t = b*d - a*e;

    if ( s + t < det )
    {
        if ( s < ZERO )
        {
            if ( t < ZERO )
            {
                if ( d < ZERO )
                {
                    s = clamp( -d/a, ZERO, ONE );
                    t = ZERO;
                }
                else
                {
                    s = ZERO;
                    t = clamp( -e/c, ZERO, ONE );
                }
            }
            else
            {
                s = ZERO;
                t = clamp( -e/c, ZERO, ONE );
            }
        }
        else if ( t < ZERO )
        {
            s = clamp( -d/a, ZERO, ONE );
            t = ZERO;
        }
        else
        {
            double invDet = ONE / det;
            s *= invDet;
            t *= invDet;
        }
    }
    else
    {
        if ( s < ZERO )
        {
            double tmp0 = b+d;
            double tmp1 = c+e;
            if ( tmp1 > tmp0 )
            {
                double numer = tmp1 - tmp0;
                double denom = a-2*b+c;
                s = clamp( numer/denom, ZERO, ONE );
                t = 1-s;
            }
            else
            {
                t = clamp( -e/c, ZERO, ONE );
                s = ZERO;
            }
        }
        else if ( t < ZERO )
        {
            if ( a+d > b+e )
            {
                double numer = c+e-b-d;
                double denom = a-2*b+c;
                s = clamp( numer/denom, ZERO, ONE );
                t = 1-s;
            }
            else
            {
                s = clamp( -e/c, ZERO, ONE );
                t = ZERO;
            }
        }
        else
        {
            double numer = c+e-b-d;
            double denom = a-2*b+c;
            s = clamp( numer/denom, ZERO, ONE );
            t = ONE - s;
        }
    }

    closestPoint = PntList[0] + edge0 * s + edge1 * t;
	CPoint delta = closestPoint - sourcePosition;
	dist = delta.Magnitude();
	
	return dist;
}


void BSPNode::_ComputeCenter( void )
{
	Center.x = Center.y = Center.z = 0.0;

	for( ushort i=0; i<PntCnt; i++ )
	{
		Center.x += PntList[ i ].x;
		Center.y += PntList[ i ].y;
		Center.z += PntList[ i ].z;
	}

	Center.x /= PntCnt;
	Center.y /= PntCnt;
	Center.z /= PntCnt;
}


void BSPNode::_ComputeNormal( void )
{
	CPoint a, b;

	assert( PntCnt >= 3 );

	a = PntList[ 0 ] - PntList[ 1 ];
	b = PntList[ 2 ] - PntList[ 1 ];

	Normal = a.CrossProduct( b );
	Normal.Normalize();
}


void BSPNode::_ComputeD( void )
{
	D = -Normal.DotProduct( Center );
}

//-------------- Public Functions

BSPNode::BSPNode( void ) :
FrontNode( NULL ),
	BackNode( NULL ),
	PntCnt( 0 ),
	PntList( NULL )
{
}


BSPNode::~BSPNode( void )
{
};


void BSPNode::ReadPoly( ifstream& Input )
{
	Input >> PntCnt;
	assert( PntCnt >= 3 );

	for( ushort i=0; i<PntCnt; i++ )
	{
		CPoint pt;
		pt.Read( Input );
		PntList.push_back(pt);
	}

	_ComputeCenter();
	_ComputeNormal();
	_ComputeD();

	// Harold: adjust direction
	_AdjustDirection();
}


void BSPNode::_AdjustDirection( void )
{
	assert(PntCnt == 3);
	
	// Harold: adjust direction
	if (D < 0.0)
	{
		CPoint tmp = PntList[0];
		PntList[0] = PntList[1];
		PntList[1] = tmp;
		D = -D;
		Normal.x = -Normal.x;
		Normal.y = -Normal.y;
		Normal.z = -Normal.z;
	}
}

//check whether the polygon point array is ordered correctly.
void BSPNode::check()
{
	int prev = 0;
	CPoint sum;
	CPoint last = PntList[PntCnt - 1] - PntList[0];
	assert(PntList[0].IsValid());
	for (int cur = 1; cur < PntCnt; cur++)
	{
		CPoint delta = PntList[cur] - PntList[prev];
		assert(PntList[cur].IsValid());
		prev++;
		sum = sum + delta;
	}

	assert(sum == last);
}

void BSPNode::RandomPoly()
{
	PntCnt = 3;

	for( ushort i=0; i<PntCnt; i++ )
	{
		CPoint pt;
		pt.RandomPoint();
		PntList.push_back(pt);
	}

	_ComputeCenter();
	_ComputeNormal();
	_ComputeD();

	// Harold: adjust direction
	//_AdjustDirection();
}


void BSPNode::PolyFromPoints(vector<CPoint> const &points)
{
	PntCnt = points.size();

	for( ushort i = 0; i<PntCnt; i++ )
	{
		PntList.push_back(points[i]);
	}

	_ComputeCenter();
	_ComputeNormal();
	_ComputeD();
	
	// Harold: adjust direction
	//_AdjustDirection();
}


void BSPNode::WriteNode( ofstream& Output, short& CurIndex )
{
	if( FrontNode )
		FrontNode->WriteNode( Output, CurIndex );

	if( BackNode )
		BackNode->WriteNode( Output, CurIndex );

	// write index and child indices

	// write point list
	Output << PntCnt << '\n';
	for( short i=0; i<PntCnt; i++ )
	{
		Output << '\t';
		PntList[ i ].Write( Output );
	}

	Output << '\n';
}


bool BSPNode::Intersects( BSPNode *Plane )
{
	return ( _SplitPoly( Plane, NULL ) != 0 );
}


BSPNode *BSPNode::Split( BSPNode *Plane )
{
	BSPNode *NewNode = 0;
	CPoint *SplitPnts;
	ulong Splits;

	SplitPnts = new CPoint[ PntCnt ];
	Splits = _SplitPoly( Plane, SplitPnts );

	if( Splits )
	{
		vector<CPoint> NewPoly1, NewPoly2;
		ushort Poly1Index = 0, Poly2Index = 0;
		ushort Destination = 0;

		for( ushort i=0; i<PntCnt; i++ )
		{
			// Handle split points
			if( Splits & ( 1 << i ) )
			{
				NewPoly1.push_back(SplitPnts[ i ]);
				Poly1Index++;
				
				NewPoly2.push_back(SplitPnts[ i ]);
				Poly2Index++;
				
				Destination ^= 1;
			}

			if( Destination ){
				NewPoly1.push_back(PntList[ i ]);
				Poly1Index++;
			}
			else{
				NewPoly2.push_back(PntList[ i ]);
				Poly2Index++;
			}
		}

		// Make New node
		NewNode = new BSPNode;
		NewNode->PntCnt = Poly1Index;
		NewNode->PntList = NewPoly1;
		NewNode->Normal = Normal;
		NewNode->_ComputeCenter();
		NewNode->_ComputeD();

		PntCnt = Poly2Index;
		PntList = NewPoly2;
		_ComputeCenter();
		_ComputeD();
	}

	delete SplitPnts;

	return NewNode;
}

void BSPNode::SearchClosestNode( const CPoint& CameraLoc, vector<BSPNode*>& closestNodes )
{
	CPoint VecToCam = CameraLoc - Center;
	
	closestNodes.push_back(this);
	
	double direction = VecToCam.DotProduct( Normal );
	if( direction < 0 )	//search back
	{
		if( BackNode )
			BackNode->SearchClosestNode( CameraLoc, closestNodes );
	}
	else				//search front
	{
		if( FrontNode )
			FrontNode->SearchClosestNode( CameraLoc, closestNodes );
	}
}

void BSPNode::Traverse( const CPoint& CameraLoc)
{
	CPoint VecToCam = CameraLoc - Center;
	double direction = VecToCam.DotProduct( Normal );
	
	if( direction < 0 )
	{
		if( FrontNode )
			FrontNode->Traverse( CameraLoc );
		
		// Process 'this'  i.e. render it to screen
		
		if( BackNode )
			BackNode->Traverse( CameraLoc );
	}
	else
	{
		if( BackNode )
			BackNode->Traverse( CameraLoc );

		// Process 'this'  i.e. render it to screen
		
		if( FrontNode )
			FrontNode->Traverse( CameraLoc );
	}
}

