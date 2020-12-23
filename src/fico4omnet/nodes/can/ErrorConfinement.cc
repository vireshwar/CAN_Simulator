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

void ErrorConfinement::setControllerState(unsigned int assignControllerState){
    this->controllerState = assignControllerState;
}

unsigned int ErrorConfinement::getControllerState(){
    return this->controllerState;
}

void ErrorConfinement::transErrorReceived(){
    TEC = TEC+8;
    std::string display = "TEC = "+std::to_string(TEC);
    getParentModule()->cComponent::bubble(&display[0]);
    getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
    getParentModule()->getDisplayString().setTagArg("tt", 0, &display[0]);
    if(TEC>=256 && errorState==1){
        errorState = 2;
        getParentModule()->cComponent::bubble("Bus-off state");
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, "WARNING: TEC>=256");
    }
    else if(TEC>=128 && errorState==0){
        errorState = 1;
        getParentModule()->cComponent::bubble("Error-Passive state");
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, "WARNING: TEC>=128");
    }
}

void ErrorConfinement::transSuccess(){
    if(TEC>0){
        TEC = TEC -1;

        std::string display = "TEC = "+std::to_string(TEC);
        getParentModule()->cComponent::bubble(&display[0]);
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, &display[0]);

        if(TEC<128 && errorState!=0)
            errorState = 0;
    }
}

}



