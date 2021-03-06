// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_MAIN_H
#define BITCOIN_MAIN_H

#include "bignum.h"
#include "constraints.h"
#include "core.h"
#include "disk.h"
#include "hashblock.h"
#include "net.h"
#include "script.h"
#include "sync.h"
#include "transaction.h"
#include "txmempool.h"

#include <list>

class CValidationState;

#define START_MASTERNODE_PAYMENTS_TESTNET 1432907775
#define START_MASTERNODE_PAYMENTS 1432907775

static const int64_t DEFAULT_MASTERNODE_COLLATERAL = (30000 * COIN);
static const int64_t DARKSEND_FEE = (0.002 * COIN);
static const int64_t DARKSEND_POOL_MAX = (9999999.99 * COIN);

#define INSTANTX_SIGNATURES_REQUIRED           20
#define INSTANTX_SIGNATURES_TOTAL              30

#define MASTERNODE_NOT_PROCESSED               0 // initial state
#define MASTERNODE_IS_CAPABLE                  1
#define MASTERNODE_NOT_CAPABLE                 2
#define MASTERNODE_STOPPED                     3
#define MASTERNODE_INPUT_TOO_NEW               4
#define MASTERNODE_PORT_NOT_OPEN               6
#define MASTERNODE_PORT_OPEN                   7
#define MASTERNODE_SYNC_IN_PROCESS             8
#define MASTERNODE_REMOTELY_ENABLED            9

#define MASTERNODE_MIN_CONFIRMATIONS           7
#define MASTERNODE_MIN_DSEEP_SECONDS           (30*60)
#define MASTERNODE_MIN_DSEE_SECONDS            (5*60)
#define MASTERNODE_PING_SECONDS                (1*60) //(1*60)
#define MASTERNODE_PING_WAIT_SECONDS           (5*60)
#define MASTERNODE_EXPIRATION_SECONDS          (43265*60) //Old 65*60
#define MASTERNODE_REMOVAL_SECONDS             (43270*60) //Old 70*60

class CBlock;
class CBlockIndex;
class CInv;
class CKeyItem;
class CNode;
class CReserveKey;
class CWallet;

/** The maximum allowed size for a serialized block, in bytes (network rule) */
static const unsigned int MAX_BLOCK_SIZE = 6000000;
/** The maximum size for mined blocks */
static const unsigned int MAX_BLOCK_SIZE_GEN = MAX_BLOCK_SIZE / 2;
/** The maximum size for transactions we're willing to relay/mine **/
static const unsigned int MAX_STANDARD_TX_SIZE = MAX_BLOCK_SIZE_GEN / 5;
/** The maximum allowed number of signature check operations in a block (network rule) */
static const unsigned int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE / 50;
/** Maxiumum number of signature check operations in an IsStandard() P2SH script */
static const unsigned int MAX_P2SH_SIGOPS = 15;
/** The maximum number of sigops we're willing to relay/mine in a single tx */
static const unsigned int MAX_TX_SIGOPS = MAX_BLOCK_SIGOPS / 5;
/** The maximum number of orphan transactions kept in memory */
static const unsigned int MAX_ORPHAN_TRANSACTIONS = MAX_BLOCK_SIZE / 100;
/** Default for -maxorphanblocks, maximum number of orphan blocks kept in memory */
static const unsigned int DEFAULT_MAX_ORPHAN_BLOCKS = 750;
/** The maximum number of entries in an 'inv' protocol message */
static const unsigned int MAX_INV_SZ = 50000;
/** Fees smaller than this (in satoshi) are considered zero fee (for transaction creation) */
static const int64_t MIN_TX_FEE = 10000;
/** Fees smaller than this (in satoshi) are considered zero fee (for relaying) */
static const int64_t MIN_RELAY_TX_FEE = MIN_TX_FEE;
/** No amount larger than this (in satoshi) is valid */
static const int64_t MAX_MONEY = 1400000 * COIN; //14M

//static const int64_t COIN_YEAR_REWARD = 10 * CENT;
//static const int64_t MAX_MINT_PROOF_OF_STAKE = 0.01 * COIN;

