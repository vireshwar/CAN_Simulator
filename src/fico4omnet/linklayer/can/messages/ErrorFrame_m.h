//
// Generated file, do not edit! Created by nedtool 5.6 from fico4omnet/linklayer/can/messages/ErrorFrame.msg.
//

#ifndef __FICO4OMNET_ERRORFRAME_M_H
#define __FICO4OMNET_ERRORFRAME_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace FiCo4OMNeT {

/**
 * Class generated from <tt>fico4omnet/linklayer/can/messages/ErrorFrame.msg:34</tt> by nedtool.
 * <pre>
 * // Used for error signaling
 * // 
 * // Sent by nodes and forwarded by the bus to signal errors to all participating nodes
 * message ErrorFrame
 * {
 *     // ID of the message
 *     unsigned int canID;
 *     // related node with error
 *     int node;
 *     //Sender: 0: Bit-Error, 1: Form-Error; Receiver: 2: CRC-Error, 3: Bit-Stuffing-Error
 *     int kind;
 *     // position of the error in the corresponding data-frame
 *     int pos;
 * }
 * </pre>
 */
class ErrorFrame : public ::omnetpp::cMessage
{
  protected:
    unsigned int canID;
    int node;
    int kind;
    int pos;
    bool active;

  private:
    void copy(const ErrorFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ErrorFrame&);

  public:
    ErrorFrame(const char *name=nullptr, short kind=0);
    ErrorFrame(const ErrorFrame& other);
    virtual ~ErrorFrame();
    ErrorFrame& operator=(const ErrorFrame& other);
    virtual ErrorFrame *dup() const override {return new ErrorFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual unsigned int getCanID() const;
    virtual void setCanID(unsigned int canID);
    virtual int getNode() const;
    virtual void setNode(int node);
    virtual int getKind() const;
    virtual void setKind(int kind);
    virtual int getPos() const;
    virtual void setPos(int pos);
    virtual bool getActive() const;
    virtual void setActive(bool act);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ErrorFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ErrorFrame& obj) {obj.parsimUnpack(b);}

} // namespace FiCo4OMNeT

#endif // ifndef __FICO4OMNET_ERRORFRAME_M_H

