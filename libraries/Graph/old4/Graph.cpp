// Graph impl file


#include "Graph.h"

using namespace std;

Graph::Graph(){
//    m_currentSize = 0;
    
    for(uint8_t i = 0; i < MAX_MAZE_SIZE; i++)
        m_dist[i] = INT_MAX, m_sptSet[i] = false;
    
    m_parent[STARTING_NODE] = -1;
    m_dist[STARTING_NODE] = 0;

    for(uint8_t i = 0; i < MAX_MAZE_SIZE; i++){
        for(uint8_t j = 0; j < MAX_MAZE_SIZE; j++){
            graph[i][j] = 0;
        }
    }



}

Graph::~Graph(){
    // empty destructor
}

void Graph::addEdge(int u, int v, int w){
/*
    m_adj[u].push_back(make_pair(v,w));
    m_adj[v].push_back(make_pair(u,w));
    m_currentSize++;
*/
    graph[u][v] = w;
    graph[v][u] = w;
}

/*
void Graph::printGraph(){
    int v, w;
    for(int i = 0; i < MAX_MAZE_SIZE; i++){
        if(m_adj[i].begin() != m_adj[i].end()){
            cout << "Node " << i << " makes an edge with \n";
            
            for( auto it = m_adj[i].begin(); it!=m_adj[i].end(); it++){
                v = it->first;
                w = it->second;
                cout << "\tNode " << v << " with edge weight = " << w << "\n";
            }
            cout << "\n";
        }
    }
}
*/
void Graph::printPath(int j){
    if(m_parent[j] == -1)
        return;

    printPath(m_parent[j]);

    cout << j << F(" ");
}

void Graph::printSolution(){
    int src = 0;
    cout << F("Node\tDistance\tPath") << endl;
    for(int i = 1; i < MAX_MAZE_SIZE; i++){
        if(m_dist[i] != INT_MAX){
            cout << src << F("->") << i;
            cout << F("\t") << m_dist[i];
            cout << F("\t\t") << src << F(" ");
            printPath(i);
            cout << F("\n");
        }
    }
}
/*
void Graph::storeEndPath(int j){
    
    if(m_parent[j] == -1)
        return;

    storeEndPath(m_parent[j]);
    
    //cout << j << " ";
    m_shortestPath.push_back(j);
}
*/
void Graph::Dijkstra(){
    // convert m_adj to [49][49] adjacency matrix
    //bool graph[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

    int u, v, w;
    // perform Dijkstras algorithm
    for(int count = 0; count < MAX_MAZE_SIZE-1; count++){
        int u = minDistance();
        m_sptSet[u] = true;
        
        for(int x = 0; x < MAX_MAZE_SIZE; x++){

        if(!m_sptSet[x] && graph[u][x] && m_dist[u] != INT_MAX && m_dist[u] + graph[u][x] < m_dist[x]){
                m_parent[x] = u;
                m_dist[x] = m_dist[u] + graph[u][x];
            }
        }
    }

    printSolution();
}

/*
void Graph::setEndIndex(int i){
    m_endIndex = i;
}
*/
int Graph::minDistance(){
    int min = INT_MAX, min_index;
    
    for(int v = 0; v < MAX_MAZE_SIZE; v++)
        if(m_sptSet[v] == false && m_dist[v] <= min)
            min = m_dist[v], min_index = v;

    return min_index;
}
/*
void Graph::printSolution(){
    cout << "Vertex\tDistance from Source\n";
    for(int i = 0; i < MAX_MAZE_SIZE; i++)
        if(m_dist[i] != INT_MAX)
            cout << i << "\t" << m_dist[i] << "\n";
}
*/