// First POS block
static const int MODIFIER_INTERVAL_SWITCH = 1;

/** Threshold for nLockTime: below this value it is interpreted as block number, otherwise as UNIX timestamp. */
static const unsigned int LOCKTIME_THRESHOLD = 500000000; // Tue Nov  5 00:53:20 1985 UTC

inline bool IsProtocolV1RetargetingFixed(int nHeight) { return TestNet() || nHeight > 0; }
inline bool IsProtocolV2(int nHeight) { return TestNet() || nHeight > 0; }

inline int64_t FutureDriftV1(int64_t nTime) { return nTime + 10 * 60; }
inline int64_t FutureDriftV2(int64_t nTime) { return nTime + 10 * 60; }
inline int64_t FutureDrift(int64_t nTime, int nHeight) { return IsProtocolV2(nHeight) ? FutureDriftV2(nTime) : FutureDriftV1(nTime); }

inline unsigned int GetTargetSpacing(int nHeight) { return IsProtocolV2(nHeight) ? 240 : 60; }

extern CScript COINBASE_FLAGS;
extern CCriticalSection cs_main;
extern CTxMemPool mempool;
extern std::map<uint256, CBlockIndex*> mapBlockIndex;
extern std::set<std::pair<COutPoint, unsigned int> > setStakeSeen;
extern CBlockIndex* pindexGenesisBlock;
extern unsigned int nStakeMinAge;
extern unsigned int nNodeLifespan;
extern int nCoinbaseMaturity;
extern int nBestHeight;
extern uint256 nBestChainTrust;
extern uint256 nBestInvalidTrust;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;
extern uint64_t nLastBlockTx;
extern uint64_t nLastBlockSize;
extern int64_t nLastCoinStakeSearchInterval;
extern const std::string strMessageMagic;
extern int64_t nTimeBestReceived;
extern bool fImporting;
extern bool fReindex;
struct COrphanBlock;
extern std::map<uint256, COrphanBlock*> mapOrphanBlocks;
extern bool fHaveGUI;

// Settings
extern bool fUseFastIndex;
extern unsigned int nDerivationMethodIndex;

extern bool fMinimizeCoinAge;

class CReserveKey;
class CTxDB;
class CTxIndex;
class CWalletInterface;

void RegisterWallet(CWalletInterface* pwalletIn); // Register a wallet to receive updates from core
void UnregisterWallet(CWalletInterface* pwalletIn);
void UnregisterAllWallets();
void SyncWithWallets(const CTransaction& tx, const CBlock* pblock = NULL, bool fConnect = true);
void ResendWalletTransactions(bool fForce = false); // Ask wallets to resend their transactions

void RegisterNodeSignals(CNodeSignals& nodeSignals);
void UnregisterNodeSignals(CNodeSignals& nodeSignals);
void PushGetBlocks(CNode* pnode, CBlockIndex* pindexBegin, uint256 hashEnd);

bool ProcessBlock(CNode* pfrom, CBlock* pblock);
bool LoadBlockIndex(bool fAllowNew=true);
void PrintBlockTree();
CBlockIndex* FindBlockByHeight(int nHeight);
bool ProcessMessages(CNode* pfrom);
bool SendMessages(CNode* pto, bool fSendTrickle);
void ThreadImport(std::vector<boost::filesystem::path> vImportFiles);

bool CheckProofOfWork(uint256 hash, unsigned int nBits);
unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake);
int64_t GetProofOfWorkReward(int64_t nFees, int nHeight);
int64_t GetProofOfStakeReward(int64_t nCoinAge, int64_t nFees, int nHeight);
unsigned int ComputeMinWork(unsigned int nBase, int64_t nTime);
unsigned int ComputeMinStake(unsigned int nBase, int64_t nTime, unsigned int nBlockTime);
bool IsInitialBlockDownload();
std::string GetWarnings(std::string strFor);
bool GetTransaction(const uint256 &hash, CTransaction &tx, uint256 &hashBlock);
uint256 WantedByOrphan(const COrphanBlock* pblockOrphan);
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);
void ThreadStakeMiner(CWallet *pwallet);

