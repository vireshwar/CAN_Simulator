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



private:
    unsigned int TEC;

    unsigned int REC;

    unsigned int errorState; //0->Error Active  1->Error Passive 2->BusOFF

};


}



#endif /* FICO4OMNET_NODES_CAN_ERRORCONFINEMENT_H_ */
