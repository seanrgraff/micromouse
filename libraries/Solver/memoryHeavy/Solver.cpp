// Sean Graff
// 2/12/18
//
// Solver Impl. file

#include "Solver.h"

//using namespace std;

Solver::Solver()
{
    m_diagnostics = new Diagnostics();

    //m_diagnostics = diag;
    /*m_graph = new Graph();*/
    m_graph = new Graph();

    m_loco = new Locomotion();

    //m_diagnostics->celebrate();
    //m_loco->goForward();

    //m_loco = new Locomotion();

    m_currentPosition = STARTING_INDEX;

    m_difference = 0;
    m_turnCounter = 0;
    m_facing = N;

    m_startingNode = true;

    m_isSolved = false;

    m_isReadyToSprint = false;
}

Solver::~Solver(){
    delete m_diagnostics;
    delete m_graph;
    delete m_loco;
}

Diagnostics* Solver::getDiagnostics(){
    return m_diagnostics;
}

Locomotion* Solver::getLocomotion(){
    return m_loco;
}

void Solver::nextNode(){
    // visually represent how long this function takes by flashing the MODE led
    m_diagnostics->getModeLED()->turnON();

    int wallCount = 0;

    bool northIsWall;
    bool eastIsWall;
    bool westIsWall;
    bool southIsWall;

    //Serial.print("m_facing before: ");
    //Serial.println(m_facing);
    Serial.print(F("m_currentPosition before: "));
    Serial.println(m_currentPosition);

    Node* currentNode = &m_nodeContainer[m_currentPosition];
    // a maze is solved if distance check returns true or
    if(!m_isSolved)
        m_isSolved = ( m_diagnostics->update() );

    // BEGIN MAPPING CURRENT NODE

    // if the maze hasn't been solved
    if(!m_isSolved){
        // if the node hasn't been mapped before
        //if( !m_visited[m_currentPosition] ){
        if( !currentNode->visited ){
            Serial.println(F("Node has not been mapped before"));
            m_difference = 0;
            // if north has no wall
            if( !m_diagnostics->getNorthSensor()->isWall() ){
                // add an edge from the current node to the north node
                m_graph->addEdge( m_currentPosition, m_currentPosition+incrementNorth(), STRAIGHT_WEIGHT );
            }
            else{
                wallCount++;
                currentNode->northIsWall = true;
            }

            if( !m_diagnostics->getEastSensor()->isWall() ){
                m_graph->addEdge( m_currentPosition, m_currentPosition+incrementEast(), TURN_WEIGHT );
            }
            else{
                wallCount++;
                currentNode->eastIsWall = true;
            }

            if( !m_diagnostics->getWestSensor()->isWall() ){
                m_graph->addEdge(m_currentPosition, m_currentPosition+incrementWest(), TURN_WEIGHT);
            }
            else{
                wallCount++;
                currentNode->westIsWall = true;
                if(wallCount == 3){
                    currentNode->deadEnd = true;
                }
            }

            if(m_startingNode){
                m_startingNode = false;
                currentNode->southIsWall = true;
            }

            // save the direction the mouse was facing when this node was mapped
            currentNode->direction = m_facing;
            currentNode->visited = true;
        }
        else{
            //Serial.println("adjusting to m_facing stored from memory");


            // TODO: refine this to work perfectly
            m_difference = currentNode->direction - m_facing;
            //if(m_difference < -1) m_difference = m_difference * -1; // makeshift abs() function

            m_facing = currentNode->direction;
        }

        // END MAPPING NODE

        // BEGIN MOVEMENT DECISION
        Serial.println(m_difference);

        // update local variables to resemble current node
        northIsWall = currentNode->northIsWall;
        eastIsWall = currentNode->eastIsWall;
        westIsWall = currentNode->westIsWall;
        southIsWall = currentNode->southIsWall;

        // at this point a decision on where to move needs to be made
        // explore any unvisited nodes first -- clockwise priority excluding south

        if( !m_nodeContainer[m_currentPosition+incrementNorth()].visited && !northIsWall){
            m_currentPosition += incrementNorth();

            goForward();
        }
        else if( !m_nodeContainer[m_currentPosition+incrementEast()].visited && !eastIsWall){
            m_currentPosition += incrementEast();

            turnRight();
        }
        else if( !m_nodeContainer[m_currentPosition+incrementWest()].visited && !westIsWall){
            m_currentPosition += incrementWest();

            turnLeft();
        }
        else{
            // all routes the mouse could take have been explored
            // find shortest path leading to unvisited node
            findShortestUnvisitedPath();

            // follow the path to unvisited node
            traverseShortestPath();
        }

        // END MOVEMENT DECISION

        // visually represent when algorithm is 'done' working
        m_diagnostics->getModeLED()->turnOFF();

    }
    else if(!m_isReadyToSprint){
        // can celebrate and also perform DFS to find the shortest path, and visually represent sprint mode
        m_diagnostics->celebrate();
        m_diagnostics->getModeLED()->turnON();
        m_isSolved = true;

        // call the shortest path function to store directions
        m_isReadyToSprint = true;

        m_graph->Dijkstra(0, m_currentPosition);
        //m_graph->storePath(0, m_currentPosition); // store path from start to finish

        m_currentPosition = 0;
    }
    else{
        // button pressed AND shortest path is set in memory...
        // delay for 5 seconds after button press...
        for(int i = 0; i < 5; i++){
            m_diagnostics->getModeLED()->turnON();
            delay(500);
            m_diagnostics->getModeLED()->turnOFF();
            delay(500);
        }
        m_diagnostics->getModeLED()->turnON();


    }

    //Serial.print("m_facing after: ");
    //Serial.println(m_facing);
    Serial.print("m_currentPosition after: ");
    Serial.println(m_currentPosition);


}


