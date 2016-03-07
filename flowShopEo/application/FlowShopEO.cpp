// eo general include
#include <EO.h>
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
#include <do/make_continue.h>
// outputs (stats, population dumps, ...)
#include <do/make_checkpoint.h>
// evolution engine (selection and replacement)
#include <do/make_algo_scalar.h>
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
        // stopping criteria
        eoCombinedContinue<FlowShop>& term = do_make_continue(parser, state, eval);
        // output
        eoCheckPoint<FlowShop>& checkpoint = do_make_checkpoint(parser, state, eval, term);
        // algorithm
        eoAlgo<FlowShop>& algo = do_make_algo_scalar(parser, state, eval, checkpoint, op);
        
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
        
        DynamicAssignmentAlgorithm assign;
        // Creates the multistart job and runs it.
        // The last argument indicates how many runs to launch.
        MultiStart<FlowShop> msjob( assign, DEFAULT_MASTER, store, 1 );
        msjob.run();

        if( msjob.isMaster() )
        {
            eo::log << eo::logging << std::endl;
            eo::log << eo::logging << "Best individuals: size ";
            msjob.best_individuals().sortedPrintOn(eo::log << eo::logging);
            eo::log << eo::logging << std::endl << "Global best individual " << std::endl << msjob.best_individuals().best_element() << std::endl;
        }
        return 0;
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }
    return EXIT_SUCCESS;
}
