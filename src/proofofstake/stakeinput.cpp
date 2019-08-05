// Copyright (c) 2017-2019 The PIVX developers
// Copyright (c) 2019 The Veil developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <tinyformat.h>
#include "chain.h"
#include "chainparams.h"
#include "wallet/deterministicmint.h"
#include "validation.h"
#include "proofofstake/stakeinput.h"
#include "proofofstake/kernel.h"
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif

typedef std::vector<unsigned char> valtype;

