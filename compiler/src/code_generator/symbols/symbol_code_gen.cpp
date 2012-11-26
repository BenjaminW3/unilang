#include "symbol_code_gen.hpp"

#include "../../log/log.hpp"

#include <llvm/Type.h>

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Type* symbol_code_generator::getTypeByName(std::string sTypeName)
		{
			// FIXME: hard coded types
			if(sTypeName=="f64")
			{
				return llvm::Type::getDoubleTy(getContext());
			}
			if(sTypeName=="f32")
			{
				return llvm::Type::getFloatTy(getContext());
			}
			else if(sTypeName=="i64")
			{
				return llvm::Type::getIntNTy(getContext(),64);
			}
			else if(sTypeName=="i32")
			{
				return llvm::Type::getIntNTy(getContext(),32);
			}
			else
			{
				return ErrorType("Use of unknown Type: '"+sTypeName+"'.");
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		VarData const * const symbol_code_generator::getVarFromName( std::string const & name )
		{
			// local search first!
			const auto itlocal = std::find_if(vLocalSymbolTable.begin(), vLocalSymbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(itlocal!=vLocalSymbolTable.end())
			{
				return &(*itlocal);
			}

			// then global search
			const auto itglobal = std::find_if(vGlobalSymbolTable.begin(), vGlobalSymbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(itglobal!=vGlobalSymbolTable.end())
			{
				return &(*itglobal);
			}
			return nullptr;
		}
	}
}