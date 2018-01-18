/*
BSPTree.cpp

Implementation of top level BSPTree class
*/

#include "bsp.h"
#include <queue>

//--------------- Private Functions
BSPNode *BSPTree::_FindRoot( BSPNodeList *List )
	// Returns node which splits the least other nodes
{
	short BestCount = 0x7fff;
	BSPNode *BestRoot = NULL;

	BSPListIterator Iter;
	BSPNode *TestNode = Iter.First( List );

	while( TestNode )
	{
		BSPListIterator Iter2;
		BSPNode *CheckNode = Iter2.First( List );
		short Count = 0;

		while( CheckNode )
		{
			if( CheckNode != TestNode )
				if( CheckNode->Intersects( TestNode ) )
					Count++;

			CheckNode = Iter2.Next();
		}

		if( Count < BestCount )
		{
			if( !Count )
				return TestNode;

			BestCount = Count;
			BestRoot = TestNode;
		}

		TestNode = Iter.Next();
	}

	return BestRoot;
}				



BSPNode *BSPTree::_BuildBSPTree( BSPNodeList *List, int level )
{
	CPoint Delta;
	BSPNodeList Front, Back;

	BSPListIterator Iter;
	BSPNode *Root = Iter.First(List);//_FindRoot( List );//This is too slow. Harold
	List->Remove( Root );

	BSPNode *TestNode = Iter.First( List );

	while( TestNode )
	{
		//TestNode->check();
		if( TestNode->Intersects( Root ) )
		{
			BSPNode *NewNode = TestNode->Split( Root );

			Delta = TestNode->GetCenter() - Root->GetCenter();
			if( Delta.DotProduct( Root->GetNormal() ) >= 0.0 )
			{
				Front.Insert( TestNode );
				Back.Insert( NewNode );
			}
			else
			{
				Back.Insert( TestNode );
				Front.Insert( NewNode );
			}
		}
		else
		{
			Delta = TestNode->GetCenter() - Root->GetCenter();
			if( Delta.DotProduct( Root->GetNormal() ) >= 0.0 )
				Front.Insert( TestNode );
			else
				Back.Insert( TestNode );
		}

		List->Remove( TestNode );
		TestNode = Iter.First( List );			
	}

	Root->SetFront( Front.Empty() ? NULL : _BuildBSPTree( &Front, level + 1 ) );
	Root->SetBack( Back.Empty() ? NULL : _BuildBSPTree( &Back, level + 1 ) );

	return Root;
}


//--------------- Public Functions

void BSPTree::BuildTree( void )
{
	if (Nodes)
		Root = _BuildBSPTree( Nodes, 0 );
}


void BSPTree::ReadPolyList( ifstream& Input )
{
	Nodes->ReadPolys( Input );
}

void BSPTree::RandomPolyList( int count )
{
	Nodes->RandomPolys( count );
}

void BSPTree::WriteTree( ofstream& Output )
{
	short NodeIndex = 0;
	if( Root )
		Root->WriteNode( Output, NodeIndex );
}


void BSPTree::SearchClosestNode( const CPoint& CameraLoc )
{
	if(Root)
	{
		vector<BSPNode*> result;
		//Root->Traverse(CameraLoc);

		cout << endl;
		Root->SearchClosestNode( CameraLoc, result);
		for (int i = 0; i < result.size(); i++)
		{
			BSPNode *cur = result[i];
			CPoint closestPoint;
			double dist = cur->ClosestPoint(CameraLoc, closestPoint);
			
			cout.precision(5);
			cout << "Dist " << dist
				<< " (" << closestPoint.x << "," << closestPoint.y << "," << closestPoint.z << ")"
				<< endl;
		}
	}
	cout << endl;
}
