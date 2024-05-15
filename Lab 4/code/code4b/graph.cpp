/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <format>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits
#include <algorithm>   // std::make_heap(), std::pop_heap(), std::push_heap()

#include "graph.h"
#include "dsets.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

// Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(n + 1), size{n}, n_edges{0} {
    assert(n >= 1);
}

Graph::Graph(const std::vector<Edge> &V, int n) : Graph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        if (auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                                   [e1](const Edge &ed) { return e1.links_same_nodes(ed); });
            it == end(T[e1.from])) {
            T[e1.from].push_back(e1);  // insert new edge e1
            ++n;                       // increment the counter of edges
        } else {
            it->weight = e1.weight;  // update the weight
        }
    };

    edge_insertion(e);
    edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                               [e1](const Edge &ed) { return e1.links_same_nodes(ed); });

        assert(it != end(T[e1.from]));
        T[e1.from].erase(it);  // remove edge e1
        --n;                   // decrement the counter of edges
    };

    edgeRemoval(e);
    edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
    std::vector<int> dist(size + 1, std::numeric_limits<int>::max());
    std::vector<int> path(size + 1, 0);
    std::vector<bool> done(size + 1, false);

    // *** TODO ***

    // Initalize the start node, in this case it's v1.
    int v = 1;
    dist[v] = 0;
    done[v] = true;

    // Initalize the total weight of the spanning tree
    int totalWeight = 0;

    while (true) {
        for (auto& e : table[v]) {
            int u = e.to;

            if (done[u] == false && dist[u] > e.weight) {
                
                dist[u] = e.weight;
                path[u] = v;
            }
        }

        // Find smallest undone distance vertex by checking every edge.
        // If only adjacent edges are checked we lose possible smaller edges
        // which would'nt result in a minimum spanning tree

        int min = std::numeric_limits<int>::max();

        for (size_t u = 1; u <= size; u++) {
            if (done[u] == false && dist[u] < min) {
                min = dist[u];
                v = u;
            }
        }


        if (min == std::numeric_limits<int>::max()) break;
        done[v] = true;

        // Print current edge and it's weight
        std::cout << "( " << path[v] << " ," << v << " ," << dist[v] << " )\n";
        totalWeight += dist[v];
    }
    // Print the total weight of the minimum spanning tree
    std::cout << "\n total weight = " << totalWeight << "\n";

}

// Kruskal's minimum spanning tree algorithm
void Graph::mstKruskal() const {
    // *** TODO ***
    
    std::vector<Edge> edgeVec;
    edgeVec.reserve(size - 1);

    
    // Compare each edge to one another and create a vector of the smallest edges
    for (int v = 1; v <= size; v++) {
        for (auto const& e : table[v]) {
            if (e.from < e.to) {
                edgeVec.push_back(e);
            }
        }
    }

    DSets ds(size);
    // Create min_heap from the vector of edges
    std::ranges::make_heap(edgeVec, std::greater<Edge>{});

    int totalWeight = 0;
    int counter = 0;

    while (counter < size - 1) {
        // The smallest edge will be in the front of the min_heap, this edge is saved
        Edge smallestEdge = edgeVec.front();
        
        // Remove the saved smallestEdge from the min_heap
        std::ranges::pop_heap(edgeVec, std::greater<Edge>{});
        edgeVec.pop_back();

        // Find the smallestEdge from and to verticies
        int smallestEdgeFrom = ds.find(smallestEdge.from);
        int smallestEdgeTo = ds.find(smallestEdge.to);

        // Perform union by size if from and to are diffrent verticies
        if (smallestEdgeFrom != smallestEdgeTo) {
            ds.join(smallestEdgeFrom, smallestEdgeTo);
            std::cout << smallestEdge << "\n";
            totalWeight += smallestEdge.weight;
            counter++;
        }
    }
    std::cout << "\n total weight = " << totalWeight << "\n";
}

// print graph
void Graph::printGraph() const {
    std::cout << std::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << std::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; v++) {
        std::cout << std::format("{:4} : ", v);
        for (auto const &e : table[v]) {
            std::cout << std::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << std::format("{:-<66}\n", '-');
}
