#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>

#include <unilang/compiler/code_generator/symbols/symbols.hpp>
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>

#include <algorithm>

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
#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//! Constructor
		//-------------------------------------------------------------------------
		symbol_code_generator::symbol_code_generator(	code_generator_errors & codeGeneratorErrors,
														llvm_code_generator & llvmCodeGenerator ) :
			m_codeGeneratorErrors	(codeGeneratorErrors),
			m_llvmCodeGenerator		(llvmCodeGenerator)
		{
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Type* symbol_code_generator::getTypeByName(std::string sTypeName) const
		{
			if(sTypeName=="f64")
			{
				return llvm::Type::getDoubleTy(m_llvmCodeGenerator.getContext());
			}
			else if(sTypeName=="f32")
			{
				return llvm::Type::getFloatTy(m_llvmCodeGenerator.getContext());
			}
			else if(sTypeName=="i64")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),64);
			}
			else if(sTypeName=="i32")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),32);
			}
			else if(sTypeName=="i16")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),16);
			}
			else if(sTypeName=="i8")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),8);
			}
			else if(sTypeName=="i1")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),1);
			}
			/*else if(sTypeName=="int")
			{
				// TODO: For now int is a hard coded 64bit integer. this has to be target architecture dependant.
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),64);
			}*/
			else
			{
				return m_codeGeneratorErrors.ErrorType("Use of unknown Type: '"+sTypeName+"'.");
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		VarData const * const symbol_code_generator::getVarFromName( std::string const & name ) const
		{
			const auto itlocal = std::find_if(vSymbolTable.begin(), vSymbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(itlocal!=vSymbolTable.end())
			{
				return &(*itlocal);
			}
			return nullptr;
		}
	}
}