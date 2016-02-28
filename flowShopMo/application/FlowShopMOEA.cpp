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
// serializable population
#include <FlowShopSerializablePop.h>

using namespace std;
using namespace eo::mpi;

struct SendArchive : public SendTaskMultiStart<FlowShop>
{
public:
    using SendTaskFunction< MultiStartData< FlowShop > >::_data;

    SendArchive(moeoUnboundedArchive<FlowShop> & _archive) : archive( _archive )
    {
    }

    void operator()( int wrkRank )
    {
        --(_data->runs);
        FlowShopSerializablePop serializablePop(archive);
        _data->comm.send( wrkRank, eo::mpi::Channel::Messages, serializablePop );
    }

private:
    moeoUnboundedArchive<FlowShop> & archive;
};

struct RunAlgo : public ProcessTaskMultiStart<FlowShop>
{
    RunAlgo(moeoUnboundedArchive<FlowShop> & _archive) : archive( _archive ), runCount(0)
    {
    }

    void operator()()
    {
        _data->resetAlgo( _data->pop );
        updateArchive();
        beforeAlgo();
        _data->algo( _data->pop );
        logProgress();
        sendArchive();
    }

private:
    void updateArchive()
    {
        // Receive master archive
        FlowShopSerializablePop masterArchive(archive);
        _data->comm.recv( _data->masterRank, eo::mpi::Channel::Messages, masterArchive );
        archive( masterArchive );
    }

    void beforeAlgo()
    {
        if (runCount == 0)
        {
            // Print initial population
            printPopulation();
        }
        ++runCount;
        eo::log << eo::logging << std::endl << "[W" << Node::comm().rank() << "] Run #" << runCount << std::endl;        
    }

    void logProgress()
    {
        printPopulation();
        printArchive();
    }

    void printPopulation()
    {
        int rank = Node::comm().rank();
        eo::log << eo::logging << "[W" << rank << "] Population run #" << runCount << " size: ";
        _data->pop.printOn(eo::log << eo::logging);
    }

    void printArchive()
    {
        int rank = Node::comm().rank();
        eo::log << eo::logging << "[W" << rank << "] Archive run #" << runCount << " size: ";
        archive.printOn(eo::log << eo::logging);
    }

    void sendArchive()
    {
        FlowShopSerializablePop workerArchive(archive);
        _data->comm.send( _data->masterRank, eo::mpi::Channel::Messages, workerArchive );
    }

    moeoUnboundedArchive<FlowShop> & archive;
    int runCount;
};

struct UpdateArchive : public HandleResponseMultiStart<FlowShop>
{
    UpdateArchive(moeoUnboundedArchive<FlowShop> & _archive) : archive( _archive )
    {
    }

    void operator()(int wrkRank)
    {
        FlowShopSerializablePop serializablePop;
        MultiStartData<FlowShop>& d = *_data;
        d.comm.recv( wrkRank, eo::mpi::Channel::Messages, serializablePop );
        archive( serializablePop );        
    }

private:
    moeoUnboundedArchive<FlowShop> & archive;
};

void logToFile() {
    std::ostringstream logfile;
    int rank = Node::comm().rank();
    bool isMaster = rank == DEFAULT_MASTER;
    logfile << "log_" << (isMaster ? "m_" : "w_") << rank << ".txt";
    eo::log << eo::file(logfile.str());
}

int main(int argc, char* argv[])
{
    try
    {
        Node::init( argc, argv );
        // General log level
        // eo::log << eo::setlevel( eo::logging );
        eo::log << eo::setlevel( eo::debug );
        int rank = Node::comm().rank();
        if (rank == DEFAULT_MASTER)
        {
            // master specific init
            eo::log << eo::setlevel( eo::debug );
            //logToFile();
        }
        else
        {
            // workers specific init
            logToFile();
        }


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

        // initialization of the population
        eoPop<FlowShop>& pop = do_make_pop(parser, state, init);
        // definition of the archive
        moeoUnboundedArchive<FlowShop> arch;
        // stopping criteria
        eoCombinedContinue<FlowShop>& term = do_make_continue_moeo(parser, state, eval);
        // output
        eoCheckPoint<FlowShop>& checkpoint = do_make_checkpoint_moeo(parser, state, eval, term, pop, arch);
        // algorithm
        eoAlgo<FlowShop>& algo = do_make_ea_moeo(parser, state, eval, checkpoint, op, arch);
        
        // MPI requirements
        /* Before a worker starts its algorithm, how does it reinits the population?
         * This one (ReuseSamePopEA) doesn't modify the population after a start, so
         * the same population is reevaluated on each multistart: the solution tend
         * to get better and better.
         */
        ReuseSameRandomPopEA<FlowShop> resetAlgo( term, pop.size(), init, eval );
        /**
         * How to send seeds to the workers, at the beginning of the parallel job?
         * This functors indicates that seeds should be random values.
         */
        DummyGetSeeds<FlowShop> getSeeds;
        // Builds the store
        MultiStartStore<FlowShop> store( algo, DEFAULT_MASTER, resetAlgo, getSeeds);
        store.wrapSendTask( new SendArchive( arch ) );
        store.wrapProcessTask( new RunAlgo( arch ) );
        store.wrapHandleResponse( new UpdateArchive( arch ) );
        
        DynamicAssignmentAlgorithm assign;
        // Creates the multistart job and runs it.
        // The last argument indicates how many runs to launch.
        MultiStart<FlowShop> msjob( assign, DEFAULT_MASTER, store, 20 );
        msjob.run();

        if( msjob.isMaster() )
        {
            eo::log << eo::logging << std::endl;
            eo::log << eo::logging << "Archive: size ";
            arch.sortedPrintOn(eo::log << eo::logging);
        }
        return 0;
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }
    return EXIT_SUCCESS;
}
