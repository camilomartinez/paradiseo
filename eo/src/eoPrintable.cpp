//-----------------------------------------------------------------------------
// eoPrintable.cpp
//-----------------------------------------------------------------------------

#include <eoPrintable.h>

//-----------------------------------------------------------------------------
//Implementation of these objects
//-----------------------------------------------------------------------------

ostream & operator << ( ostream& _os, const eoPrintable& _o ) {
	_o.printOn(_os);
	return _os;
};

//-----------------------------------------------------------------------------

