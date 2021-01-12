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
        ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
        CanPortOutput* portOutput = check_and_cast<CanPortOutput*>(
                getParentModule()->getSubmodule("canNodePort")->getSubmodule(
                        "canPortOutput"));
        unsigned int cs =  ec->getControllerState();

        if(ec->getSendClutter())
        {
            cMessage* clutter = generateClutter();
            putFrame(clutter);
            ec->incClutterCount();
            ec->setSendClutter(false);
        }

        else if(cs == 0){
            ec->setControllerState(1);

            ErrorFrame* ef = generateError();
            ec->transErrorReceived();
            sendDirect(ef, portOutput, "directIn");
        }

        else if(cs==1)
        {
            if(ec->getErrorState()==1){
                handlePassiveFlag();
            }
        }
        else if(cs == 2){
            if(delimCounter==8){
                delimCounter = 0;
                ec->setControllerState(0);
                retransmitDF();
            }
            else{
                handleDelimiter();
            }
        }
        else if(cs==3){
            if(ec->getErrorState()==1) {
                ErrorFrame* ef = generateError();
                sendDirect(ef, portOutput, "directIn");
                ec->setControllerState(1);
            }
        }

        //CHECK PASSIVE

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

//        std::cout << "DLC Bit-byte "<< frame->getBitLength() <<" "<<frame->getByteLength() << " "<<simTime()<<endl;
        registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getBitLength());
        emit(rxPkSignal, msg);
 }

void CanOutputBuffer::registerForArbitration(unsigned int canID, bool rtr) {
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    canBusLogic->registerForArbitration(canID, this, simTime(), rtr);
}

void CanOutputBuffer::registerForArbitration(unsigned int canID, bool rtr, unsigned int bitlength) {

    //should register after delay
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    canBusLogic->registerForArbitration(canID, this, simTime(), rtr, bitlength);
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

    CanPortOutput* cpo = check_and_cast<CanPortOutput*>(getParentModule()->getSubmodule("canNodePort")->getSubmodule("canPortOutput"));
    double bandwidth = cpo->getBandwidth();

    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
    ec->transSuccess();

    //only for SFBO adversary
    if(ec->getTEC()==127)
    {
        ec->setSendClutter(true);
        cMessage *self = new cMessage("idle_signin");
        scheduleAt(simTime()+(1)/bandwidth, self);

    }
    else if(ec->checkClutterCount())
    {
        ec->setSendClutter(true);
        cMessage *self = new cMessage("idle_signin");
        scheduleAt(simTime()+(1)/bandwidth, self);

    }
    else
    {
        ec->setSendClutter(false);
        ec->resetClutterCount();
    }


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
            scheduleAt(simTime()+(35)/bandwidth, self);
        }
        else if(state==1 && newTEC<256){
            cMessage *self = new cMessage("idle_signin");
            scheduleAt(simTime()+(bitlength-8)/bandwidth, self);
        }
        else  ec->transErrorReceived();
    }
    else{
        registerForArbitration(frame->getCanID(), frame->getRtr(), (unsigned int)frame->getBitLength());
    }

    emit(rxPkSignal, frame);
}

unsigned int CanOutputBuffer::getErrorState(){
    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
    return  ec->getErrorState();
}

unsigned int CanOutputBuffer::getControllerState(){
    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
    return  ec->getControllerState();
}

ErrorFrame* CanOutputBuffer::generateError() {
    ErrorFrame *errorMsg = new ErrorFrame("BitError");
//    int errorPos = intuniform(0, static_cast<int>(retransmitDataFrame->getBitLength()) - MAXERRORFRAMESIZE);
    errorMsg->setKind(0);
    errorMsg->setCanID(retransmitDataFrame->getCanID());
//    if (errorPos > 0)
//        errorPos--;
//    errorMsg->setPos(errorPos);
    errorMsg->setActive(getErrorState()==0);
    return errorMsg;
}

void CanOutputBuffer::handleDelimiter(){
    Enter_Method_Silent
    ();
    CanBusLogic *canBusLogic =
            dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                    "canBusLogic"));
    CanPortOutput* cpo = check_and_cast<CanPortOutput*>(getParentModule()->getSubmodule("canNodePort")->getSubmodule("canPortOutput"));
    double bandwidth = cpo->getBandwidth();

    if(canBusLogic->isIdle()){
        delimCounter++;
    }
    else if(delimCounter){
        delimCounter = 0;
        //passive -> controllerTs=1 and TEC+=8
        ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));
        if(ec->getErrorState()==1)
        {
            ec->transErrorReceived();
            ec->setControllerState(1);
        }

    }

    cMessage *self = new cMessage("idle_signin");
    scheduleAt(simTime()+(1)/bandwidth, self);
}

void CanOutputBuffer::retransmitDF() {
    //add IFS
    if(retransmitDataFrame != nullptr){
        registerForArbitration(retransmitDataFrame->getCanID(), retransmitDataFrame->getRtr(), (unsigned int)retransmitDataFrame->getBitLength());
        retransmitDataFrame = nullptr;
    }
}

void CanOutputBuffer::setRetransmitDF() {
    //Verify
    retransmitDataFrame = currentFrame;
}


CanDataFrame* CanOutputBuffer::generateClutter()
{
    CanDataFrame *can_msg = new CanDataFrame("Clutter");
//    CanDataFrame *can_msg = new CanDataFrame("message");
//    can_msg->setCanID(0);
    can_msg->setCanID(1);
    can_msg->setRtr(0);
    can_msg->setBitLength(67);
    can_msg->setPeriod(0);

    return can_msg;

}

void CanOutputBuffer::handlePassiveFlag()
{
    CanPortOutput* cpo = check_and_cast<CanPortOutput*>(getParentModule()->getSubmodule("canNodePort")->getSubmodule("canPortOutput"));
    double bandwidth = cpo->getBandwidth();

    CanBusLogic *canBusLogic =
                dynamic_cast<CanBusLogic*> (getParentModule()->gate("gate$o")->getPathEndGate()->getOwnerModule()->getParentModule()->getSubmodule(
                        "canBusLogic"));
    simtime_t send_time = canBusLogic->getNextIdle() - (8.0/bandwidth);

    ErrorConfinement* ec =  check_and_cast<ErrorConfinement*>(getParentModule()->getSubmodule("errorConfinement"));

    if(send_time > simTime() )
    {
        cMessage *self = new cMessage("idle_signin");
        scheduleAt(send_time, self);
        ec->setControllerState(3);

    }
    else
    {
        ErrorFrame* ef = generateError();
        sendDirect(ef, cpo, "directIn");
    }
}

}
