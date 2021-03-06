// Copyright (c) 2014-2019 The Dash Core Developers
// Copyright (c) 2016-2019 Duality Blockchain Solutions Developers
// Copyright (c) 2017-2019 Credits Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CREDITS_PRIVATESEND_RELAY_H
#define CREDITS_PRIVATESEND_RELAY_H

#include "main.h"

#include "activemasternode.h"
#include "masternodeman.h"

class CPrivateSendRelay
{
public:
    CTxIn vinMasternode;
    std::vector<unsigned char> vchSig;
    std::vector<unsigned char> vchSig2;
    int nBlockHeight;
    int nRelayType;
    CTxIn in;
    CTxOut out;

    CPrivateSendRelay();
    CPrivateSendRelay(CTxIn& vinMasternodeIn, std::vector<unsigned char>& vchSigIn, int nBlockHeightIn, int nRelayTypeIn, CTxIn& in2, CTxOut& out2);
    
    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(vinMasternode);
        READWRITE(vchSig);
        READWRITE(vchSig2);
        READWRITE(nBlockHeight);
        READWRITE(nRelayType);
        READWRITE(in);
        READWRITE(out);
    }

    std::string ToString();

    bool Sign(std::string strSharedKey);
    bool VerifyMessage(std::string strSharedKey);
    void Relay();
    void RelayThroughNode(int nRank);
};



#endif // CREDITS_PRIVATESEND_RELAY_H