bool AcceptToMemoryPool(CTxMemPool& pool, CTransaction &tx, bool fLimitFree, bool* pfMissingInputs);
bool AcceptableInputs(CTxMemPool& pool, const CTransaction &txo, bool fLimitFree, bool* pfMissingInputs);

bool FindTransactionsByDestination(const CTxDestination &dest, std::vector<uint256> &vtxhash);

int GetInputAge(CTxIn& vin);
bool AbortNode(const std::string &msg, const std::string &userMessage="");
void Misbehaving(NodeId nodeid, int howmuch); // Increase a node's misbehavior score

int64_t GetMasternodePayment(int nHeight, int64_t blockValue);

enum GetMinFee_mode
{
    GMF_BLOCK,
    GMF_RELAY,
    GMF_SEND,
};

int64_t GetMinFee(const CTransaction& tx, unsigned int nBlockSize = 1, enum GetMinFee_mode mode = GMF_BLOCK, unsigned int nBytes = 0);

/** Wrapper for CTxOut that provides a more compact serialization */
class CTxOutCompressor
{
private:
    CTxOut &txout;
public:
    CTxOutCompressor(CTxOut &txoutIn) : txout(txoutIn) { }

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(txout.nValue));
        CScriptCompressor cscript(REF(txout.scriptPubKey));
        READWRITE(cscript);
    )
};

/** Check for standard transaction types
    @param[in] mapInputs Map of previous transactions that have outputs we're spending
    @return True if all inputs (scriptSigs) use only standard transaction forms
    @see CTransaction::FetchInputs
*/
bool AreInputsStandard(const CTransaction& tx, const MapPrevTx& mapInputs);

/** Count ECDSA signature operations the old-fashioned (pre-0.6) way
    @return number of sigops this transaction's outputs will produce when spent
    @see CTransaction::FetchInputs
*/
unsigned int GetLegacySigOpCount(const CTransaction& tx);

/** Count ECDSA signature operations in pay-to-script-hash inputs.

    @param[in] mapInputs Map of previous transactions that have outputs we're spending
    @return maximum number of sigops required to validate this transaction's inputs
    @see CTransaction::FetchInputs
 */
unsigned int GetP2SHSigOpCount(const CTransaction& tx, const MapPrevTx& mapInputs);

// A transaction with a merkle branch linking it to the block chain
class CMerkleTx : public CTransaction
{
private:
    int GetDepthInMainChainINTERNAL(CBlockIndex* &pindexRet) const;
public:
    uint256 hashBlock;
    std::vector<uint256> vMerkleBranch;
    int nIndex;

    // Memory only
    mutable bool fMerkleVerified;

    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        hashBlock = 0;
        nIndex = -1;
        fMerkleVerified = false;
    }

    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, nVersion, ser_action);
        nVersion = this->nVersion;
        READWRITE(hashBlock);
        READWRITE(vMerkleBranch);
        READWRITE(nIndex);
    )

    int SetMerkleBranch(const CBlock* pblock=NULL);

    // Return depth of transaction in blockchain:
    // -1  : not in blockchain, and not in memory pool (conflicted transaction)
    //  0  : in memory pool, waiting to be included in a block
    // >=1 : this many blocks deep in the main chain
    int GetDepthInMainChain(CBlockIndex* &pindexRet) const;
    int GetDepthInMainChain() const { CBlockIndex *pindexRet; return GetDepthInMainChain(pindexRet); }
    bool IsInMainChain() const { CBlockIndex *pindexRet; return GetDepthInMainChainINTERNAL(pindexRet) > 0; }
    int GetBlocksToMaturity() const;
    bool AcceptToMemoryPool(bool fLimitFree=true);
    int GetTransactionLockSignatures() const;
    bool IsTransactionLockTimedOut() const;
};

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 *
 * Blocks are appended to blk0001.dat files on disk.  Their location on disk
 * is indexed by CBlockIndex objects in memory.
 */
class CBlock
{
public:
    // header
    static const int CURRENT_VERSION = 7;
    int nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    unsigned int nTime;
    unsigned int nBits;
    unsigned int nNonce;

