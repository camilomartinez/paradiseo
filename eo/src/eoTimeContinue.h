// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoTimeContinue.h
// (c) OPAC Team (LIFL), Dolphin Project (INRIA), 2007
/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: thomas.legrand@lifl.fr
 */
//-----------------------------------------------------------------------------

#ifndef _EOTIMECONTINUE_H
#define _EOTIMECONTINUE_H

#include <chrono>
#include <eoContinue.h>
#include <eoResettable.h>
#include <utils/eoLogger.h>

using namespace std::chrono;

/**
 * Termination condition until a running time is reached.
 *
 * @ingroup Continuators
 */
template < class EOT >
class eoTimeContinue: public eoContinue<EOT>, public eoResettable
{
public:

    /**
     * Ctor.
     * @param _max maximum running time
     */
    eoTimeContinue(double _max): max(_max)
    {
        start = steady_clock::now();
    }


    /**
     * Returns false when the running time is reached.
     * @param _pop the population
     */
    virtual bool operator() (const eoPop < EOT > & _pop)
    {
        steady_clock::time_point end = steady_clock::now();
        duration<double> diff = end-start;
        double elapsed = diff.count();
        if (elapsed >= max)
        {
            eo::log << eo::progress << "STOP in eoTimeContinue: Reached maximum time [" << elapsed << "/" << max << "]" << std::endl;
            return false;
        }
        return true;
    }


    /**
     * Class name
     */
    virtual std::string className(void) const
    {
        return "eoTimeContinue";
    }

    virtual void reset()
    {
        steady_clock::time_point end = steady_clock::now();
        duration<double> diff = end-start;
        eo::log << eo::progress << "RESET in eoTimeContinue after " << diff.count() << " seconds" << std::endl;
        start = steady_clock::now();
    }

private:

    /** maximum running time */
    double max;
    /** starting time */
    steady_clock::time_point start;

};

#endif
