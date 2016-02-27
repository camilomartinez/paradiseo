#ifndef MOEOARCHIVESTAT_H_
#define MOEOARCHIVESTAT_H_

#include <eoPop.h>
#include <utils/eoStat.h>
#include <archive/moeoArchive.h>

/**
 * This class allows to update the archive at each generation with newly found non-dominated solutions.
 * Implemented as a stat to receive the current population instead of relying on a reference as
 * updated does, which is giving trouble when using ReuseSamePopEA reset algo with mpi.
 */
template < class MOEOT >
class moeoArchiveStat : public eoStatBase<MOEOT>
  {
  public:

    /**
     * Ctor
     * @param _arch an archive of non-dominated solutions
     */
    moeoArchiveStat(moeoArchive < MOEOT > & _arch) : arch(_arch)
    {
    }


    /**
     * Updates the archive with newly found non-dominated solutions contained in the main population
     */
    void operator()(const eoPop<MOEOT>& _pop)
    {
      //std::cout << "moeoArchiveStat() Pop address: " << &_pop << std::endl;
      arch(_pop);
    }


  private:

    /** the archive of non-dominated solutions */
    moeoArchive < MOEOT > & arch;

  };

#endif /*MOEOARCHIVESTAT_H_*/