    // Network and disk
    std::vector<CTransaction> vtx;

    // ppcoin: Block signature - signed by one of the coin base txout[N]'s owner
    std::vector<unsigned char> vchBlockSig;

    // Memory only
    mutable std::vector<uint256> vMerkleTree;

    // Denial-of-service detection:
    mutable int nDoS;
    bool DoS(int nDoSIn, bool fIn) const { nDoS += nDoSIn; return fIn; }

    CBlock()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);

        // ConnectBlock depends on vtx following header to generate CDiskTxPos
        if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
        {
            READWRITE(vtx);
            READWRITE(vchBlockSig);
        }
        else if (fRead)
        {
            const_cast<CBlock*>(this)->vtx.clear();
            const_cast<CBlock*>(this)->vchBlockSig.clear();
        }
    )

    void SetNull()
    {
        nVersion = CBlock::CURRENT_VERSION;
        hashPrevBlock = 0;
        hashMerkleRoot = 0;
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        vtx.clear();
        vchBlockSig.clear();
        vMerkleTree.clear();
        nDoS = 0;
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const
    {
        if (nVersion > 6)
            return Hash9(BEGIN(nVersion), END(nNonce));
        else
            return GetPoWHash();
    }

    uint256 GetPoWHash() const
    {
        return Hash9(BEGIN(nVersion), END(nNonce));
    }

    int64_t GetBlockTime() const
    {
        return (int64_t) nTime;
    }

    void UpdateTime(const CBlockIndex* pindexPrev);

    // Entropy bit for stake modifier if chosen by modifier
    unsigned int GetStakeEntropyBit() const
    {
        // Take last bit of block hash as entropy bit
        unsigned int nEntropyBit = ((GetHash().Get64()) & 1llu);
        LogPrint("stakemodifier", "GetStakeEntropyBit: hashBlock=%s nEntropyBit=%u\n", GetHash().ToString(), nEntropyBit);
        return nEntropyBit;
    }

    // ppcoin: Two types of block: proof-of-work or proof-of-stake
    bool IsProofOfStake() const
    {
        return (vtx.size() > 1 && vtx[1].IsCoinStake());
    }

    bool IsProofOfWork() const
    {
        return !IsProofOfStake();
    }

    std::pair<COutPoint, unsigned int> GetProofOfStake() const
    {
        return IsProofOfStake()? std::make_pair(vtx[1].vin[0].prevout, vtx[1].nTime) : std::make_pair(COutPoint(), (unsigned int)0);
    }

    // ppcoin: Get max transaction timestamp
    int64_t GetMaxTransactionTime() const
    {
        int64_t maxTransactionTime = 0;

        BOOST_FOREACH(const CTransaction& tx, vtx)
            maxTransactionTime = std::max(maxTransactionTime, (int64_t)tx.nTime);

        return maxTransactionTime;
    }

    uint256 BuildMerkleTree() const
    {
        vMerkleTree.clear();

        BOOST_FOREACH(const CTransaction& tx, vtx)
            vMerkleTree.push_back(tx.GetHash());

        int j = 0;

        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            for (int i = 0; i < nSize; i += 2)
            {
                int i2 = std::min(i + 1, nSize - 1);

                vMerkleTree.push_back(Hash(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
                                           BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
            }

            j += nSize;
        }

        return vMerkleTree.empty() ? 0 : vMerkleTree.back();
    }

    std::vector<uint256> GetMerkleBranch(int nIndex) const
    {
        if (vMerkleTree.empty())
            BuildMerkleTree();

        std::vector<uint256> vMerkleBranch;
        int j = 0;

        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            int i = std::min(nIndex^1, nSize - 1);
            vMerkleBranch.push_back(vMerkleTree[j + i]);
            nIndex >>= 1;
            j += nSize;
        }

        return vMerkleBranch;
    }

    static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex)
    {
        if (nIndex == -1)
            return 0;

        BOOST_FOREACH(const uint256& otherside, vMerkleBranch)
        {
            if (nIndex & 1)
                hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
            else
                hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));

            nIndex >>= 1;
        }

        return hash;
    }


    bool WriteToDisk(unsigned int& nFileRet, unsigned int& nBlockPosRet)
    {
        // Open history file to append
        CAutoFile fileout = CAutoFile(AppendBlockFile(nFileRet), SER_DISK, CLIENT_VERSION);

        if (!fileout)
            return error("CBlock::WriteToDisk() : AppendBlockFile failed");

        // Write index header
        unsigned int nSize = fileout.GetSerializeSize(*this);
        fileout << FLATDATA(Params().MessageStart()) << nSize;
        long fileOutPos = ftell(fileout);

        if (fileOutPos < 0)
            return error("CBlock::WriteToDisk() : ftell failed");

        nBlockPosRet = fileOutPos;
        fileout << *this;

        // Flush stdio buffers and commit to disk before returning
        fflush(fileout);

        if (!IsInitialBlockDownload() || (nBestHeight+1) % 500 == 0)
            FileCommit(fileout);

        return true;
    }

    bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions=true)
    {
        SetNull();

        // Open history file to read
        CAutoFile filein = CAutoFile(OpenBlockFile(nFile, nBlockPos, "rb"), SER_DISK, CLIENT_VERSION);

        if (!filein)
            return error("CBlock::ReadFromDisk() : OpenBlockFile failed");

        if (!fReadTransactions)
            filein.nType |= SER_BLOCKHEADERONLY;

        // Read block
        try
        {
            filein >> *this;
        }
        catch (std::exception &e)
        {
            return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
        }

        // Check the header
        if (fReadTransactions && IsProofOfWork() && !CheckProofOfWork(GetPoWHash(), nBits))
            return error("CBlock::ReadFromDisk() : errors in block header");

        return true;
    }



    std::string ToString() const
    {
        std::stringstream s;
        s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, "
                       "nNonce=%u, vtx=%u, vchBlockSig=%s)\n", GetHash().ToString(), nVersion,
                       hashPrevBlock.ToString(), hashMerkleRoot.ToString(), nTime, nBits, nNonce,
                       vtx.size(), HexStr(vchBlockSig.begin(), vchBlockSig.end()));

        for (unsigned int i = 0; i < vtx.size(); i++)
            s << "  " << vtx[i].ToString() << "\n";

        s << "  vMerkleTree: ";

        for (unsigned int i = 0; i < vMerkleTree.size(); i++)
            s << " " << vMerkleTree[i].ToString();

        s << "\n";
        return s.str();
    }

    bool DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex);
    bool ConnectBlock(CTxDB& txdb, CBlockIndex* pindex, bool fJustCheck=false);
    bool ReadFromDisk(const CBlockIndex* pindex, bool fReadTransactions=true);
    bool SetBestChain(CTxDB& txdb, CBlockIndex* pindexNew);
    bool AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos, const uint256& hashProof);
    bool CheckBlock(CNode* pfrom=NULL, bool fCheckPOW=true, bool fCheckMerkleRoot=true, bool fCheckSig=true) const;
    bool AcceptBlock();
    bool SignBlock(CWallet& keystore, int64_t nFees);
    bool CheckBlockSignature() const;
    void RebuildAddressIndex(CTxDB& txdb);

