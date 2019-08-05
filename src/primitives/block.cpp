// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>
#include <utilstrencodings.h>
#include <crypto/common.h>
#include <crypto/scrypt.h>
#include <streams.h>

uint256 CBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}
#define TIME_MASK 0xffffff80
uint256 CBlockHeader::GetPoWHash() const
{
	    //Only change every 128 seconds
    int32_t nTimeX16r = nTime&TIME_MASK;
    uint256 hashTime = Hash(BEGIN(nTimeX16r), END(nTimeX16r));
    return HashX16R(BEGIN(nVersion), END(nNonce), hashTime);
}

/* bool CBlock::IsProofOfStake() const
{
    return (vtx.size() > 1 && vtx[1]->IsCoinStake());
}

bool CBlock::IsProofOfWork() const
{
    return !IsProofOfStake();
}*/

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashVeilData=%s, hashMerkleRoot=%s, hashWitnessMerkleRoot=%s, hashProofOfFullNode=%s, nTime=%u, nBits=%08x, nNonce=%u, proof=%s, prevoutStake=%s, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        // hashVeilData.ToString(),
        hashMerkleRoot.ToString(),
        // hashWitnessMerkleRoot.ToString(),
        // hashPoFN.ToString(),
        nTime, nBits, nNonce,
	IsProofOfStake() ? "PoS" : "PoW",
        prevoutStake.ToString(),
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}