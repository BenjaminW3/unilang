#pragma once

#include <string>
#include <vector>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class Function;
}

namespace unilang
{ 
	// forward declarations
	namespace ast
	{
		struct function_declaration;
		struct function_definition;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class namespace_code_generator;
		class llvm_code_generator;
		class symbol_code_generator;
		class allocation_code_generator;
		class statement_code_generator;

		//#########################################################################
		//! Generates the code from an AST.
		//!
		//! Given a ast::module it recursively creates the corresponding llvm::Module.
		//#########################################################################
		class function_code_generator :
			//public boost::static_visitor<llvm::Function * >,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_code_generator(code_generator_errors & codeGeneratorErrors,
									namespace_code_generator & namespaceCodeGenerator,
									llvm_code_generator & llvmCodeGenerator,
									symbol_code_generator & symbolCodeGenerator,
									allocation_code_generator & allocationCodeGenerator,
									statement_code_generator & statementCodeGenerator);

		public:
			typedef llvm::Function * result_type;

			llvm::Function * operator()(ast::function_declaration const & x);
			llvm::Function * operator()(ast::function_definition const & x);
			
			template<typename R, typename T, typename... Args>
			llvm::Function * declare_extern(std::string name, R (*fp)(Args... args))
			{
				std::vector<std::string> lParameterTypeNames;
				assembleCTypeToUnilangTypeName(lParameterTypeNames, args...);
				std::vector<std::string> lReturnTypeNames;
				assembleCTypeToUnilangTypeName(lReturnTypeNames, cTypeToUnilangTypeName<R>())
				return declare_extern(name, lReturnTypeNames, lParameterTypeNames, false, fp);
			}
			llvm::Function * declare_extern(std::string const & name, std::vector<std::string> const & resTypes, std::vector<std::string> const & paramTypes, bool _bIsVarArg, void * const fp);

		private:
			code_generator_errors & m_codeGeneratorErrors;
			namespace_code_generator & m_namespaceCodeGenerator;
			llvm_code_generator & m_llvmCodeGenerator;
			symbol_code_generator & m_symbolCodeGenerator;
			allocation_code_generator & m_allocationCodeGenerator;
			statement_code_generator & m_statementCodeGenerator;
		};
	}
}