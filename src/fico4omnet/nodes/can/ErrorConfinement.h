/*
 * ErrorConfinement.h
 *
 *  Created on: 16-Nov-2020
 *      Author: Dell
 */

#ifndef FICO4OMNET_NODES_CAN_ERRORCONFINEMENT_H_
#define FICO4OMNET_NODES_CAN_ERRORCONFINEMENT_H_

#include "fico4omnet/base/FiCo4OMNeT_Defs.h"

namespace FiCo4OMNeT {

class ErrorConfinement: public omnetpp::cSimpleModule {

public:

    /**
     * @brief Constructor of ErrorConfinement
    */
    ErrorConfinement();

    /**
    * @brief Destructor of ErrorConfinement
    */
    ~ErrorConfinement();

    void setTEC(unsigned int assignTEC);

    void setREC(unsigned int assignREC);

    void setErrorState(unsigned int assignErrorState);

    unsigned int getTEC();

    unsigned int getREC();

    unsigned int getErrorState();

    void setControllerState(unsigned int controllerState);

    unsigned int getControllerState();

    void transErrorReceived();

    void transSuccess();

    void incClutterCount();

    bool checkClutterCount();

    void resetClutterCount();

    bool getSendClutter();

    void setSendClutter(bool par);

private:
    unsigned int TEC;

    unsigned int REC;

    unsigned int errorState; //0->Error Active  1->Error Passive 2->BusOFF

    /**
    * @brief 0 -> Normal Transmitting State
    * 1 -> Error Flag Transmission
    * 2 -> Delimiter
    * 3 -> retransmit passive error flag
    */
    unsigned int controllerState = 0;

    unsigned int clutterCount = 0;

    bool sendClutter = false;
};


}



#endif /* FICO4OMNET_NODES_CAN_ERRORCONFINEMENT_H_ */
