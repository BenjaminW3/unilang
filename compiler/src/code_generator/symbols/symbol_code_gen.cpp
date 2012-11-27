#include "symbol_code_gen.hpp"

#include "../../log/log.hpp"

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

#include <llvm/IRBuilder.h>
#include <llvm/Type.h>
#include <llvm/LLVMContext.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <algorithm>

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