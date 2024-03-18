#include "CENGFlight.h"
#include "MultiGraph.h"
#include "HashTable.h"
#include <iostream>
#include "IntPair.h"
void InsertEdge(MultiGraph& graph,
                const std::string& E,
                const std::string& V0,
                const std::string& V1,
                float w0, float w1)
{
    std::cout << "Inserting \"" << E << "\" (w0: "
              << w0 << ", w1: " << w1 << ") to "<< V0 << "->" << V1 << std::endl;
    graph.AddEdge(E, V0, V1, w0, w1);
}

void decrease_priority(MinPairHeap<float,int> &pq,float new_dist,int index){
    MinPairHeap<float,int> tmp;
    while (!pq.empty()){
        Pair<float,int> v = pq.top();
        if (v.value == index)
            v.key = new_dist;
        tmp.push(v);
        pq.pop();
    }
    while(!tmp.empty()){
        Pair<float,int> v = tmp.top();
        pq.push(v);
        tmp.pop();
    }
}


int main(int argc, const char* argv[])
{
    // For testing from VPL
    //  ...
    /*std::cout << "Testing Max Depth Via Edge" << std::endl;
    std::cout << std::endl;*/

    MultiGraph graph;

    std::cout << "Inserting \"A\"" << std::endl;
    graph.InsertVertex("A");
    std::cout << "Inserting \"B\"" << std::endl;
    graph.InsertVertex("B");
    std::cout << "Inserting \"C\"" << std::endl;
    graph.InsertVertex("C");
    std::cout << "Inserting \"D\"" << std::endl;
    graph.InsertVertex("D");
    std::cout << "Inserting \"E\"" << std::endl;
    graph.InsertVertex("E");
    std::cout << "Inserting \"F\"" << std::endl;
    graph.InsertVertex("F");

    InsertEdge(graph, "E01", "A", "B", 2.3f, 4.12f);
    InsertEdge(graph, "E02", "A", "B", 2.3f, 4.12f);
    InsertEdge(graph, "E02", "B", "C", 1.2f, 2.12f);
    InsertEdge(graph, "E02", "C", "E", 0.2f, 0.02f);
    InsertEdge(graph, "E01", "E", "F", 0.1f, 4.122f);
    
    

    //graph.PrintEntireGraph();
    
    
    
    
    
    
    return 0;
}