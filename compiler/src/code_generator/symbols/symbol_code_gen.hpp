#pragma once

#include "../llvm/llvm_code_gen.hpp"

#include "symbols.hpp"
#include "../errors.hpp"

#include <string>

#include <boost/noncopyable.hpp>

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class symbol_code_generator :	public virtual code_generator_errors,
										public virtual llvm_code_generator,
										virtual boost::noncopyable
		{
		protected:
			//-----------------------------------------------------------------------------
			//! \return The type corresponding to the given Typename
			//-----------------------------------------------------------------------------
			llvm::Type* getTypeByName(std::string sTypeName);
			
			//-----------------------------------------------------------------------------
			//! \return The Variable with the given name in the current scope.
			//-----------------------------------------------------------------------------
			VarData const * const getVarFromName( std::string const & name );

			std::vector<VarData> vLocalSymbolTable;
			std::vector<VarData> vGlobalSymbolTable;
		};
	}
}