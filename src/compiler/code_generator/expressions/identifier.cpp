#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>

#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>

#include <unilang/compiler/ast/Identifier.hpp>

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
		llvm::Value * CExpressionCodeGen::operator()(ast::SIdentifier const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			VarData const * const pVariable (m_symbolCodeGenerator.getVarFromName(x.m_sName));
			if(!pVariable)
			{
				return m_codeGeneratorErrors.getErrorValue("Undeclared variable name: '"+x.m_sName+"' !");
			}
			else
			{
				if(!pVariable->getAllocaInst())
				{
					return m_codeGeneratorErrors.getErrorValue("Variable is not allocated: '"+x.m_sName+"' !", EErrorLevel::Internal);
				}
				else
				{
					return m_llvmCodeGenerator.getBuilder()->CreateLoad(pVariable->getAllocaInst(), x.m_sName.c_str());
				}
			}
		}
	}
}