private:
    bool SetBestChainInner(CTxDB& txdb, CBlockIndex *pindexNew);
};

/** The block chain is a tree shaped structure starting with the
 * genesis block at the root, with each block potentially having multiple
 * candidates to be the next block.  pprev and pnext link a path through the
 * main/longest chain.  A blockindex may have multiple pprev pointing back
 * to it, but pnext will only point forward to the longest branch, or will
 * be null if the block is not part of the longest chain.
 */
class CBlockIndex
{
public:
    const uint256* phashBlock;
    CBlockIndex* pprev;
    CBlockIndex* pnext;
    unsigned int nFile;
    unsigned int nBlockPos;
    uint256 nChainTrust; // ppcoin: Trust score of block chain
    int nHeight;

    int64_t nMint;
    int64_t nMoneySupply;

    unsigned int nFlags;  // ppcoin: Block index flags

    enum
    {
        BLOCK_PROOF_OF_STAKE = (1 << 0), // Is proof-of-stake block
        BLOCK_STAKE_ENTROPY  = (1 << 1), // Entropy bit for stake modifier
        BLOCK_STAKE_MODIFIER = (1 << 2), // Regenerated stake modifier
    };

    uint64_t nStakeModifier; // Hash modifier for proof-of-stake

    // proof-of-stake specific fields
    COutPoint prevoutStake;
    unsigned int nStakeTime;

