/*
BSPNodeList.cpp

Implementation of BSPNodeList class
*/

#include "bsp.h"


BSPNodeList::BSPNodeList( void )
{
	FirstNode = LastNode = NULL;
}


BSPNodeList::~BSPNodeList( void )
{
	BSPNodeHeader *Node = FirstNode;
	while( Node )
	{
		BSPNodeHeader *temp = Node;
		Node = Node->GetNext();
		delete temp;
	}
}


void BSPNodeList::ReadPolys( ifstream& Input )
{
	ushort PolyCnt;

	Input >> PolyCnt;

	while( PolyCnt-- )
	{
		BSPNode *NewNode = new BSPNode;
		NewNode->ReadPoly( Input );

		Insert( NewNode );
	}
}

void BSPNodeList::RandomPolys(int count)
{
	for (int i = 0; i < count; i++)
	{
		BSPNode *NewNode = new BSPNode;
		NewNode->RandomPoly();
		Insert( NewNode );
	}
}


void BSPNodeList::Insert( BSPNode *Node )
{
	BSPNodeHeader *NewHeader = new BSPNodeHeader( Node );

	NewHeader->SetNext( FirstNode );
	FirstNode = NewHeader;
}


void BSPNodeList::Remove( BSPNode *Node )
{
	if( Node == FirstNode->GetData() )
	{
		BSPNodeHeader *tempPtr= FirstNode;

		FirstNode = FirstNode->GetNext();
		delete tempPtr;
		return;
	}

	BSPNodeHeader *HeaderPtr = FirstNode;

	do
	{
		if( HeaderPtr->GetNext()->GetData() == Node )
		{
			BSPNodeHeader *tempPtr = HeaderPtr->GetNext();

			HeaderPtr->SetNext( HeaderPtr->GetNext()->GetNext() );
			delete tempPtr;
			return;	
		}

		HeaderPtr = HeaderPtr->GetNext();
	} while( HeaderPtr->GetNext() );	

	assert( 0 );
}



//--------------------- BSPListIterator

BSPListIterator::BSPListIterator( void )
{
	Current = NULL;
}


BSPNode *BSPListIterator::First( const BSPNodeList *List )
{
	Current = List->FirstNode;
	return Current ? Current->GetData() : NULL;
}


BSPNode *BSPListIterator::Next( void )
{
	Current = Current->GetNext();
	return Current ? Current->GetData() : NULL;
}




