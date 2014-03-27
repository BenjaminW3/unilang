#pragma once

#include <string>
#include <memory>

// base classes
#include <boost/noncopyable.hpp>

// members
#include <vector>
#include <unilang/compiler/code_generator/symbols/Variable.hpp>

// forward declarations
namespace llvm
{
	class Type;
}
namespace unilang
{
	namespace code_generator
	{
		class CErrorCodeGen;
		class CLLVMCodeGen;
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
		//! 
		//#########################################################################
		class CSymbolCodeGen :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			CSymbolCodeGen(
				CErrorCodeGen & codeGeneratorErrors,
				CLLVMCodeGen & llvmCodeGenerator );

			//-------------------------------------------------------------------------
			//! \return The type corresponding to the given Typename
			//-------------------------------------------------------------------------
			llvm::Type * getTypeByName(std::string sTypeName) const;

			//-------------------------------------------------------------------------
			//! \return A new symbol table for the current block inside the current symbol table.
			//-------------------------------------------------------------------------
			//std::shared_ptr<> addBlock();

			//-------------------------------------------------------------------------
			//! \return The Variable with the given name in the current scope.
			//-------------------------------------------------------------------------
			VarData const * getVarFromName(std::string const & name) const;

			//-------------------------------------------------------------------------
			//! Adds the given variable to the symbol table in the current scope.
			//-------------------------------------------------------------------------
			void addVar(VarData const & var);

			//-------------------------------------------------------------------------
			//! TODO: Remove clearSymbolTable!
			//-------------------------------------------------------------------------
			void clearSymbolTable();

		private:
			std::vector<VarData /*const*/> vSymbolTable;

			CErrorCodeGen & m_codeGeneratorErrors;
			CLLVMCodeGen & m_llvmCodeGenerator;
		};
	}
}