#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::if_statement const& x)
		{
			LOG_SCOPE_DEBUG;

			llvm::Value *CondV = (*this)(x.condition);
			if (!CondV)
			{
				throw std::runtime_error("Invalid condition!");
			}
  
			// Convert condition to a bool by comparing equal to 0.0.
			CondV = builder.CreateFCmpONE(CondV, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "ifcond");

			llvm::Function *TheFunction = builder.GetInsertBlock()->getParent();
  
			// Create blocks for the then and else cases.  Insert the 'then' block at the
			// end of the function.
			llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(context, "then", TheFunction);
			llvm::BasicBlock *ElseBB = nullptr;
			if(x.else_.is_initialized())
			{
				ElseBB = llvm::BasicBlock::Create(context, "else");
			}
			llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(context, "ifcont");

			builder.CreateCondBr(CondV, ThenBB, ElseBB);
			builder.SetInsertPoint(ThenBB);		// Emit then value.

			llvm::Value *ThenV = (*this)(x.then);
			if (!ThenV) 
			{
				throw std::runtime_error("Invalid then block!");
			}
  
			builder.CreateBr(MergeBB);
			ThenBB = builder.GetInsertBlock();		// Codegen of 'Then' can change the current block, update ThenBB for the PHI.

			llvm::Value *ElseV = nullptr;
			if(x.else_.is_initialized())
			{
				// Emit else block.
				TheFunction->getBasicBlockList().push_back(ElseBB);
				builder.SetInsertPoint(ElseBB);
  
				ElseV = (*this)(x.else_.get());
				if (!ElseV)
				{
					throw std::runtime_error("Invalid else block!");
				}
  
				builder.CreateBr(MergeBB);
				ElseBB = builder.GetInsertBlock();	// Codegen of 'Else' can change the current block, update ElseBB for the PHI.
			}

			// Emit merge block.
			TheFunction->getBasicBlockList().push_back(MergeBB);
			builder.SetInsertPoint(MergeBB);

			llvm::PHINode *PN = builder.CreatePHI(llvm::Type::getDoubleTy(context), 2, "iftmp");
			PN->addIncoming(ThenV, ThenBB);
			if(x.else_.is_initialized())
			{
				PN->addIncoming(ElseV, ElseBB);
			}
			return PN;
		}
	}
}