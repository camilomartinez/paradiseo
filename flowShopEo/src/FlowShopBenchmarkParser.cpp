/* 
* <FlowShopBenchmarkParser.cpp>
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

#include <stdexcept>
#include <cstdlib>
#include <FlowShopBenchmarkParser.h>

FlowShopBenchmarkParser::FlowShopBenchmarkParser(const std::string _benchmarkFileName)
{
    init(_benchmarkFileName);
}


const unsigned int FlowShopBenchmarkParser::getM()
{
    return M;
}


const unsigned int FlowShopBenchmarkParser::getN()
{
    return N;
}


const std::vector< std::vector<unsigned int> > FlowShopBenchmarkParser::getP()
{
    return p;
}


void FlowShopBenchmarkParser::printOn(std::ostream & _os) const
{
    _os << "M=" << M << " N=" << N << std::endl;
    _os << "*** processing times" << std::endl;
    for (unsigned int i=0; i<M; i++) {
        for (unsigned int j=0; j<N; j++) {
            _os << p[i][j] << " ";
        }
        _os << std::endl;
    }
    _os << std::endl << std::endl;
}


void FlowShopBenchmarkParser::init(const std::string _benchmarkFileName)
{
    std::string buffer;
    std::string::size_type start, end;
    std::ifstream inputFile(_benchmarkFileName.data(), std::ios::in);
    // opening of the benchmark file
    if (! inputFile)
    	throw std::runtime_error("*** ERROR : Unable to open the benchmark file");
    getline(inputFile, buffer, '\n');
    end = buffer.find_first_of(" ");
    // number of jobs (N)
    N = atoi(buffer.substr(0, start).data());
    start = buffer.find_first_not_of(" ");
    end = buffer.find_first_of(" ", start);
    // number of machines M
    M = atoi(buffer.substr(start, end-start).data());
    // processing times and due-dates
    p = std::vector< std::vector<unsigned int> >(M, std::vector<unsigned int>(N));
    // for each job...
    for (unsigned int j=0 ; j<N ; j++) {
        // index of the job (<=> j)
        getline(inputFile, buffer, '\n');
        // processing times of the job j on each machine
        getline(inputFile, buffer, '\n');
        start = buffer.find_first_not_of(" ");
        for (unsigned int i=0 ; i<M ; i++) {
            end = buffer.find_first_of(" ", start);
            p[i][j] = atoi(buffer.substr(start, end-start).data());
            start = buffer.find_first_not_of(" ", end);
        }
    }
    // closing of the input file
    inputFile.close();
}
