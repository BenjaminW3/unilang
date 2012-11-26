#include "statement_code_gen.hpp"

#include "../../log/log.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#endif

#include "llvm/Function.h"

#if defined(_MSC_VER)
#pragma warning(push)
#endif

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::if_statement const& x)
		{
			LOG_SCOPE_DEBUG;

			llvm::Value *CondV = (*static_cast<expression_code_generator*>(this))(x._condition);
			if (!CondV)
			{
				std::stringstream sstr;
				sstr << "Unable to generate _condition: '" << x._condition << "' in if_statement '" << x << "'.";
				return ErrorBool( sstr.str() );
			}
  
			// Convert condition to a bool by comparing equal to 0.0.
			//CondV = builder.CreateFCmpONE(CondV, (*this)(long double(0.0)), "if.cond");

			llvm::Function *TheFunction = builder.GetInsertBlock()->getParent();
			if(!TheFunction)
			{
				std::stringstream sstr;
				sstr << "Unable to get parent function for if_statement '" << x << "'.";
				return ErrorBool( sstr.str() );
			}
  
			// Create blocks for the then and else cases.  Insert the 'then' block at the end of the function.
			llvm::BasicBlock * ThenBB = llvm::BasicBlock::Create(context, "if._then", TheFunction);
			llvm::BasicBlock * ElseBB = x._else.is_initialized() ? llvm::BasicBlock::Create(context, "if.else") : nullptr;
			llvm::BasicBlock * MergeBB = llvm::BasicBlock::Create(context, "if.merge");

			llvm::BranchInst * condBranch = builder.CreateCondBr(CondV, ThenBB, ElseBB);
			if(!condBranch)
			{
				std::stringstream sstr;
				sstr << "Unable to generate condBranch in if_statement '" << x << "'.";
				return ErrorBool( sstr.str() );
			}

			builder.SetInsertPoint(ThenBB);		// Emit _then value.

			if (!(*this)(x._then)) 
			{
				std::stringstream sstr;
				sstr << "Unable to generate _then block: '" << x._then << "' in if_statement '" << x << "'.";
				condBranch->eraseFromParent();
				ThenBB->eraseFromParent();
				//if(x._else.is_initialized()){ElseBB->eraseFromParent();}		// not attached here
				//MergeBB->eraseFromParent();									// not attached here
				return ErrorBool( sstr.str() );
			}
  
			// create branch to the merge block
			llvm::BranchInst * thenMergeBranch = builder.CreateBr(MergeBB);
			if(!thenMergeBranch)
			{
				condBranch->eraseFromParent();
				ThenBB->eraseFromParent();
				//if(x._else.is_initialized()){ElseBB->eraseFromParent();}		// not attached here
				//MergeBB->eraseFromParent();									// not attached here
				std::stringstream sstr;
				sstr << "Unable to generate thenMergeBranch in if_statement '" << x << "'.";
				return ErrorBool( sstr.str() );
			}

			ThenBB = builder.GetInsertBlock();		// Codegen of 'Then' can change the current block, update ThenBB for the PHI.

			llvm::BranchInst * elseMergeBranch = nullptr;
			if(x._else.is_initialized())
			{
				// Emit else block.
				TheFunction->getBasicBlockList().push_back(ElseBB);
				builder.SetInsertPoint(ElseBB);
  
				if (!(*this)(x._else.get()))
				{
					std::stringstream sstr;
					sstr << "Unable to generate else block: '" << x._else << "' in if_statement '" << x << "'.";
					condBranch->eraseFromParent();
					ThenBB->eraseFromParent();
					thenMergeBranch->eraseFromParent();
					ElseBB->eraseFromParent();
					//MergeBB->eraseFromParent();								// not attached here
					return ErrorBool( sstr.str() );
				}
  
				// create branch to the merge block
				elseMergeBranch = builder.CreateBr(MergeBB);
				if(!elseMergeBranch)
				{
					condBranch->eraseFromParent();
					ThenBB->eraseFromParent();
					thenMergeBranch->eraseFromParent();
					ElseBB->eraseFromParent();
					//MergeBB->eraseFromParent();								// not attached here
					std::stringstream sstr;
					sstr << "Unable to generate elseMergeBranch in if_statement '" << x << "'.";
					return ErrorBool( sstr.str() );
				}
				ElseBB = builder.GetInsertBlock();	// Codegen of 'Else' can change the current block, update ElseBB for the PHI.
			}

			// Emit merge block.
			TheFunction->getBasicBlockList().push_back(MergeBB);
			builder.SetInsertPoint(MergeBB);

			return true;
		}
	}
}