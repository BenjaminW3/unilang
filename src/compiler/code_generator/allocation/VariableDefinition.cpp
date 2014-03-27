#include <unilang/compiler/code_generator/allocation/AllocationCodeGen.hpp>

#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>
#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>

#include <unilang/compiler/ast/Expression.hpp>

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
		llvm::AllocaInst * CAllocationCodeGen::operator()(ast::SVariableDefinition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			
			bool const bHasName		(x.m_declaration.m_optionalIdentifier.is_initialized());
			
			// TODO: hard coded types
			std::string const sTypeName		(x.m_declaration.m_type.m_idfName.m_sName);
			auto const pDeclType	(m_symbolCodeGenerator.getTypeByName(sTypeName));
			if(!pDeclType)
			{
				auto const sVarName (bHasName ? x.m_declaration.m_optionalIdentifier.get().m_sName : "");
				return m_codeGeneratorErrors.getErrorAllocaInst("Unable to get type of variable '"+sVarName+"'.");
			}

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
			llvm::Value * pInitVal	(nullptr);

			if(pDeclType->isIntegerTy() || pDeclType->isFloatingPointTy())
			{
				if(x.m_vParameterExpressions.size()==0)
				{
					pInitVal = llvm::Constant::getNullValue(pDeclType);
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x.m_vParameterExpressions.begin();
						auto const sVarName (bHasName ? x.m_declaration.m_optionalIdentifier.get().m_sName : "");
						return m_codeGeneratorErrors.getErrorAllocaInst("Initial null value for variable '"+sVarName+"' of type '"+sTypeName+"' could not be generated.");
					}
				}
				else if(x.m_vParameterExpressions.size()==1)
				{
					pInitVal = m_expressionCodeGenerator(*x.m_vParameterExpressions.begin());
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x.m_vParameterExpressions.begin();
						auto const sVarName (bHasName ? x.m_declaration.m_optionalIdentifier.get().m_sName : "");
						// TODO: output real parameter number
						return m_codeGeneratorErrors.getErrorAllocaInst("Invalid 1. initialisation parameter '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"'");
					}
					else if(pInitVal->getType() != pDeclType)
					{
						pInitVal = createCast(pInitVal, pDeclType);
						if(!pInitVal)
						{
							std::stringstream sstr;
							sstr << *x.m_vParameterExpressions.begin();
							auto const sVarName (bHasName ? x.m_declaration.m_optionalIdentifier.get().m_sName : "");
							return m_codeGeneratorErrors.getErrorAllocaInst("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"' can not be cast.");
						}
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x.m_vParameterExpressions.size();
					return m_codeGeneratorErrors.getErrorAllocaInst("Variable of type '"+sTypeName+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}
			else
			{
				return m_codeGeneratorErrors.getErrorAllocaInst("Unknown type '"+sTypeName+"' in variable definition.");
			}

			// build declaration (allocation, etc.)
			llvm::AllocaInst * pDeclAlloc ((*this)(x.m_declaration));
			if(!pDeclAlloc)
			{
				auto const sVarName (bHasName ? x.m_declaration.m_optionalIdentifier.get().m_sName : "");
				return m_codeGeneratorErrors.getErrorAllocaInst("Unable to declare variable '"+sVarName+"' of type '"+sTypeName+"'.");
			}

			m_llvmCodeGenerator.getBuilder()->CreateStore(pInitVal, pDeclAlloc);

			return pDeclAlloc;
		}
	}
}