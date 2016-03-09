/* 
* <make_eval_FlowShop.h>
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

#ifndef MAKE_EVAL_FLOWSHOP_H_
#define MAKE_EVAL_FLOWSHOP_H_


#include <math.h>
#include <utils/eoParser.h>
#include <utils/eoState.h>
#include <mpi/eoMpiNode.h>
#include <eoEvalFuncCounter.h>
#include <FlowShop.h>
#include <FlowShopBenchmarkParser.h>
#include <FlowShopEval.h>

/*
 * This function creates an eoEvalFuncCounter<eoFlowShop> that can later be used to evaluate an individual.
 * @param eoParser& _parser  to get user parameters
 * @param eoState& _state  to store the memory
 */
eoEvalFuncCounter<FlowShop> & do_make_eval(eoParser& _parser, eoState& _state)
{
    // benchmark file name
    std::string benchmarkFileName = _parser.getORcreateParam(std::string(), "BenchmarkFile", "Benchmark file name", 'B',"Representation", true).value();
    if (benchmarkFileName == "") {
        std::string stmp = "*** Missing name of the benchmark file\n";
        stmp += "    Type '-B=the_benchmark_file_name' or '--BenchmarkFile=the_benchmark_file_name'\n";
        stmp += "    Benchmarks files are available at mistic.heig-vd.ch/taillard/problemes.dir/ordonnancement.dir/ordonnancement.html";
        throw std::runtime_error(stmp.c_str());
    }
    // reading of the parameters contained in the benchmark file
    FlowShopBenchmarkParser fParser(benchmarkFileName);
    unsigned int M = fParser.getM();
    unsigned int N = fParser.getN();
    std::vector< std::vector<unsigned int> > p = fParser.getP();
    // Debug benchmark file reading
    eo::log << eo::debug << "Benchmark: " << benchmarkFileName << std::endl;
    fParser.printOn(eo::log << eo::xdebug);
    // reading potential additional information
    eoValueParam<bool> autoTotalTimeParam = _parser.getORcreateParam(false, "autoTotalTime", "Set total time according to benchmark", '\0', "Multistart");
    eoValueParam<unsigned int>& numRunsParam = _parser.createParam((unsigned int)0, "numRuns", "Number of times the algo is started. If missing, twice the number of workers", '\0', "Multistart");
    unsigned int nWorkers = eo::mpi::Node::comm().size() - 1;
    if (autoTotalTimeParam.value()) {
        if (!_parser.isItThere(numRunsParam)) {
            numRunsParam.value( nWorkers * 2 );
        } 
        unsigned int numRuns = numRunsParam.value();
        if (numRuns < 1) {
            std::string stmp = "Number of runs (numRuns) should be positive";
            throw std::runtime_error(stmp.c_str());
        }        
        // termination is n*(m/2)*60 milliseconds
        // taken from Vallada et al. (2007)
        // "Cooperative metaheuristics for the permutation flowshop scheduling problem"
        unsigned long totalTime = M * N * 30; // in milliseconds
        unsigned long eachRunTime = (unsigned long) round(totalTime / double(numRuns) / 1000); // in seconds
        eoValueParam<unsigned long>& maxTimeParam = _parser.setORcreateParam(eachRunTime, "maxTime", "Maximum running time in seconds set from eval", 'T', "Stopping criterion");
        unsigned long wallclockTime = (unsigned long) ceil(numRuns / double(nWorkers)) * eachRunTime;
        // Debug total run time
        eo::log << eo::debug << "Number of runs: " << numRuns << std::endl;
        eo::log << eo::debug << "Expected total time: " << (eachRunTime * numRuns) << ", max time (each run): " << eachRunTime << ", expected wallclock time: " << wallclockTime << std::endl;        
    }    
    // build of the initializer (a pointer, stored in the eoState)
    FlowShopEval* plainEval = new FlowShopEval(M, N, p);
    // turn that object into an evaluation counter
    eoEvalFuncCounter<FlowShop>* eval = new eoEvalFuncCounter<FlowShop> (* plainEval);
    // store in state
    _state.storeFunctor(eval);
    // and return a reference
    return *eval;
}

#endif /*MAKE_EVAL_FLOWSHOP_H_*/
