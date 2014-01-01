#pragma once

#include <string>

// base classes
#include <boost/noncopyable.hpp>

// members
#include <vector>
#include <unilang/compiler/code_generator/symbols/symbols.hpp>

// forward declarations
namespace llvm
{
	class Type;
}

namespace unilang
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class llvm_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class symbol_code_generator :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			symbol_code_generator(	code_generator_errors & codeGeneratorErrors,
									llvm_code_generator & llvmCodeGenerator );

			//-------------------------------------------------------------------------
			//! \return The type corresponding to the given Typename
			//-------------------------------------------------------------------------
			llvm::Type* getTypeByName(std::string sTypeName) const;
			//-------------------------------------------------------------------------
			//! \return The Variable with the given name in the current scope.
			//-------------------------------------------------------------------------
			VarData const * const getVarFromName( std::string const & name ) const;

			// FIXME: private
			std::vector<VarData> vSymbolTable;

		private:
			code_generator_errors & m_codeGeneratorErrors;
			llvm_code_generator & m_llvmCodeGenerator;
		};
	}
}