// finds the shortest path to an unvisited node
void Solver::findShortestUnvisitedPath(){
    int min = MAX;
    int minIndex = 0;


    for(int i = 0; i < MAX_MAZE_SIZE; i++){
        if( !m_nodeContainer[i].visited ){
            if( m_graph->getDistance(i) <= min ){
                 min = m_graph->getDistance(i);
                 minIndex = i;
            }
        }
    }
    Serial.println(minIndex);

    m_graph->Dijkstra(m_currentPosition, minIndex); // shortest path is stored
}

void Solver::traverseShortestPath(){

    // while m_graph's shortest path vector is not empty
    while(!m_graph->isSPEmpty()){
        // grab the next index
        int nextIndex = m_graph->getNextSPIndex();

        // calculatre the orientational difference
        m_difference = m_nodeContainer[m_currentPosition].direction - m_facing;
        //if(m_difference < -1) m_difference = m_difference * -1; // makeshift abs() function

        Serial.println(m_difference);

        m_facing = m_nodeContainer[m_currentPosition].direction;

        Serial.print(F("going to: "));
        Serial.println(nextIndex);

        // determine which way to go in order
        // to get to the next index
        if( m_currentPosition+incrementNorth() == nextIndex){
            m_currentPosition += incrementNorth();

            goForward();
        }
        else if( m_currentPosition+incrementEast() == nextIndex){
            m_currentPosition += incrementEast();

            turnRight();
        }
        else if( m_currentPosition+incrementWest() == nextIndex){
            m_currentPosition += incrementWest();

            turnLeft();
        }
        else if( m_currentPosition+incrementSouth() == nextIndex){

            m_nodeContainer[m_currentPosition].deadEnd = true;

            m_currentPosition += incrementSouth();

            turnAround();

        }
        else{
            Serial.println(F("error tsp"));
            break;
        }
        //cout << "after: " << m_currentPosition << endl;
    }
}

// the increment functions add to the position relative to the mouses facing direction.
// when the mouse's north sensor is pointing(facing) east, moving the mouse "north" is actually moving
// the mouse object east one node in the graph

