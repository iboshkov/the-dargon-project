#pragma once

#include "DSPEx.hpp"
#include "IDSPExInstructionSet.hpp"

namespace dargon { namespace IO { namespace DSP { 
   class DefaultDSPExInstructionSet : public IDSPExInstructionSet
   {
      virtual bool TryConstructRITHandler(UINT32 transactionId, DSPEx opcode, OUT DSPExRITransactionHandler** ppResult) override;
   };
} } }