    uint256 hashProof;

    // block header
    int nVersion;
    uint256 hashMerkleRoot;
    unsigned int nTime;
    unsigned int nBits;
    unsigned int nNonce;

    CBlockIndex()
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nFile = 0;
        nBlockPos = 0;
        nHeight = 0;
        nChainTrust = 0;
        nMint = 0;
        nMoneySupply = 0;
        nFlags = 0;
        nStakeModifier = 0;
        hashProof = 0;
        prevoutStake.SetNull();
        nStakeTime = 0;

        nVersion       = 0;
        hashMerkleRoot = 0;
        nTime          = 0;
        nBits          = 0;
        nNonce         = 0;
    }

    CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nFile = nFileIn;
        nBlockPos = nBlockPosIn;
        nHeight = 0;
        nChainTrust = 0;
        nMint = 0;
        nMoneySupply = 0;
        nFlags = 0;
        nStakeModifier = 0;
        hashProof = 0;

        if (block.IsProofOfStake())
        {
            SetProofOfStake();
            prevoutStake = block.vtx[1].vin[0].prevout;
            nStakeTime = block.vtx[1].nTime;
        }
        else
        {
            prevoutStake.SetNull();
            nStakeTime = 0;
        }

        nVersion       = block.nVersion;
        hashMerkleRoot = block.hashMerkleRoot;
        nTime          = block.nTime;
        nBits          = block.nBits;
        nNonce         = block.nNonce;
    }

    CBlock GetBlockHeader() const
    {
        CBlock block;
        block.nVersion       = nVersion;

        if (pprev)
            block.hashPrevBlock = pprev->GetBlockHash();

        block.hashMerkleRoot = hashMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;

        return block;
    }

    uint256 GetBlockHash() const
    {
        return *phashBlock;
    }

    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }

    uint256 GetBlockTrust() const;

    bool IsInMainChain() const
    {
        return (pnext || this == pindexBest);
    }

    bool CheckIndex() const
    {
        return true;
    }

    int64_t GetPastTimeLimit() const
    {
        if (IsProtocolV2(nHeight))
            return GetBlockTime() - 120;
        else
            return GetMedianTimePast();
    }

    enum { nMedianTimeSpan=11 };

    int64_t GetMedianTimePast() const
    {
        int64_t pmedian[nMedianTimeSpan];
        int64_t* pbegin = &pmedian[nMedianTimeSpan];
        int64_t* pend = &pmedian[nMedianTimeSpan];
        const CBlockIndex* pindex = this;

        for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->pprev)
            *(--pbegin) = pindex->GetBlockTime();

        std::sort(pbegin, pend);
        return pbegin[(pend - pbegin)/2];
    }

    /**
     * Returns true if there are nRequired or more blocks of minVersion or above
     * in the last nToCheck blocks, starting at pstart and going backwards.
     */
    static bool IsSuperMajority(int minVersion, const CBlockIndex* pstart,
                                unsigned int nRequired, unsigned int nToCheck);


    bool IsProofOfWork() const
    {
        return !(nFlags & BLOCK_PROOF_OF_STAKE);
    }

    bool IsProofOfStake() const
    {
        return (nFlags & BLOCK_PROOF_OF_STAKE);
    }

    void SetProofOfStake()
    {
        nFlags |= BLOCK_PROOF_OF_STAKE;
    }

    unsigned int GetStakeEntropyBit() const
    {
        return ((nFlags & BLOCK_STAKE_ENTROPY) >> 1);
    }

    bool SetStakeEntropyBit(unsigned int nEntropyBit)
    {
        if (nEntropyBit > 1)
            return false;

        nFlags |= (nEntropyBit? BLOCK_STAKE_ENTROPY : 0);
        return true;
    }

    bool GeneratedStakeModifier() const
    {
        return (nFlags & BLOCK_STAKE_MODIFIER);
    }

    void SetStakeModifier(uint64_t nModifier, bool fGeneratedStakeModifier)
    {
        nStakeModifier = nModifier;
        if (fGeneratedStakeModifier)
            nFlags |= BLOCK_STAKE_MODIFIER;
    }

    std::string ToString() const
    {
        return strprintf("CBlockIndex(nprev=%p, pnext=%p, nFile=%u, nBlockPos=%-6d nHeight=%d, nMint=%s, "
                         "nMoneySupply=%s, nFlags=(%s)(%d)(%s), nStakeModifier=%016x, hashProof=%s, "
                         "prevoutStake=(%s), nStakeTime=%d merkle=%s, hashBlock=%s)",
                         pprev, pnext, nFile, nBlockPos, nHeight, FormatMoney(nMint), FormatMoney(nMoneySupply),
                         GeneratedStakeModifier() ? "MOD" : "-", GetStakeEntropyBit(), IsProofOfStake()? "PoS" : "PoW",
                         nStakeModifier, hashProof.ToString(), prevoutStake.ToString(), nStakeTime,
                         hashMerkleRoot.ToString(), GetBlockHash().ToString());
    }
};