// these increment functions could be implemented with a modulous type circular function possible
int Solver::incrementNorth(){
//    Serial.println("Increment North called");
    switch(m_facing){
        case N:
            if(m_currentPosition+VERTICAL_INCREMENT < MAX_MAZE_SIZE)
                return VERTICAL_INCREMENT;
            else
                return VERTICAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case E:
            if(m_currentPosition+HORIZONTAL_INCREMENT < MAX_MAZE_SIZE)
                return HORIZONTAL_INCREMENT;
            else
                return HORIZONTAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case S:
            if(m_currentPosition-VERTICAL_INCREMENT < 0)
                return MAX_MAZE_SIZE-VERTICAL_INCREMENT;
            else
                return -VERTICAL_INCREMENT;
            break;
        case W:
            if(m_currentPosition-HORIZONTAL_INCREMENT >= 0)
                return -HORIZONTAL_INCREMENT;
            else
                return MAX_MAZE_SIZE-HORIZONTAL_INCREMENT;
            break;
    }
}

int Solver::incrementEast(){
//    Serial.println("Increment East called");
    switch(m_facing){
        case N:
            if(m_currentPosition+HORIZONTAL_INCREMENT < MAX_MAZE_SIZE)
                return HORIZONTAL_INCREMENT;
            else
                return HORIZONTAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case E:
            if(m_currentPosition-VERTICAL_INCREMENT < 0)
                return MAX_MAZE_SIZE-VERTICAL_INCREMENT;
            else
                return -VERTICAL_INCREMENT;
            break;
        case S:
            if(m_currentPosition-HORIZONTAL_INCREMENT >= 0)
                return -HORIZONTAL_INCREMENT;
            else
                return MAX_MAZE_SIZE-HORIZONTAL_INCREMENT;
            break;
        case W:
            if(m_currentPosition+VERTICAL_INCREMENT < MAX_MAZE_SIZE)
                return VERTICAL_INCREMENT;
            else
                return VERTICAL_INCREMENT-MAX_MAZE_SIZE;
            break;
    }
}

int Solver::incrementSouth(){
//    Serial.println("Increment South called");
    switch(m_facing){
        case N:
            if(m_currentPosition-VERTICAL_INCREMENT < 0)
                return MAX_MAZE_SIZE-VERTICAL_INCREMENT;
            else
                return -VERTICAL_INCREMENT;
            break;
        case E:
            if(m_currentPosition-HORIZONTAL_INCREMENT >= 0)
                return -HORIZONTAL_INCREMENT;
            else
                return MAX_MAZE_SIZE-HORIZONTAL_INCREMENT;
            break;
        case S:
            if(m_currentPosition+VERTICAL_INCREMENT < MAX_MAZE_SIZE)
                return VERTICAL_INCREMENT;
            else
                return VERTICAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case W:
            if(m_currentPosition+HORIZONTAL_INCREMENT < MAX_MAZE_SIZE)
                return HORIZONTAL_INCREMENT;
            else
                return HORIZONTAL_INCREMENT-MAX_MAZE_SIZE;
            break;
    }
}

int Solver::incrementWest(){
    //Serial.println("Increment West called");
    switch(m_facing){
        case N:
            if(m_currentPosition-HORIZONTAL_INCREMENT >= 0)
                return -HORIZONTAL_INCREMENT;
            else
                return MAX_MAZE_SIZE-HORIZONTAL_INCREMENT;
            break;
        case E:
            if(m_currentPosition+VERTICAL_INCREMENT < MAX_MAZE_SIZE)
                return VERTICAL_INCREMENT;
            else
                return VERTICAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case S:
            if(m_currentPosition+HORIZONTAL_INCREMENT < MAX_MAZE_SIZE)
                return HORIZONTAL_INCREMENT;
            else
                return HORIZONTAL_INCREMENT-MAX_MAZE_SIZE;
            break;
        case W:
            if(m_currentPosition-VERTICAL_INCREMENT < 0)
                return MAX_MAZE_SIZE-VERTICAL_INCREMENT;
            else
                return -VERTICAL_INCREMENT;
            break;
    }
}

