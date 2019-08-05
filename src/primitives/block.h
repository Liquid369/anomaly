// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include <primitives/transaction.h>
#include <serialize.h>
#include <uint256.h>
#include <hash.h>

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
class CBlockHeader
{
public:
    // header
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    // uint256 hashVeilData; // Serialzie hash of CVeilBlockData(hashMerkleRoot, hashAccumulators, hashWitnessMerkleRoot, hashPoFN)
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
    COutPoint prevoutStake;
    uint8_t fProofOfStake;
    uint8_t fProofOfFullNode;

    CBlockHeader()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        // READWRITE(hashVeilData);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        READWRITE(prevoutStake);
        if (!(s.GetType() & SER_GETHASH)) {
            READWRITE(fProofOfStake);
            READWRITE(fProofOfFullNode);
        }
    }

    void SetNull()
    {
        nVersion = 0;
        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        // hashVeilData.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        fProofOfStake = 0;
        fProofOfFullNode = 0;
        prevoutStake.SetNull();
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const;

    uint256 GetPoWHash() const;
    uint256 GetHashWithoutSign() const;
    
    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    };
     // ppcoin: two types of block: proof-of-work or proof-of-stake
    virtual bool IsProofOfStake() const //qtum
    {
        return !prevoutStake.IsNull();
    }

    virtual uint32_t StakeTime() const
    {
        uint32_t ret = 0;
        if(IsProofOfStake())
        {
            ret = nTime;
        }
        return ret;
    }

    virtual bool IsProofOfWork() const
    {
        return !IsProofOfStake();
    }

    virtual ~CBlockHeader(){};
};

class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransactionRef> vtx;
    std::vector<unsigned char> vchBlockSig;

    // memory only
    mutable CTxOut txoutMasternode;
    mutable std::vector<CTxOut> voutSuperblock;
    mutable bool fChecked;
    mutable bool fSignaturesVerified;

    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header)
    {
        SetNull();
        *(static_cast<CBlockHeader*>(this)) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITEAS(CBlockHeader, *this);
        READWRITE(vtx);
        READWRITE(hashMerkleRoot);
        // READWRITE(hashWitnessMerkleRoot);
        if (IsProofOfStake()) {
            // READWRITE(hashPoFN);
            READWRITE(vchBlockSig);
        }
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        fChecked = false;
		vchBlockSig.clear();

        hashMerkleRoot = uint256();
        // hashWitnessMerkleRoot = uint256();
        // hashPoFN = uint256();
        fSignaturesVerified = false;
    }

    std::pair<COutPoint, unsigned int> GetProofOfStake() const //qtum
    {
        return IsProofOfStake()? std::make_pair(prevoutStake, nTime) : std::make_pair(COutPoint(), (unsigned int)0);
    }
    
    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nVersion       = nVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        //block.hashVeilData   = SerializeHash(veilBlockData);
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.prevoutStake   = prevoutStake;
        block.fProofOfStake = IsProofOfStake();
        block.fProofOfFullNode = fProofOfFullNode;
        return block;
    }

    // two types of block: proof-of-work or proof-of-stake
/*      bool IsProofOfStake() const
    {
        return (vtx.size() > 1 && vtx[1]->IsCoinStake());
    }

    bool IsProofOfWork() const
    {
        return !IsProofOfStake();
    }*/

    std::string ToString() const;
};

/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    explicit CBlockLocator(const std::vector<uint256>& vHaveIn) : vHave(vHaveIn) {}

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        int nVersion = s.GetVersion();
        if (!(s.GetType() & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull() const
    {
        return vHave.empty();
    }
};

#endif // BITCOIN_PRIMITIVES_BLOCK_H