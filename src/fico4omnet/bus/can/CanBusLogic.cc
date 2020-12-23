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

#include "fico4omnet/bus/can/CanBusLogic.h"

#include "fico4omnet/buffer/can/CanOutputBuffer.h"

//Auto-generated messages
#include "fico4omnet/linklayer/can/messages/ErrorFrame_m.h"

namespace FiCo4OMNeT {

using namespace std;

Define_Module(CanBusLogic);

CanBusLogic::CanBusLogic() {
    numDataFrames = 0;
    numRemoteFrames = 0;
    numErrorFrames = 0;

    errpos = INT_MAX;
    errored = false;
    idle = true;

    scheduledDataFrame = new CanDataFrame();

    numFramesSent = 0;
    numBitsSent = 0;

    bandwidth = 0;
    currentSendingID = 0;
    sendingNode = nullptr;
}
CanBusLogic::~CanBusLogic() {
    if(scheduledDataFrame){
        cancelAndDelete(scheduledDataFrame);
    }
}

void CanBusLogic::initialize() {
    rcvdSignal = registerSignal("rxPk");
    rcvdDFSignal = registerSignal("rxDF");
    rcvdRFSignal = registerSignal("rxRF");
    rcvdEFSignal = registerSignal("rxEF");
    stateSignal = registerSignal("state");
    arbitrationLengthSignal = registerSignal("arbitrationLength");

    bubble("state: idle");
    getDisplayString().setTagArg("tt", 0, "state: idle");

    bandwidth = getParentModule()->par("bandwidth");
}

void CanBusLogic::finish() {

}

int CanBusLogic::getSendingNodeID() {
    if (sendingNode != nullptr) {
        return sendingNode->getId();
    }
    return -1;
}

void CanBusLogic::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (dynamic_cast<CanDataFrame *>(msg)) {
            CanDataFrame *df = check_and_cast<CanDataFrame *>(msg);
            send(msg->dup(), "gate$o");
            numFramesSent++;
            numBitsSent += static_cast<unsigned long> (df->getBitLength());
            sendingCompleted();
        } else if (dynamic_cast<ErrorFrame *>(msg)) {
            send(msg->dup(), "gate$o");
            colorIdle();
            emit(stateSignal, static_cast<long>(State::IDLE));
            if (scheduledDataFrame != nullptr) {
                cancelEvent(scheduledDataFrame);
            }
            delete scheduledDataFrame;
            scheduledDataFrame = nullptr;
            errored = false;
            eraseids.clear();
        }

        grantSendingPermission();
    } else if (dynamic_cast<CanDataFrame *>(msg)) {
        colorBusy();
        emit(stateSignal, static_cast<long>(State::TRANSMITTING));
        handleDataFrame(msg);
    } else if (dynamic_cast<ErrorFrame *>(msg)) {
        colorError();
        emit(stateSignal, static_cast<long>(State::TRANSMITTING));
        handleErrorFrame(msg);
    }
    delete msg;
}

