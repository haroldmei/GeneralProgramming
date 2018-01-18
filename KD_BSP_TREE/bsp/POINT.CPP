/*
point.cpp

Implementation of CPoint class
*/

#include <math.h>

#include "bsp.h"
#include <stdlib.h>


#define MAX_DELTA		0.0001


CPoint::CPoint( void )
{
	x = y = z = 0.0;
}


void CPoint::Read( ifstream& Input )
{
	Input >> x >> y >> z;
}

double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

bool CPoint::IsValid( ) const
{
	if ((x >= 0.0 && x <= 100.0)
		&& (y >= 0.0 && y <= 100.0)
		&& (z >= 0.0 && z <= 100.0))
		return true;
	
	return true;
}

void CPoint::RandomPoint( )
{
	x = fRand(0.0, 100.0);
	y = fRand(0.0, 100.0);
	z = fRand(0.0, 100.0);
}

void CPoint::Write( ofstream& Output ) const
{
	Output << x << ' ' << y << ' ' << z << '\n' ;
}


double CPoint::Magnitude( void ) const
{
	return sqrt( this->DotProduct( *this ) );
}

void CPoint::Normalize( void )
{
	double Length = Magnitude();

	if( Length )
	{
		x /= Length;
		y /= Length;
		z /= Length;	
	}
	else
		x = y = z = 0.0;
}


double CPoint::DotProduct( const CPoint& Pnt ) const
{
	return x * Pnt.x + y * Pnt.y + z * Pnt.z;
}


CPoint CPoint::CrossProduct( const CPoint& Pnt ) const
{
	CPoint Result;

	Result.x = y * Pnt.z - z * Pnt.y;
	Result.y = z * Pnt.x - x * Pnt.z;
	Result.z = x * Pnt.y - y * Pnt.x;

	return Result;
}

bool CPoint::operator==( const CPoint& Pnt ) const
{
	CPoint Result = *this - Pnt;

	return ( MAX_DELTA >= Result.Magnitude() );
}

CPoint CPoint::operator-( const CPoint& Pnt ) const
{
	CPoint Result;

	Result.x = x - Pnt.x;
	Result.y = y - Pnt.y;
	Result.z = z - Pnt.z;

	return Result;
}

CPoint CPoint::operator+( const CPoint& Pnt ) const
{
	CPoint Result;

	Result.x = x + Pnt.x;
	Result.y = y + Pnt.y;
	Result.z = z + Pnt.z;

	return Result;
}

CPoint CPoint::operator*( double scale ) const
{
	CPoint Result;

	Result.x = x * scale;
	Result.y = y * scale;
	Result.z = z * scale;

	return Result;
}

