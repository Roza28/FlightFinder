#include "CENGFlight.h"
#include <iostream>

//=======================//
// Implemented Functions //
//=======================//
void CENGFlight::PrintCanNotHalt(const std::string& airportFrom,
                                 const std::string& airportTo,
                                 const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines is not found and cannot be halted"
              << std::endl;
}

void CENGFlight::PrintCanNotResumeFlight(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines cannot be resumed"
              << std::endl;
}

void CENGFlight::PrintFlightFoundInCache(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         bool isCostWeighted)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  " is found in cache." << std::endl;
}

void CENGFlight::PrintFlightCalculated(const std::string& airportFrom,
                                       const std::string& airportTo,
                                       bool isCostWeighted)
{
    std::cout << "A flight path is calculated between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  "." << std::endl;
}

void CENGFlight::PrintPathDontExist(const std::string& airportFrom,
                                    const std::string& airportTo)
{
    std::cout << "A flight path does not exists between \""
              << airportFrom << "\" and \""
              << airportTo <<"\"." << std::endl;
}

void CENGFlight::PrintSisterAirlinesDontCover(const std::string& airportFrom)
{
    std::cout << "Could not able to generate sister airline list from \""
              << airportFrom <<"\"." << std::endl;
}

void CENGFlight::PrintMap()
{
    navigationMap.PrintEntireGraph();
}

void CENGFlight::PrintCache()
{
    lruTable.PrintTable();
}

CENGFlight::CENGFlight(const std::string& flightMapPath)
    : navigationMap(flightMapPath)
{}


//=======================//
//          TODO         //
//=======================//
void CENGFlight::HaltFlight(const std::string& airportFrom,
                            const std::string& airportTo,
                            const std::string& airlineName)
{
    /* TODO */
    std::vector<GraphVertex> vertexList = navigationMap.get_vertexList();
    int IndexFrom = navigationMap.search_vertex(airportFrom);
    int IndexTo = navigationMap.search_vertex(airportTo);
    if (IndexFrom == -1 || IndexTo == -1){
        PrintCanNotHalt(airportFrom,airportTo,airlineName);
        return;  
    }
    int edgeIndex = navigationMap.search_edge(airlineName,vertexList[IndexFrom],IndexTo);
    if (edgeIndex == -1){
        PrintCanNotHalt(airportFrom,airportTo,airlineName);
        return; 
    }
    
    HaltedFlight halted;
    halted.airportFrom = airportFrom;  halted.airportTo = airportTo; 
    halted.airline = airlineName; halted.w0 = navigationMap.get_weight_w0(airlineName,IndexFrom,IndexTo); 
    halted.w1 = navigationMap.get_weight_w1(airlineName,IndexFrom,IndexTo);
    haltedFlights.push_back(halted);
    navigationMap.RemoveEdge(airlineName,airportFrom,airportTo);
    
}

    // (Direct Function call)
void CENGFlight::ContinueFlight(const std::string& airportFrom,
                                const std::string& airportTo,
                                const std::string& airlineName)
{
    /* TODO */
    std::vector<GraphVertex> vertexList = navigationMap.get_vertexList();
    int IndexFrom = navigationMap.search_vertex(airportFrom);
    int IndexTo = navigationMap.search_vertex(airportTo);
    if (IndexFrom == -1 || IndexTo == -1){
        PrintCanNotResumeFlight(airportFrom,airportTo,airlineName);
        return; 
    }
    int edgeIndex = navigationMap.search_edge(airlineName,vertexList[IndexFrom],IndexTo);
    if (edgeIndex != -1){
        PrintCanNotResumeFlight(airportFrom,airportTo,airlineName);
        return; 
    }
    int indexHalted;
    for (int i = 0; i < haltedFlights.size(); i++){
        if (airportFrom == haltedFlights[i].airportFrom && airportTo == haltedFlights[i].airportTo && airlineName == haltedFlights[i].airline)
            indexHalted = i;
    }
    navigationMap.AddEdge(airlineName,airportFrom,airportTo,haltedFlights[indexHalted].w0,haltedFlights[indexHalted].w1);
    
    
}

