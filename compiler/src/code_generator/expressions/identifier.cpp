#include "exp_code_gen.hpp"

#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"

#include "../../ast/identifier_ast.hpp"

#include "../../log/log.hpp"

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
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::identifier const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			VarData const * const pVariable (m_symbolCodeGenerator.getVarFromName(x._name));
			if(!pVariable)
			{
				return m_codeGeneratorErrors.ErrorValue("Undeclared variable name: '"+x._name+"' !");
			}
			else
			{
				if(!pVariable->getAllocaInst())
				{
					return m_codeGeneratorErrors.ErrorValue("Variable is not allocated: '"+x._name+"' !", EErrorLevel::Internal);
				}
				else
				{
					return m_llvmCodeGenerator.getBuilder()->CreateLoad(pVariable->getAllocaInst(), x._name.c_str());
				}
			}
		}
	}
}