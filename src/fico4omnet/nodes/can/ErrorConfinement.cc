/*
 * ErrorConfinement.cc
 *
 *  Created on: 16-Nov-2020
 *      Author: Dell
 */

#include "fico4omnet/nodes/can/ErrorConfinement.h"

namespace FiCo4OMNeT {

//using namespace std;

Define_Module(ErrorConfinement);

ErrorConfinement::ErrorConfinement() {
    TEC = 0;
    REC = 0;
    errorState = 0;
}

ErrorConfinement::~ErrorConfinement() {

}

void ErrorConfinement::setTEC(unsigned int assignTEC){
    this->TEC = assignTEC;
}

void ErrorConfinement::setREC(unsigned int assignREC){
    this->REC = assignREC;
}

void ErrorConfinement::setErrorState(unsigned int assignErrorState){
    this->errorState = assignErrorState;
}

unsigned int ErrorConfinement::getTEC(){
    return this->TEC;
}

unsigned int ErrorConfinement::getREC(){
    return this->REC;
}

unsigned int ErrorConfinement::getErrorState(){
    return this->errorState;
}



}