void CanBusLogic::grantSendingPermission() {
    currentSendingID = INT_MAX;
    sendingNode = nullptr;
    unsigned int sendByteLength = INT_MAX;
    CanID* sendingid = nullptr;

//    std::cout<<ids.size()<<" Ids length\n";
    for (std::list<CanID*>::iterator it = ids.begin(); it != ids.end(); ++it) {
        CanID *id = *it;
        if (id->getCanID() < currentSendingID) {
            currentSendingID = id->getCanID();
            sendingNode = dynamic_cast<CanOutputBuffer*> (id->getNode());
            sendingid = id;
            sendByteLength = id->getDlc();
            currsit = id->getSignInTime();
        }

        else if(id->getCanID() == currentSendingID &&(id->getDlc()<sendByteLength))
        {
            sendingNode = dynamic_cast<CanOutputBuffer*> (id->getNode());
            sendByteLength = std::min(sendByteLength,id->getDlc());
            sendingid = id;
            currsit = id->getSignInTime();
        }
    }

    int sendcount = 0;


    for (std::list<CanID*>::iterator it = ids.begin(); it != ids.end(); ++it) {
        CanID *id = *it;

        if (id->getCanID() == currentSendingID) {
            if (id->getRtr() == false) { //Data-Frame
                sendcount++;
            }
        }
        eraseids.push_back(it);
    }

    if (sendcount > 1) {
        getParentModule()->cComponent::bubble("More than one node sends with the same ID.");
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/excl3");
        getParentModule()->getDisplayString().setTagArg("tt", 0, "WARNING: More than one node sends with the same ID.");

//        idle = true;

//        bool advFail = false;
//        for (unsigned int it = 0; it != eraseids.size(); it++) {
//            std::list<CanID*>::iterator ite = eraseids.at(it);
//            CanID *id = *ite;
//            if(sendingNode!=id->getNode() &&  id->getCanID() == currentSendingID){
//                CanOutputBuffer* controller = check_and_cast<CanOutputBuffer *>(
//                                id->getNode());
//                unsigned int es = controller->getErrorState();
//                if(es==0)
//                    advFail = true;
//            }
//        }
//
//
//        if(advFail){
//            for (unsigned int it = 0; it != eraseids.size(); it++) {
//                std::list<CanID*>::iterator ite = eraseids.at(it);
//                CanID *id = *ite;
//
//                sendingNotCompleted(id,sendingid->getBitLength());
//
//                delete *(ite);
//                ids.erase(ite);
//            }
//
//            sendingNode = nullptr;
//            eraseids.clear();
//
////            errored = false;
////            if (scheduledDataFrame != nullptr) {
////                cancelEvent(scheduledDataFrame);
////            }
////            scheduledDataFrame = nullptr;
//        }
    }


    if (sendingNode != nullptr) {
        for (unsigned int it = 0; it != eraseids.size(); it++) {
             std::list<CanID*>::iterator ite = eraseids.at(it);
             CanID *id = *ite;

             if(sendingNode != id->getNode() || id->getCanID()!= currentSendingID)
             {
                 sendingNotCompleted(id,sendingid->getBitLength());
             }
             delete *(ite);
             ids.erase(ite);
        }

        CanOutputBuffer* controller = check_and_cast<CanOutputBuffer *>(
                sendingNode);
        controller->receiveSendingPermission(currentSendingID);
    } else {
        idle = true;
        getDisplayString().setTagArg("tt", 0, "state: idle");
        bubble("state: idle");
    }
}

void CanBusLogic::sendingCompleted() {
    colorIdle();
    emit(stateSignal, static_cast<long>(State::IDLE));
    CanOutputBuffer* controller = check_and_cast<CanOutputBuffer*>(sendingNode);
    controller->sendingCompleted();

//    CanID *sendingid;
//    for (unsigned int it = 0; it != eraseids.size(); it++) {
//        std::list<CanID*>::iterator ite = eraseids.at(it);
//        CanID *id = *ite;
//        if(sendingNode == id->getNode())
//        {
//            sendingid = id;
//        }
//    }

    emit(arbitrationLengthSignal, static_cast<unsigned long>(ids.size()));
    eraseids.clear();
    errored = false;
    if (scheduledDataFrame != nullptr) {
        cancelEvent(scheduledDataFrame);
    }
    scheduledDataFrame = nullptr;
}

void CanBusLogic::sendingNotCompleted(CanID* id,unsigned int bitlength) {
        CanOutputBuffer* controller = check_and_cast<CanOutputBuffer*>(id->getNode());
        if(currentSendingID == id->getCanID())
            controller->sendingNotCompleted(id->getCanID(),1,bitlength);
        else
            controller->sendingNotCompleted(id->getCanID(),0,bitlength);
//        emit(arbitrationLengthSignal, static_cast<unsigned long>(ids.size()));
}

void CanBusLogic::handleDataFrame(cMessage *msg) {
    CanDataFrame *df = check_and_cast<CanDataFrame *>(msg);
    int64_t length = df->getBitLength();
    double nextidle;
    nextidle = static_cast<double> (length) / bandwidth;
//    nextidle = static_cast<double> (length+2) / bandwidth;
    if (scheduledDataFrame != nullptr) {
        cancelEvent(scheduledDataFrame);
    }
    delete (scheduledDataFrame);
    scheduledDataFrame = df->dup();
    scheduleAt(simTime() + nextidle, scheduledDataFrame);
    emit(rcvdSignal, df);
    if (df->getRtr()) {
        emit(rcvdRFSignal, df);
        numRemoteFrames++;
    } else {
        emit(rcvdDFSignal, df);
        numDataFrames++;
    }
//    send(msg->dup(), "gate$o");
//    numFramesSent++;
//    numBitsSent += static_cast<unsigned long> (df->getBitLength());
}

