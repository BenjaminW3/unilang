#pragma once

// base classes 
#include <boost/noncopyable.hpp>

// members
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/symbols/symbols.hpp>
#include <unilang/compiler/code_generator/namespace/namespace_code_gen.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/constants/constants_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>
#include <unilang/compiler/code_generator/allocation/alloc_code_gen.hpp>
#include <unilang/compiler/code_generator/expressions/exp_code_gen.hpp>
#include <unilang/compiler/code_generator/statements/statement_code_gen.hpp>
#include <unilang/compiler/code_generator/functions/function_code_gen.hpp>

namespace unilang
{ 
	// forward declarations
	namespace ast
	{
		struct module;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! Generates the code from an AST.
		//!
		//! Given a ast::module it recursively creates the corresponding llvm::Module.
		//#########################################################################
		class code_generator :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			code_generator( ast::module const & AST );
			
			//-------------------------------------------------------------------------
			//! The llvm_code_generator holds the result of the compilation.
			//! \return The internal llvm code generator used.
			//-------------------------------------------------------------------------
			llvm_code_generator const & getllvmCodeGenerator() const;

		private:
			void addStringConverters();

		private:
			code_generator_errors m_codeGeneratorErrors;
			namespace_code_generator m_namespaceCodeGenerator;
			llvm_code_generator m_llvmCodeGenerator;
			constants_code_generator m_constantsCodeGenerator;
			symbol_code_generator m_symbolCodeGenerator;
			allocation_code_generator m_allocationCodeGenerator;
			expression_code_generator m_expressionCodeGenerator;
			statement_code_generator m_statementCodeGenerator;
			function_code_generator m_functionCodeGenerator;
		};
	}
}