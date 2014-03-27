#pragma once

// base classes 
#include <boost/noncopyable.hpp>

// members
#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/Variable.hpp>
#include <unilang/compiler/code_generator/module/ModuleCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/constants/ConstantsCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>
#include <unilang/compiler/code_generator/allocation/AllocationCodeGen.hpp>
#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>
#include <unilang/compiler/code_generator/statements/StatementCodeGen.hpp>
#include <unilang/compiler/code_generator/functions/CFunctionCodeGen.hpp>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SModule;
	}
}

namespace unilang
{ 
	//-------------------------------------------------------------------------
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! Generates the code from an AST.
		//!
		//! Given a ast::module it recursively creates the corresponding llvm::Module.
		//#########################################################################
		class CCodeGenerator :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CCodeGenerator( ast::SModule const & AST );
			
			//-------------------------------------------------------------------------
			//! The CLLVMCodeGen holds the result of the compilation.
			//! \return The internal llvm code generator used.
			//-------------------------------------------------------------------------
			CLLVMCodeGen const & getllvmCodeGenerator() const;

		//private:
			//void addStringConverters();

		private:
			CErrorCodeGen m_codeGeneratorErrors;
			CModuleCodeGen m_namespaceCodeGenerator;
			CLLVMCodeGen m_llvmCodeGenerator;
			CConstantsCodeGen m_constantsCodeGenerator;
			CSymbolCodeGen m_symbolCodeGenerator;
			CAllocationCodeGen m_allocationCodeGenerator;
			CExpressionCodeGen m_expressionCodeGenerator;
			CStatementCodeGen m_statementCodeGenerator;
			CFunctionCodeGen m_functionCodeGenerator;
		};
	}
}