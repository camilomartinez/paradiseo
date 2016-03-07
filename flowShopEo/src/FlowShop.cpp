/* 
* <FlowShop.cpp>
* Copyright (C) DOLPHIN Project-Team, INRIA Futurs, 2006-2007
* (C) OPAC Team, LIFL, 2002-2007
*
* Arnaud Liefooghe
*
* This software is governed by the CeCILL license under French law and
* abiding by the rules of distribution of free software.  You can  use,
* modify and/ or redistribute the software under the terms of the CeCILL
* license as circulated by CEA, CNRS and INRIA at the following URL
* "http://www.cecill.info".
*
* As a counterpart to the access to the source code and  rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty  and the software's author,  the holder of the
* economic rights,  and the successive licensors  have only  limited liability.
*
* In this respect, the user's attention is drawn to the risks associated
* with loading,  using,  modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean  that it is complicated to manipulate,  and  that  also
* therefore means  that it is reserved for developers  and  experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software's suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and,  more generally, to use and operate it in the
* same conditions as regards security.
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL license and that you accept its terms.
*
* ParadisEO WebSite : http://paradiseo.gforge.inria.fr
* Contact: paradiseo-help@lists.gforge.inria.fr
*
*/
//-----------------------------------------------------------------------------

#include <FlowShop.h>

#include <vector>

std::string FlowShop::className() const
{
    return "FlowShop";
}

eoserial::Object* FlowShop::pack() const
{
    eoserial::Object* obj = new eoserial::Object;
    obj->add( "array",
            eoserial::makeArray< std::vector<AtomType>, eoserial::MakeAlgorithm >
            ( *this )
            );

    bool invalidVal = invalid();
    obj->add("invalid", eoserial::make( invalidVal ) );
    if( !invalidVal )
    {
        Fitness fitnessVal = fitness();
        obj->add("fitness", eoserial::make( fitnessVal ) );
    }
    return obj;
}

void FlowShop::unpack( const eoserial::Object* obj )
{
    this->clear();
    eoserial::unpackArray< std::vector<AtomType>, eoserial::Array::UnpackAlgorithm >
        ( *obj, "array", *this );
    bool invalid;
    eoserial::unpack( *obj, "invalid", invalid );
    if( invalid ) {
        invalidate();
    } else {
        double fitnessVal;
        eoserial::unpack<Fitness>( *obj, "fitness", fitnessVal );
        fitness( fitnessVal );
    }
    //obj->print(std::cout);
    //std::cout << *this << std::endl << std::endl;
}
