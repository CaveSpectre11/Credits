// Copyright (c) 2009-2019 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Developers
// Copyright (c) 2014-2019 The Dash Core Developers
// Copyright (c) 2016-2019 Duality Blockchain Solutions Developers
// Copyright (c) 2017-2019 Credits Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "creditsconsensus.h"

#include "script/interpreter.h"
#include "pubkey.h"
#include "primitives/transaction.h"
#include "version.h"

namespace {

/** A class that deserializes a single CTransaction one time. */
class TxInputStream
{
public:
    TxInputStream(int nTypeIn, int nVersionIn, const unsigned char *txTo, size_t txToLen) :
    m_type(nTypeIn),
    m_version(nVersionIn),
    m_data(txTo),
    m_remaining(txToLen)
    {}

    TxInputStream& read(char* pch, size_t nSize)
    {
        if (nSize > m_remaining)
            throw std::ios_base::failure(std::string(__func__) + ": end of data");

        if (pch == NULL)
            throw std::ios_base::failure(std::string(__func__) + ": bad destination buffer");

        if (m_data == NULL)
            throw std::ios_base::failure(std::string(__func__) + ": bad source buffer");

        memcpy(pch, m_data, nSize);
        m_remaining -= nSize;
        m_data += nSize;
        return *this;
    }

    template<typename T>
    TxInputStream& operator>>(T& obj)
    {
        ::Unserialize(*this, obj, m_type, m_version);
        return *this;
    }

private:
    const int m_type;
    const int m_version;
    const unsigned char* m_data;
    size_t m_remaining;
};

inline int set_error(creditsconsensus_error* ret, creditsconsensus_error serror)
{
    if (ret)
        *ret = serror;
    return 0;
}

struct ECCryptoClosure
{
    ECCVerifyHandle handle;
};

ECCryptoClosure instance_of_eccryptoclosure;
}

/** Check that all specified flags are part of the libconsensus interface. */
static bool verify_flags(unsigned int flags)
{
    return (flags & ~(creditsconsensus_SCRIPT_FLAGS_VERIFY_ALL)) == 0;
}

int creditsconsensus_verify_script(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen,
                                    const unsigned char *txTo        , unsigned int txToLen,
                                    unsigned int nIn, unsigned int flags, creditsconsensus_error* err)
{
    if (!verify_flags(flags)) {
        return creditsconsensus_ERR_INVALID_FLAGS;
    }
    try {
        TxInputStream stream(SER_NETWORK, PROTOCOL_VERSION, txTo, txToLen);
        CTransaction tx;
        stream >> tx;
        if (nIn >= tx.vin.size())
            return set_error(err, creditsconsensus_ERR_TX_INDEX);
        if (tx.GetSerializeSize(SER_NETWORK, PROTOCOL_VERSION) != txToLen)
            return set_error(err, creditsconsensus_ERR_TX_SIZE_MISMATCH);

         // Regardless of the verification result, the tx did not error.
         set_error(err, creditsconsensus_ERR_OK);

        return VerifyScript(tx.vin[nIn].scriptSig, CScript(scriptPubKey, scriptPubKey + scriptPubKeyLen), flags, TransactionSignatureChecker(&tx, nIn), NULL);
    } catch (const std::exception&) {
        return set_error(err, creditsconsensus_ERR_TX_DESERIALIZE); // Error deserializing
    }
}

unsigned int creditsconsensus_version()
{
    // Just use the API version for now
    return CREDITSCONSENSUS_API_VER;
}
