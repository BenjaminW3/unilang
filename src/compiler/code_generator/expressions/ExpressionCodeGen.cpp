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

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/constants/ConstantsCodeGen.hpp>
#include <unilang/compiler/code_generator/allocation/AllocationCodeGen.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CExpressionCodeGen::CExpressionCodeGen(
			CErrorCodeGen & codeGeneratorErrors,
			CModuleCodeGen & namespaceCodeGenerator,
			CLLVMCodeGen & llvmCodeGenerator,
			CConstantsCodeGen & constantsCodeGenerator,
			CSymbolCodeGen & symbolCodeGenerator,
			CAllocationCodeGen & allocationCodeGenerator ) :
			m_codeGeneratorErrors		(codeGeneratorErrors),
			m_namespaceCodeGenerator	(namespaceCodeGenerator),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_constantsCodeGenerator	(constantsCodeGenerator),
			m_symbolCodeGenerator		(symbolCodeGenerator),
			m_allocationCodeGenerator	(allocationCodeGenerator)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(ast::SPrimaryExpr const & x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*llvm::Value * CExpressionCodeGen::operator()(ast::SOperand const & x)
		{
			LOG(x);
			// get the value
			llvm::Value * pRetVal = x.apply_visitor(*this);
			
			// if it was a definition we have to load the value from memory
			if(x.var.type()==typeid(ast::SVariableDefinition))
			{
				llvm::AllocaInst * pDeclAlloca = (llvm::AllocaInst *) pRetVal;
				pRetVal = m_llvmCodeGenerator.getBuilder()->CreateLoad(pDeclAlloca, "load.var_def.alloca");
			}

			return pRetVal;
		}*/
		
		//-------------------------------------------------------------------------
		// if it was a definition we have to load the value from memory
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(ast::SVariableDefinition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			llvm::AllocaInst * pDeclAlloca (m_allocationCodeGenerator(x));
			if(!pDeclAlloca)
			{
				return m_codeGeneratorErrors.getErrorValue("Unable to generate allocation for variable definition.");
			}

			llvm::Value * pRetVal (m_llvmCodeGenerator.getBuilder()->CreateLoad(pDeclAlloca, "load.var_def.alloca"));
			if(!pRetVal)
			{
				return m_codeGeneratorErrors.getErrorValue("Unable to generate load instruction from variable definition allocation.");
			}

			return pRetVal;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(std::string const & x)
		{
			return m_llvmCodeGenerator.getBuilder()->CreateGlobalStringPtr(x, "create.global_string_ptr");
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(lexer::SFloatLiteral const & x)
		{
			return m_constantsCodeGenerator(x);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(lexer::SIntLiteral const & x)
		{
			return m_constantsCodeGenerator(x);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(bool const & x)
		{
			return m_constantsCodeGenerator(x);
		}
	}
}