// TODO: these functions need to be verified
void Solver::goForward(){
    m_difference = 0;
    if(m_difference == 2){
        //m_diagnostics->getSouthLED()->flashLED();
        //m_diagnostics->getSouthLED()->flashLED();
        // at this point, the locomotion objects "turn around" is called
        m_loco->makeUTurn();
    }
    else if(m_difference == -1){
        //m_diagnostics->getEastLED()->flashLED();
        //m_diagnostics->getEastLED()->flashLED();
        m_loco->turnRight();
    }
    else if(m_difference == 1){
        //m_diagnostics->getWestLED()->flashLED();
        //m_diagnostics->getWestLED()->flashLED();
        m_loco->turnLeft();
    }
    else if(m_difference == 0){
        //m_diagnostics->getNorthLED()->flashLED();
        //m_diagnostics->getNorthLED()->flashLED();
    }
    // at the end of this function, go forward is executed
    m_loco->goForward();
}

void Solver::turnRight(){
    m_difference = 0;
    //Serial.println("Turning right");
    if(m_difference == 2){
        //m_diagnostics->getWestLED()->flashLED();
        //m_diagnostics->getWestLED()->flashLED();
        m_loco->turnLeft();
    }
    else if(m_difference == 1){
        //m_diagnostics->getNorthLED()->flashLED();
        //m_diagnostics->getNorthLED()->flashLED();

        // nothing to do with orientation
    }
    else if(m_difference == -1){
        //m_diagnostics->getSouthLED()->flashLED();
        //m_diagnostics->getSouthLED()->flashLED();

        m_loco->makeUTurn();
    }
    else if(m_difference == 0){
        //m_diagnostics->getEastLED()->flashLED();
        //m_diagnostics->getEastLED()->flashLED();

        m_loco->turnRight();
    }

    m_facing = static_cast<DIRECTION>((m_facing + 1)%4);

    // move to the next node
    m_loco->goForward();
}

void Solver::turnLeft(){
    m_difference = 0;
    if(m_difference == 2){
        //m_diagnostics->getEastLED()->flashLED();
        //m_diagnostics->getEastLED()->flashLED();
        m_loco->turnRight();
    }
    else if(m_difference == 1){
        //m_diagnostics->getNorthLED()->flashLED();
        //m_diagnostics->getNorthLED()->flashLED();
    }
    else if(m_difference == -1){
        //m_diagnostics->getSouthLED()->flashLED();
        //m_diagnostics->getSouthLED()->flashLED();

        m_loco->makeUTurn();
    }
    else if(m_difference == 0){
        //m_diagnostics->getWestLED()->flashLED();
        //m_diagnostics->getWestLED()->flashLED();

        m_loco->turnLeft();
    }

    //Serial.println("Turning left");
    m_facing = static_cast<DIRECTION>((m_facing + 3)%4);

    m_loco->goForward();
}

void Solver::turnAround(){
    //Serial.println("Turning around");
    if(m_difference == 2){
        //m_diagnostics->getNorthLED()->flashLED();
        //m_diagnostics->getNorthLED()->flashLED();
    }
    else if(m_difference == -1){
        //m_diagnostics->getEastLED()->flashLED();
        //m_diagnostics->getEastLED()->flashLED();

        m_loco->turnRight();
    }
    else if(m_difference == 1){
        //m_diagnostics->getWestLED()->flashLED();
        //m_diagnostics->getWestLED()->flashLED();

        m_loco->turnLeft();
    }
    else if(m_difference == 0){
        //m_diagnostics->getSouthLED()->flashLED();
        //m_diagnostics->getSouthLED()->flashLED();

        m_loco->makeUTurn();
    }

    m_facing = static_cast<DIRECTION>((m_facing + 2)%4);

    m_loco->goForward();
}