/** Used to marshal pointers into hashes for db storage. */
class CDiskBlockIndex : public CBlockIndex
{
private:
    uint256 blockHash;

public:
    uint256 hashPrev;
    uint256 hashNext;

    CDiskBlockIndex()
    {
        hashPrev = 0;
        hashNext = 0;
        blockHash = 0;
    }

    explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)
    {
        hashPrev = (pprev ? pprev->GetBlockHash() : 0);
        hashNext = (pnext ? pnext->GetBlockHash() : 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);

        READWRITE(hashNext);
        READWRITE(nFile);
        READWRITE(nBlockPos);
        READWRITE(nHeight);
        READWRITE(nMint);
        READWRITE(nMoneySupply);
        READWRITE(nFlags);
        READWRITE(nStakeModifier);
        if (IsProofOfStake())
        {
            READWRITE(prevoutStake);
            READWRITE(nStakeTime);
        }
        else if (fRead)
        {
            const_cast<CDiskBlockIndex*>(this)->prevoutStake.SetNull();
            const_cast<CDiskBlockIndex*>(this)->nStakeTime = 0;
        }
        READWRITE(hashProof);

        // Block header
        READWRITE(this->nVersion);
        READWRITE(hashPrev);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        READWRITE(blockHash);
    )

    uint256 GetBlockHash() const
    {
        if (fUseFastIndex && (nTime < GetAdjustedTime() - 24 * 60 * 60) && blockHash != 0)
            return blockHash;

        CBlock block;
        block.nVersion        = nVersion;
        block.hashPrevBlock   = hashPrev;
        block.hashMerkleRoot  = hashMerkleRoot;
        block.nTime           = nTime;
        block.nBits           = nBits;
        block.nNonce          = nNonce;

        const_cast<CDiskBlockIndex*>(this)->blockHash = block.GetHash();
        return blockHash;
    }

    std::string ToString() const
    {
        std::string str = "CDiskBlockIndex(";
        str += CBlockIndex::ToString();
        str += strprintf("\n                hashBlock=%s, hashPrev=%s, hashNext=%s)",
                         GetBlockHash().ToString(), hashPrev.ToString(), hashNext.ToString());
        return str;
    }
};

// Describes a place in the block chain to another node such that if the
// other node doesn't have the same branch, it can find a recent common trunk
class CBlockLocator
{
protected:
    std::vector<uint256> vHave;
public:

    CBlockLocator()
    {
    }

    explicit CBlockLocator(const CBlockIndex* pindex)
    {
        Set(pindex);
    }

    explicit CBlockLocator(uint256 hashBlock)
    {
        std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);

