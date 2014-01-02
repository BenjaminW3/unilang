#include <unilang/compiler/code_generator/statements/statement_code_gen.hpp>

#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/expressions/exp_code_gen.hpp>

#include <unilang/compiler/ast/statement_ast.hpp>

#include <unilang/compiler/log/log.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::if_statement const & x)
		{
			LOG_SCOPE_DEBUG;

			// generate condition value
			llvm::Value * const CondV (m_expressionCodeGenerator(x._expCondition));
			if (!CondV)
			{
				std::stringstream sstr;
				sstr << "Unable to generate condition: '" << x._expCondition << "' in if_statement '" << x << "'.";
				return m_codeGeneratorErrors.ErrorBool( sstr.str() );
			}
  
			// Convert condition to a bool by comparing equal to 0.0.
			//CondV = getBuilder()->CreateFCmpONE(CondV, (*this)(long double(0.0)), "if.cond");

			llvm::Function *TheFunction (m_llvmCodeGenerator.getBuilder()->GetInsertBlock()->getParent());
			if(!TheFunction)
			{
				std::stringstream sstr;
				sstr << "Unable to get parent function for if_statement '" << x << "'.";
				return m_codeGeneratorErrors.ErrorBool( sstr.str() );
			}
  
			// Create blocks for the then and else cases.
			llvm::BasicBlock * ThenBB	(llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "if.then", TheFunction));
			llvm::BasicBlock * MergeBB	(llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "if.merge"));
			llvm::BasicBlock * ElseBB	(x._elseOptionalStatementList.is_initialized() ? llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "if.else") : MergeBB);

			llvm::BranchInst * condBranch = m_llvmCodeGenerator.getBuilder()->CreateCondBr(CondV, ThenBB, ElseBB);
			if(!condBranch)
			{
				std::stringstream sstr;
				sstr << "Unable to generate condBranch in if_statement '" << x << "'.";
				return m_codeGeneratorErrors.ErrorBool( sstr.str() );
			}

			m_llvmCodeGenerator.getBuilder()->SetInsertPoint(ThenBB);		// Emit then value.

			if (!(*this)(x._thenStatementList))
			{
				std::stringstream sstr;
				sstr << "Unable to generate then block: '" << x._thenStatementList << "' in if_statement '" << x << "'.";
				condBranch->eraseFromParent();
				ThenBB->eraseFromParent();
				//if(x._elseOptionalStatementList.is_initialized()){ElseBB->eraseFromParent();}		// not attached here
				//MergeBB->eraseFromParent();									// not attached here
				return m_codeGeneratorErrors.ErrorBool( sstr.str() );
			}
  
			// create branch to the merge block
			llvm::BranchInst * thenMergeBranch	(m_llvmCodeGenerator.getBuilder()->CreateBr(MergeBB));
			if(!thenMergeBranch)
			{
				condBranch->eraseFromParent();
				ThenBB->eraseFromParent();
				//if(x._elseOptionalStatementList.is_initialized()){ElseBB->eraseFromParent();}		// not attached here
				//MergeBB->eraseFromParent();									// not attached here
				std::stringstream sstr;
				sstr << "Unable to generate thenMergeBranch in if_statement '" << x << "'.";
				return m_codeGeneratorErrors.ErrorBool( sstr.str() );
			}

			ThenBB = m_llvmCodeGenerator.getBuilder()->GetInsertBlock();		// Codegen of 'Then' can change the current block.

			if(x._elseOptionalStatementList.is_initialized())
			{
				// Emit else block.
				TheFunction->getBasicBlockList().push_back(ElseBB);
				m_llvmCodeGenerator.getBuilder()->SetInsertPoint(ElseBB);
  
				if (!(*this)(x._elseOptionalStatementList.get()))
				{
					std::stringstream sstr;
					sstr << "Unable to generate else block: '" << x._elseOptionalStatementList.get() << "' in if_statement '" << x << "'.";
					condBranch->eraseFromParent();
					ThenBB->eraseFromParent();
					thenMergeBranch->eraseFromParent();
					ElseBB->eraseFromParent();
					//MergeBB->eraseFromParent();								// not attached here
					return m_codeGeneratorErrors.ErrorBool( sstr.str() );
				}
  
				// create branch to the merge block
				llvm::BranchInst * elseMergeBranch	(m_llvmCodeGenerator.getBuilder()->CreateBr(MergeBB));
				if(!elseMergeBranch)
				{
					condBranch->eraseFromParent();
					ThenBB->eraseFromParent();
					thenMergeBranch->eraseFromParent();
					ElseBB->eraseFromParent();
					//MergeBB->eraseFromParent();								// not attached here
					std::stringstream sstr;
					sstr << "Unable to generate elseMergeBranch in if_statement '" << x << "'.";
					return m_codeGeneratorErrors.ErrorBool( sstr.str() );
				}
				ElseBB = m_llvmCodeGenerator.getBuilder()->GetInsertBlock();	// Codegen of 'Else' can change the current block
			}

			// Emit merge basic block.
			TheFunction->getBasicBlockList().push_back(MergeBB);

			// following code is inserted after the merge block
			m_llvmCodeGenerator.getBuilder()->SetInsertPoint(MergeBB);

			return true;
		}
	}
}