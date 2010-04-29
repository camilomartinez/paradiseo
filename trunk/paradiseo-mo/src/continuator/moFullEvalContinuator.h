/*
<moFullEvalContinuator.h>
Copyright (C) DOLPHIN Project-Team, INRIA Lille - Nord Europe, 2006-2010

Sebastien Verel, Arnaud Liefooghe, Jeremie Humeau

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  ue,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.
The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

ParadisEO WebSite : http://paradiseo.gforge.inria.fr
Contact: paradiseo-help@lists.gforge.inria.fr
*/

#ifndef _moFullEvalContinuator_h
#define _moFullEvalContinuator_h

#include <continuator/moContinuator.h>
#include <neighborhood/moNeighborhood.h>
#include <eoEvalFuncCounter.h>

/**
 * Continue until a maximum fixed number of full evaluation is reached
 *
 * Becareful 1: The number of full evaluations considered is within the local search (not before it) 
 * Becareful 2: Can not be used if the evaluation function is used in parallel
 */
template< class Neighbor >
class moFullEvalContinuator : public moContinuator<Neighbor>
{
public:
  typedef typename Neighbor::EOT EOT ;

  /**
   * Default constructor
   * @param _eval evaluation function to count
   * @param _maxFullEval number maximum of iterations
   */
  moFullEvalContinuator(eoEvalFuncCounter<EOT> & _eval, unsigned int _maxFullEval): eval(_eval), maxFullEval(_maxFullEval) {
    nbEval_start = eval.value();
  }
  
  /**
   * Test if continue
   *@param _solution a solution
   *@return true if number of evaluations < maxFullEval
   */
  virtual bool operator()(EOT & _solution) {
    return (eval.value() - nbEval_start < maxFullEval);
  }
  
  /**
   * Reset the number of evaluations
   * @param _solution a solution
   */
  virtual void init(EOT & _solution) {
    nbEval_start = eval.value();
  }
  
    /**
     * the current number of evaluation from the begining
     * @return the number of evaluation
     */
    unsigned int value() {
      return eval.value() - nbEval_start ;
    }

private:
  eoEvalFuncCounter<EOT> & eval;
  unsigned int nbEval_start ;
  unsigned int maxFullEval ;
  
};
#endif