        if (mi != mapBlockIndex.end())
            Set((*mi).second);
    }

    CBlockLocator(const std::vector<uint256>& vHaveIn)
    {
        vHave = vHaveIn;
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);

        READWRITE(vHave);
    )

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull()
    {
        return vHave.empty();
    }

    void Set(const CBlockIndex* pindex)
    {
        vHave.clear();
        int nStep = 1;
        while (pindex)
        {
            vHave.push_back(pindex->GetBlockHash());

            // Exponentially larger steps back
            for (int i = 0; pindex && i < nStep; i++)
                pindex = pindex->pprev;

            if (vHave.size() > 10)
                nStep *= 2;
        }

        vHave.push_back(Params().HashGenesisBlock());
    }

    int GetDistanceBack()
    {
        // Retrace how far back it was in the sender's branch
        int nDistance = 0;
        int nStep = 1;

        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);

            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;

                if (pindex->IsInMainChain())
                    return nDistance;
            }

            nDistance += nStep;

            if (nDistance > 10)
                nStep *= 2;
        }

        return nDistance;
    }

    CBlockIndex* GetBlockIndex()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);

            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;

                if (pindex->IsInMainChain())
                    return pindex;
            }
        }
        return pindexGenesisBlock;
    }

    uint256 GetBlockHash()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);

            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;

                if (pindex->IsInMainChain())
                    return hash;
            }
        }

        return Params().HashGenesisBlock();
    }

    int GetHeight()
    {
        CBlockIndex* pindex = GetBlockIndex();

        if (!pindex)
            return 0;

        return pindex->nHeight;
    }
};

// Capture information about block/transaction validation
class CValidationState {
private:
    enum mode_state {
        MODE_VALID,   // Everything ok
        MODE_INVALID, // Network rule violation (DoS value may be set)
        MODE_ERROR,   // Run-time error
    } mode;

    int nDoS;
    std::string strRejectReason;
    unsigned char chRejectCode;
    bool corruptionPossible;

public:
    CValidationState() : mode(MODE_VALID), nDoS(0), chRejectCode(0), corruptionPossible(false) { }

    bool DoS(int level, bool ret = false, unsigned char chRejectCodeIn=0, std::string strRejectReasonIn="",
             bool corruptionIn=false)
    {
        chRejectCode = chRejectCodeIn;
        strRejectReason = strRejectReasonIn;
        corruptionPossible = corruptionIn;

        if (mode == MODE_ERROR)
            return ret;

        nDoS += level;
        mode = MODE_INVALID;
        return ret;
    }

    bool Invalid(bool ret = false, unsigned char _chRejectCode=0, std::string _strRejectReason="")
    {
        return DoS(0, ret, _chRejectCode, _strRejectReason);
    }

    bool Error(std::string strRejectReasonIn="")
    {
        if (mode == MODE_VALID)
            strRejectReason = strRejectReasonIn;

        mode = MODE_ERROR;
        return false;
    }

    bool Abort(const std::string &msg)
    {
        AbortNode(msg);
        return Error(msg);
    }

    bool IsValid() const
    {
        return mode == MODE_VALID;
    }

    bool IsInvalid() const
    {
        return mode == MODE_INVALID;
    }

    bool IsError() const
    {
        return mode == MODE_ERROR;
    }

    bool IsInvalid(int &nDoSOut) const
    {
        if (IsInvalid())
        {
            nDoSOut = nDoS;
            return true;
        }

        return false;
    }

    bool CorruptionPossible() const
    {
        return corruptionPossible;
    }

    unsigned char GetRejectCode() const { return chRejectCode; }
    std::string GetRejectReason() const { return strRejectReason; }
};

class CWalletInterface {
protected:
    virtual void SyncTransaction(const CTransaction &tx, const CBlock *pblock, bool fConnect) =0;
    virtual void EraseFromWallet(const uint256 &hash) =0;
    virtual void SetBestChain(const CBlockLocator &locator) =0;
    virtual void UpdatedTransaction(const uint256 &hash) =0;
    virtual void Inventory(const uint256 &hash) =0;
    virtual void ResendWalletTransactions(bool fForce) =0;
    friend void ::RegisterWallet(CWalletInterface*);
    friend void ::UnregisterWallet(CWalletInterface*);
    friend void ::UnregisterAllWallets();
};

#endif
