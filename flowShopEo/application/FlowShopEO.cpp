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
// random number generator
#include <utils/eoRNG.h>
// checks for help demand, and writes the status file and make_help; in libutils
void make_help(eoParser & _parser);
// definition of the representation
#include <FlowShop.h>

using namespace std;
using namespace eo::mpi;

struct SendBest : public SendTaskMultiStart<FlowShop>
{
public:
    using SendTaskFunction< MultiStartData< FlowShop > >::_data;

    void operator()( int wrkRank )
    {
        --(_data->runs);
        sendBestIndividual(wrkRank);
    }

private:
    void sendBestIndividual(int wrkRank)
    {
        FlowShop individual;
        // Any individual received so far?        
        if (_data->bests.size() > 0)
        {
            // Send one of the bests
            individual = eo::rng.choice(_data->bests);
        }
        _data->comm.send( wrkRank, eo::mpi::Channel::Messages, individual );
    }
};

struct RunAlgo : public ProcessTaskMultiStart<FlowShop>
{
    RunAlgo() : runCount(0)
    {
    }

    void operator()()
    {
        _data->resetAlgo( _data->pop );
        updateBest();
        beforeAlgo();
        _data->algo( _data->pop );
        logProgress();
        sendBest();
    }

private:
    void updateBest()
    {
        // Receive best element
        FlowShop globalBest;
        _data->comm.recv( _data->masterRank, eo::mpi::Channel::Messages, globalBest );
        // Update if better, similar to weak elitism
        // Check if received individual is valid and if local is worse (always maximizing)
        if (!globalBest.invalid() && _data->pop.best_element() < globalBest)
        {
            eo::log << eo::logging << "Updating best fitness with global best " << globalBest.fitness();
            eo::log << eo::logging << ", previous best was " << _data->pop.best_element().fitness() << std::endl;
            // Replace worst element
            typename eoPop<FlowShop>::iterator itWorse = _data->pop.it_worse_element();
            (*itWorse) = globalBest;
        }
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
        printBest();
    }

    void printPopulation()
    {
        int rank = Node::comm().rank();
        eo::log << eo::logging << "[W" << rank << "] Population run #" << runCount << " size: ";
        _data->pop.printOn(eo::log << eo::logging);
    }

    void printBest()
    {
        int rank = Node::comm().rank();
        eo::log << eo::logging << "[W" << rank << "] Best fitness run #" << runCount << " " << _data->pop.best_element().fitness() << std::endl;
    }

    void sendBest()
    {
        _data->comm.send( _data->masterRank, eo::mpi::Channel::Messages, _data->pop.best_element() );
    }

    int runCount;
};

struct UpdateBest : public HandleResponseMultiStart<FlowShop>
{
    void operator()(int wrkRank)
    {
        int master = Node::comm().rank();
        FlowShop workerBest;
        _data->comm.recv( wrkRank, eo::mpi::Channel::Messages, workerBest );
        eoPop<FlowShop>& globalBests = _data->bests;
        if (globalBests.size() == 0)
        {
            eo::log << eo::logging << "[M" << master << "] ";
            eo::log << eo::logging << "Initial best from W" << wrkRank << " fitness " << workerBest.fitness() << std::endl;
            // Add first individual to bests
            globalBests.push_back(workerBest);
            return;
        }
        FlowShop globalBest = globalBests.best_element();        
        // Is it better?
        if (globalBest < workerBest)
        {
            eo::log << eo::logging << "[M" << master << "] ";
            eo::log << eo::logging << "Updating best from W" << wrkRank << " fitness " << workerBest.fitness();
            eo::log << eo::logging << ", previous was " << globalBest.fitness() << std::endl;
            // Clear all elements
            globalBests.clear();
            globalBests.push_back(workerBest);
        } else if (globalBest.fitness() == workerBest.fitness()) { // Is it equally good but different
            bool isDifferent = false;
            for (unsigned i=0; i < globalBests.size(); i++) {
                if ( globalBests[i] != workerBest ) {
                    isDifferent = true;
                    break;
                }
            }
            if (isDifferent) {
                eo::log << eo::logging << "[M" << master << "] ";
                eo::log << eo::logging << "Adding new best from W" << wrkRank << " fitness " << workerBest.fitness() << std::endl;
                globalBests.push_back(workerBest);
            }
        }     
    }
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
        store.wrapSendTask( new SendBest() );
        store.wrapProcessTask( new RunAlgo() );
        store.wrapHandleResponse( new UpdateBest() );
        
        DynamicAssignmentAlgorithm assign;
        // How many runs (if missing, should have been set)
        int numberOfRuns = parser.valueOf<unsigned int>("numRuns");
        // Creates the multistart job and runs it.
        // The last argument indicates how many runs to launch.
        MultiStart<FlowShop> msjob( assign, DEFAULT_MASTER, store, numberOfRuns );

        // called after all parameters have been read
        make_help(parser);

        // Execute the job
        msjob.run();

        if( msjob.isMaster() )
        {
            eo::log << eo::logging << std::endl;
            eo::log << eo::logging << "Best fitness found: " << msjob.best_individuals().best_element().fitness()  << std::endl;
            eo::log << eo::logging << "Best individuals: size ";
            msjob.best_individuals().sortedPrintOn(eo::log << eo::logging);
        }
        return 0;
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }
    return EXIT_SUCCESS;
}
