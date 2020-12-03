//Copyright (c) 2014, CoRE Research Group, Hamburg University of Applied Sciences
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without modification,
//are permitted provided that the following conditions are met:
//
//1. Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
//2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
//3. Neither the name of the copyright holder nor the names of its contributors
//   may be used to endorse or promote products derived from this software without
//   specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
//ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
//ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "fico4omnet/buffer/can/CanOutputBuffer.h"

#include "fico4omnet/bus/can/CanBusLogic.h"
#include "fico4omnet/linklayer/can/CanPortOutput.h"

#include "fico4omnet/nodes/can/ErrorConfinement.h"

namespace FiCo4OMNeT {

Define_Module(CanOutputBuffer);

CanOutputBuffer::~CanOutputBuffer(){
    for (std::list<cMessage*>::iterator it =  frames.begin(); it != frames.end(); ++it)
    {
            cancelAndDelete((*it));
    }
    frames.clear();
}

void CanOutputBuffer::handleMessage(cMessage *msg) {
    if(getErrorState()==2){
        delete msg;
        return;
    }
    if (msg->isSelfMessage()) {
        if(retransmitDataFrame != nullptr){
            registerForArbitration(retransmitDataFrame->getCanID(), retransmitDataFrame->getRtr(), (unsigned int)retransmitDataFrame->getByteLength());
            retransmitDataFrame = nullptr;
        }
        delete msg;
    }
    else if (msg->arrivedOn("in") || msg->arrivedOn("directIn")) {
        recordPacketReceived(msg);
        putFrame(msg);
    }
}


void CanOutputBuffer::putFrame(cMessage* msg) {

        CanDataFrame *frame = dynamic_cast<CanDataFrame *>(msg);
        if (MOB == true) {
            if (getFrame(frame->getCanID()) != nullptr) {
                checkoutFromArbitration(frame->getCanID());
            }
        }
        frames.push_back(frame);
        emit(queueLengthSignal, static_cast<unsigned long>(frames.size()));
        queueSize+=static_cast<size_t>(frame->getByteLength());
        emit(queueSizeSignal, static_cast<unsigned long>(queueSize));

    //    registerForArbitration(frame->getCanID(), frame->getRtr());

        registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getByteLength());
        emit(rxPkSignal, msg);
 }

void CanOutputBuffer::registerForArbitration(unsigned int canID, bool rtr) {
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    canBusLogic->registerForArbitration(canID, this, simTime(), rtr);
}

void CanOutputBuffer::registerForArbitration(unsigned int canID, bool rtr, unsigned int bytelength) {

    //should register after delay
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    canBusLogic->registerForArbitration(canID, this, simTime(), rtr, bytelength);
}

void CanOutputBuffer::checkoutFromArbitration(unsigned int canID) {
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    if (canBusLogic->getCurrentSendingId() != canID && canBusLogic->getSendingNodeID() != this->getId()) {
        canBusLogic->checkoutFromArbitration(canID);
        deleteFrame(canID);
    }
}

void CanOutputBuffer::receiveSendingPermission(unsigned int canID) {
    Enter_Method_Silent
    ();
    deliverFrame(canID);
}

void CanOutputBuffer::sendingCompleted() {
    Enter_Method_Silent
    ();
    deleteFrame(currentFrame);
    currentFrame = nullptr;

    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
    unsigned int state = ec->getErrorState();
    unsigned int newTEC = ec->getTEC();
    if(newTEC>0){
        newTEC = newTEC -1;
        ec->setTEC(newTEC);

        std::string display = "TEC = "+std::to_string(newTEC);
        getParentModule()->cComponent::bubble(&display[0]);
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, &display[0]);

        if(newTEC<128 && state!=0)
            ec->setErrorState(0);
    }

    std::cout<<ec->getErrorState()<<" "<<ec->getTEC()<<"\n";
    CanPortOutput* portOutput = check_and_cast<CanPortOutput*>(
            getParentModule()->getSubmodule("canNodePort")->getSubmodule(
                    "canPortOutput"));
    portOutput->sendingCompleted();
}


void CanOutputBuffer::sendingNotCompleted(unsigned int canID,bool error,unsigned int bitlength){
    Enter_Method_Silent
    ();
    CanDataFrame *frame;

    std::list<cMessage*>::iterator itr;

    for(itr = frames.begin(); itr != frames.end() ;itr++)
    {
        CanDataFrame *frame_temp = dynamic_cast<CanDataFrame *>(*itr);
        if(frame_temp->getCanID() == canID)
        {
            frame = frame_temp;
            break;
        }
    }

    if(error){
        ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
        unsigned int state = ec->getErrorState();
        unsigned int newTEC = ec->getTEC()+8;

        retransmitDataFrame = frame;

        CanPortOutput* cpo = check_and_cast<CanPortOutput*>(getParentModule()->getSubmodule("canNodePort")->getSubmodule("canPortOutput"));
        double bandwidth = cpo->getBandwidth();

        //SOF -1 ID-11 SRR-1 IDE-1 IDEN-18 RTR-1 R0-1 R1-1
        if(state==0){
            cMessage *self = new cMessage("idle_signin");
            scheduleAt(simTime()+(35+12+8+3)/bandwidth, self);
//            registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getByteLength());
        }
        else if(state==1 && newTEC<256){
            cMessage *self = new cMessage("idle_signin");
            scheduleAt(simTime()+(bitlength+11)/bandwidth, self);
//            registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getByteLength());
        }

        ec->setTEC(newTEC);
        std::string display = "TEC = "+std::to_string(newTEC);
        getParentModule()->cComponent::bubble(&display[0]);
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, &display[0]);
        if(newTEC>=256 && state==1){
            ec->setErrorState(2);
            retransmitDataFrame = nullptr;
            getParentModule()->cComponent::bubble("Bus-off state");
            getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
            getParentModule()->getDisplayString().setTagArg("tt", 0, "WARNING: TEC>=256");
        }
        else if(newTEC>=128 && state==0){
            ec->setErrorState(1);
            getParentModule()->cComponent::bubble("Error-Passive state");
            getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
            getParentModule()->getDisplayString().setTagArg("tt", 0, "WARNING: TEC>=128");
        }
    }
    else{
        registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getByteLength());
//        std::cout<<"No error : Arb failed\n";
    }

    emit(rxPkSignal, frame);
}

unsigned int CanOutputBuffer::getErrorState(){
    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
    return  ec->getErrorState();
}

}
