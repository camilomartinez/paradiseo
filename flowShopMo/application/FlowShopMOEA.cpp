/* 
* <FlowShopEA.cpp>
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


// moeo general include
#include <moeo>
// eo-mpi general include
#include <mpi/eoMpi.h>
// Execute algo in several nodes
#include <mpi/eoMultiStart.h>
// for the creation of an evaluator
#include <make_eval_FlowShop.h>
// for the creation of an initializer
#include <make_genotype_FlowShop.h>
// for the creation of the variation operators
#include <make_op_FlowShop.h>
// how to initialize the population
#include <do/make_pop.h>
// the stopping criterion
#include <do/make_continue_moeo.h>
// outputs (stats, population dumps, ...)
#include <do/make_checkpoint_moeo.h>
// evolution engine (selection and replacement)
#include <do/make_ea_moeo.h>
// simple call to the algo
#include <do/make_run.h>
// checks for help demand, and writes the status file and make_help; in libutils
void make_help(eoParser & _parser);
// definition of the representation
#include <FlowShop.h>

using namespace std;
using namespace eo::mpi;

/*
 * Wrapper for HandleResponse: shows the best answer, as it is found.
 *
 * Finding the best solution is an associative operation (as it is based on a "min" function, which is associative too)
 * and that's why we can perform it here. Indeed, the min element of 5 elements is the min element of the 3 first
 * elements and the min element of the 2 last elements:
 * min(1, 2, 3, 4, 5) = min( min(1, 2, 3), min(4, 5) )
 *
 * This is a reduction. See MapReduce example to have another examples of reduction.
 */
struct CatBestAnswers : public HandleResponseParallelApply<FlowShop>
{
    CatBestAnswers()
    {
    }

    /*
        our structure inherits the member _wrapped from HandleResponseFunction,
        which is a HandleResponseFunction pointer;

        it inherits also the member _d (like Data), which is a pointer to the
        ParallelApplyData used in the HandleResponseParallelApply&lt;EOT&gt;. Details
        of this data are contained in the file eoParallelApply. We need just to know that
        it contains a member assignedTasks which maps a worker rank and the sent slice
        to be processed by the worker, and a reference to the processed table via the
        call of the data() function.
    */

    // if EOT were a template, we would have to do: (thank you C++ :)
    // using eo::mpi::HandleResponseParallelApply<EOT>::_wrapped;
    // using eo::mpi::HandleResponseParallelApply<EOT>::d;

    void operator()(int wrkRank)
    {
        ParallelApplyData<FlowShop> * d = _data;
        // Retrieve informations about the slice processed by the worker
        int index = d->assignedTasks[wrkRank].index;
        int size = d->assignedTasks[wrkRank].size;
         // call to the wrapped function HERE
        (*_wrapped)( wrkRank );
        FlowShop instance = d->table()[ index ];
    }
};

struct UpdateArchive : public HandleResponseMultiStart<FlowShop>
{
    UpdateArchive(moeoUnboundedArchive<FlowShop> & _archive) : archive( _archive )
    {
    }

    void operator()(int wrkRank)
    {
        FlowShop individual;
        MultiStartData<FlowShop>& d = *_data;
        d.comm.recv( wrkRank, eo::mpi::Channel::Messages, individual );
        cout << "Response from worker " << wrkRank << endl;
        cout << individual << endl;
        d.bests.push_back( individual );
        archive.push_back( individual );
    }

private:
    moeoUnboundedArchive<FlowShop> & archive;
};

int main(int argc, char* argv[])
{
    // PARAMETRES
    // all parameters are hard-coded!
    const unsigned int SEED = 133742; // seed for random number generator
    const unsigned int MAX_GEN = 100; // Maximum number of generation before STOP

    try
    {
        Node::init( argc, argv );
        // eo::log << eo::setlevel( eo::debug );
        eo::log << eo::setlevel( eo::quiet );

        eoParser parser(argc, argv);  // for user-parameter reading
        eoState state;                // to keep all things allocated

        //make_parallel(parser);
        // help ?
        //make_help(parser);

        
        /*** the representation-dependent things ***/

        // The fitness evaluation
        eoEvalFuncCounter<FlowShop>& eval = do_make_eval(parser, state);
        // the genotype (through a genotype initializer)
        eoInit<FlowShop>& init = do_make_genotype(parser, state);
        // the variation operators
        eoGenOp<FlowShop>& op = do_make_op(parser, state);


        int rank = Node::comm().rank();
        DynamicAssignmentAlgorithm assign;
        
        // initialization of the population
        eoPop<FlowShop>& pop = do_make_pop(parser, state, init);
        // definition of the archive
        moeoUnboundedArchive<FlowShop> arch;
        // stopping criteria
        eoContinue<FlowShop>& term = do_make_continue_moeo(parser, state, eval);
        // output
        eoCheckPoint<FlowShop>& checkpoint = do_make_checkpoint_moeo(parser, state, eval, term, pop, arch);
        // algorithm
        eoAlgo<FlowShop>& algo = do_make_ea_moeo(parser, state, eval, checkpoint, op, arch);
        
        //MPI requirements
        eoGenContinue<FlowShop> continuator(MAX_GEN);
        /* Before a worker starts its algorithm, how does it reinits the population?
         * This one (ReuseSamePopEA) doesn't modify the population after a start, so
         * the same population is reevaluated on each multistart: the solution tend
         * to get better and better.
         */
        ReuseSamePopEA<FlowShop> resetAlgo( continuator, pop, eval );
        /**
         * How to send seeds to the workers, at the beginning of the parallel job?
         * This functors indicates that seeds should be random values.
         */
        GetRandomSeeds<FlowShop> getSeeds( SEED );
        // Builds the store
        MultiStartStore<FlowShop> store( algo, DEFAULT_MASTER, resetAlgo, getSeeds);
        store.wrapHandleResponse( new UpdateArchive( arch ) );

        // Creates the multistart job and runs it.
        // The last argument indicates that we want to launch 5 runs.
        MultiStart<FlowShop> msjob( assign, DEFAULT_MASTER, store, 1 );
        msjob.run();

        if( msjob.isMaster() )
        {
            msjob.best_individuals().sort();
            cout << "Global best individual:" << endl;
            cout << msjob.best_individuals().best_element() << endl;
            cout << "Archive: size ";
            arch.sortedPrintOn(cout);
        }

        // MultiStart< FlowShop > msjob10( assign, DEFAULT_MASTER, store, 10 );
        // msjob10.run();

        // if( msjob10.isMaster() )
        // {
        //     msjob10.best_individuals().sort();
        //     std::cout << "Global best individual has fitness " << msjob10.best_individuals().best_element() << std::endl;
        // }
        return 0;

        /*** Go ! ***/

        // // first evalution
        // apply<FlowShop>(eval, pop);

        // // printing of the initial population
        // cout << "Initial Population\n";
        // pop.sortedPrintOn(cout);
        // cout << endl;

        // // run the algo
        // do_run(algo, pop);

        // // printing of the final population
        // cout << "Final Population\n";
        // pop.sortedPrintOn(cout);
        // cout << endl;

        // // printing of the final archive
        // cout << "Final Archive\n";
        // arch.sortedPrintOn(cout);
        // cout << endl;


    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }
    return EXIT_SUCCESS;
}