void CanBusLogic::handleErrorFrame(cMessage *msg) {
    if (scheduledDataFrame != nullptr) {
        cancelEvent(scheduledDataFrame);
    }
    if (!errored) {
        numErrorFrames++;
        ErrorFrame *ef2 = new ErrorFrame();
        scheduleAt(simTime() + (MAXERRORFRAMESIZE / (bandwidth)), ef2);
        emit(rcvdEFSignal, ef2);
        errored = true;
    }
}

void CanBusLogic::registerForArbitration(unsigned int canID, cModule *module,
        simtime_t signInTime, bool rtr) {
    Enter_Method_Silent
    ();
    ids.push_back(new CanID(canID, module, signInTime, rtr));
    emit(arbitrationLengthSignal, static_cast<unsigned long>(ids.size()));
    if (idle) {
        cMessage *self = new cMessage("idle_signin");
        scheduleAt(simTime() + (1 / (bandwidth)), self);
        idle = false;
        bubble("state: busy");
        getDisplayString().setTagArg("tt", 0, "state: busy");
        emit(stateSignal, static_cast<long>(State::TRANSMITTING));
    }
}

void CanBusLogic::registerForArbitration(unsigned int canID, cModule *module,
        simtime_t signInTime, bool rtr, unsigned int bytelength) {
    Enter_Method_Silent
    ();
    ids.push_back(new CanID(canID, module, signInTime, rtr, bytelength));
    emit(arbitrationLengthSignal, static_cast<unsigned long>(ids.size()));
    if (idle) {
        cMessage *self = new cMessage("idle_signin");
        scheduleAt(simTime() + (1 / (bandwidth)), self);
        idle = false;
        bubble("state: busy");
        getDisplayString().setTagArg("tt", 0, "state: busy");
        emit(stateSignal, static_cast<long>(State::TRANSMITTING));
    }
}

void CanBusLogic::checkoutFromArbitration(unsigned int canID) {
    Enter_Method_Silent
    ();
    for (std::list<CanID*>::iterator it = ids.begin(); it != ids.end(); ++it) {
        CanID* tmp = *it;
        if (tmp->getCanID() == canID) {
            ids.remove(tmp);
            delete tmp;
            break;
        }
    }
    emit(arbitrationLengthSignal, static_cast<unsigned long>(ids.size()));
}

void CanBusLogic::colorBusy() {
    if (getEnvir()->isGUI()) {
        for (int gateIndex = 0;
                gateIndex
                        < getParentModule()->gate("gate$o", 0)->getVectorSize();
                gateIndex++) {
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 0, "yellow");
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 1, "3");

            //TODO: This is necessary due to visualization issues with OMNeT++
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 0, "yellow");
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 1, "3");
        }
    }
}

void CanBusLogic::colorIdle() {
    if (getEnvir()->isGUI()) {
        for (int gateIndex = 0;
                gateIndex
                        < getParentModule()->gate("gate$o", 0)->getVectorSize();
                gateIndex++) {
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 0, "black");
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 1, "1");

            //TODO: This is necessary due to visualization issues with OMNeT++
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 0, "black");
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 1, "1");
        }
    }
}

void CanBusLogic::colorError() {
    if (getEnvir()->isGUI()) {
        for (int gateIndex = 0;
                gateIndex
                        < getParentModule()->gate("gate$o", 0)->getVectorSize();
                gateIndex++) {
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 0, "red");
            getParentModule()->gate("gate$o", gateIndex)->getDisplayString().setTagArg(
                    "ls", 1, "3");

            //TODO: This is necessary due to visualization issues with OMNeT++
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 0, "red");
            getParentModule()->gate("gate$i", gateIndex)->getPreviousGate()->getDisplayString().setTagArg(
                    "ls", 1, "3");
        }
    }
}

}