void CENGFlight::FindFlight(const std::string& startAirportName,
                            const std::string& endAirportName,
                            float alpha)
{
    /* TODO */
    // ASK CHECKS, CHECK AND DELETE LRU
    int IndexFrom = navigationMap.search_vertex(startAirportName);
    int IndexTo = navigationMap.search_vertex(endAirportName);
    if (IndexFrom == -1 || IndexTo == -1)
    {
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    std::vector<int> path;
    bool cache;
    bool isPath;
    if (alpha == 0.0f){
        cache = lruTable.Find(path,IndexFrom,IndexTo,true,true); // ASK IF LAST SHOULD BE TRUE OR NOT
        if (cache){
            PrintFlightFoundInCache(startAirportName,endAirportName,true);
            navigationMap.PrintPath(path,alpha,true);
            return;
        }
    }
    if (alpha == 1.0f){
        cache = lruTable.Find(path,IndexFrom,IndexTo,false,true);
        if (cache){
            PrintFlightFoundInCache(startAirportName,endAirportName,false);
            navigationMap.PrintPath(path,alpha,true);
            return;
        }
    }
    isPath = navigationMap.HeuristicShortestPath(path,startAirportName,endAirportName,alpha);
    if (!isPath){
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    if (alpha == 0.0f){
        PrintFlightCalculated(startAirportName,endAirportName,true);
        navigationMap.PrintPath(path,alpha,true);
        if (lruTable.get_elementCount() == (FLIGHT_TABLE_SIZE + 1)/2)
            lruTable.RemoveLRU(1);
        lruTable.Insert(path,true);
        return;
    }
    if (alpha == 1.0f){
        PrintFlightCalculated(startAirportName,endAirportName,false);
        navigationMap.PrintPath(path,alpha,true);
        if (lruTable.get_elementCount() == (FLIGHT_TABLE_SIZE + 1)/2)
            lruTable.RemoveLRU(1);
        lruTable.Insert(path,false);
        return;
    }
    navigationMap.PrintPath(path,alpha,true);
}

void CENGFlight::FindSpecificFlight(const std::string& startAirportName,
                                    const std::string& endAirportName,
                                    float alpha,
                                    const std::vector<std::string>& unwantedAirlineNames) const
{
    /* TODO */
    // ASK CHECKS
    int IndexFrom = navigationMap.search_vertex(startAirportName);
    int IndexTo = navigationMap.search_vertex(endAirportName);
    if (IndexFrom == -1 || IndexTo == -1)
    {
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    std::vector<int> path;
    bool isPath;
    isPath = navigationMap.FilteredShortestPath(path,startAirportName,endAirportName,alpha,unwantedAirlineNames);
    if (!isPath){
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    navigationMap.PrintPath(path,alpha,true);

}

void CENGFlight::FindSisterAirlines(std::vector<std::string>& airlineNames,
                               const std::string& startAirlineName,
                               const std::string& airportName) const
{
    /* TODO */
    int indexAirport = navigationMap.search_vertex(airportName);
    if (indexAirport == -1){
        PrintSisterAirlinesDontCover(airportName);
        return;
    }
    int trueCounter = 1;
    std::vector<std::string> tmp;
    MinHeap<int> pq;
    std::vector<GraphVertex> vertexList = navigationMap.get_vertexList();
    std::vector<bool> visited(vertexList.size(),false);
    visited[indexAirport] = true;
    pq.push(indexAirport);
    while(!pq.empty()){
        int v = pq.top();
        pq.pop();
        for (int i = 0; i < vertexList[v].edges.size(); i++){
            if (vertexList[v].edges[i].name == startAirlineName && !visited[vertexList[v].edges[i].endVertexIndex]){
                pq.push(vertexList[v].edges[i].endVertexIndex);
                visited[vertexList[v].edges[i].endVertexIndex] = true;
                trueCounter++;
            }
        }
    }
    
    // Check if we reach anywhere
    bool reached = false;
    for (int i = 0; i < visited.size(); i++){
        if (i == indexAirport)
            continue;
        if (visited[i]){
            reached = true;
            break;
        }
    }
    if (!reached){
        PrintSisterAirlinesDontCover(airportName);
        return;
    }
    int previous = -1;
    tmp.push_back(startAirlineName);
    while((trueCounter != visited.size()) && (trueCounter != previous)){
        previous = trueCounter;
        
        // SELECT A VISITED AIRPORT WITH MAX NON-VISITED NEIGHBOUR
        int index_maxnvn = -1;
        int maxnvn = -1;
        for (int i = 0; i < visited.size(); i++)
        {
            if (visited[i])
            {
                std::vector<bool> nonVisitedNeighbors(visited.size(), false);
        
                for (int j = 0; j < vertexList[i].edges.size(); j++)
                {
                    if (!visited[vertexList[i].edges[j].endVertexIndex])
                    {
                        nonVisitedNeighbors[vertexList[i].edges[j].endVertexIndex] = true;
                    }
                }
        
                int counter = 0;
                for (int j = 0; j < nonVisitedNeighbors.size(); j++)
                {
                    if (nonVisitedNeighbors[j])
                    {
                        counter++;
                    }
                }
        
                if (counter > maxnvn)
                {
                    index_maxnvn = i;
                    maxnvn = counter;
                }
            }
        }
        
        
        int j; //j holds non-utilized airline
        for (j = 0; j < vertexList[index_maxnvn].edges.size();j++){
            bool flag = true;
            for (int k = 0 ; k < tmp.size();k++){
                if (tmp[k] == vertexList[index_maxnvn].edges[j].name)
                    flag = false;
            }
            if (flag && !visited[vertexList[index_maxnvn].edges[j].endVertexIndex])
                break;
        }
        if (j == vertexList[index_maxnvn].edges.size()){
            PrintSisterAirlinesDontCover(airportName);
            return;
        }
        
        // HERE NEEDS TO Visit every non-visited airport only using this and previously found airline. Add this airline to the sister airline list.
        tmp.push_back(vertexList[index_maxnvn].edges[j].name);
        for (int n = 0; n < visited.size();n++){
            if (visited[n]){
                pq.push(n);
                while(!pq.empty()){
                    int v = pq.top();
                    pq.pop();
                    for (int i = 0; i < vertexList[v].edges.size(); i++){
                        bool myflag = false;
                        for (int k = 0 ; k < tmp.size();k++){
                            if (tmp[k] == vertexList[v].edges[i].name){
                                myflag = true;
                                break;
                            }        
                        }
                        if (myflag && !visited[vertexList[v].edges[i].endVertexIndex]){
                            pq.push(vertexList[v].edges[i].endVertexIndex);
                            visited[vertexList[v].edges[i].endVertexIndex] = true;
                            trueCounter++;
                        }
                    }
                }
            }
        }
    }

    if (trueCounter == visited.size()){
        airlineNames.resize(tmp.size() - 1);
        for (int i = 0; i < tmp.size() - 1;i++)
            airlineNames[i] = tmp[i + 1];
        return;
    }
    if (trueCounter == previous){
        PrintSisterAirlinesDontCover(airportName);
        return;
    }
}

int CENGFlight::FurthestTransferViaAirline(const std::string& airportName,
                                           const std::string& airlineName) const
{
    /* TODO */
    int IndexVertex = navigationMap.search_vertex(airportName);
    if (IndexVertex == -1)
        return -1;
    return navigationMap.MaxDepthViaEdgeName(airportName,airlineName);
}