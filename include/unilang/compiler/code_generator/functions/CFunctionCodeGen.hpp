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
	namespace ast
	{
		struct SFunctionDeclaration;
		struct SFunctionDefinition;
	}

	namespace code_generator
	{
		class CErrorCodeGen;
		class CModuleCodeGen;
		class CLLVMCodeGen;
		class CSymbolCodeGen;
		class CAllocationCodeGen;
		class CExpressionCodeGen;
		class CStatementCodeGen;
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
		class CFunctionCodeGen :
			//public boost::static_visitor<llvm::Function * >,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CFunctionCodeGen(
				CErrorCodeGen & codeGeneratorErrors,
				CModuleCodeGen & namespaceCodeGenerator,
				CLLVMCodeGen & llvmCodeGenerator,
				CSymbolCodeGen & symbolCodeGenerator,
				CAllocationCodeGen & allocationCodeGenerator,
				CExpressionCodeGen & expressionCodeGenerator,
				CStatementCodeGen & statementCodeGenerator);

		public:
			typedef llvm::Function * result_type;

			llvm::Function * operator()(ast::SFunctionDeclaration const & x);
			llvm::Function * operator()(ast::SFunctionDefinition const & x);
			
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
			CErrorCodeGen & m_codeGeneratorErrors;
			CModuleCodeGen & m_namespaceCodeGenerator;
			CLLVMCodeGen & m_llvmCodeGenerator;
			CSymbolCodeGen & m_symbolCodeGenerator;
			CAllocationCodeGen & m_allocationCodeGenerator;
			CExpressionCodeGen & m_expressionCodeGenerator;
			CStatementCodeGen & m_statementCodeGenerator;
		};
	}
}