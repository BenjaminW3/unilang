#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>

#include <unilang/compiler/code_generator/symbols/Variable.hpp>
#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>

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
		//
		//-------------------------------------------------------------------------
		CSymbolCodeGen::CSymbolCodeGen(
			CErrorCodeGen & codeGeneratorErrors,
			CLLVMCodeGen & llvmCodeGenerator ) :
			m_codeGeneratorErrors	(codeGeneratorErrors),
			m_llvmCodeGenerator		(llvmCodeGenerator)
		{
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Type* CSymbolCodeGen::getTypeByName(std::string sTypeName) const
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
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(), 64);
			}
			else if(sTypeName=="i32")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(), 32);
			}
			else if(sTypeName=="i16")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(), 16);
			}
			else if(sTypeName=="i8")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(), 8);
			}
			else if(sTypeName=="i1")
			{
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(), 1);
			}
			/*else if(sTypeName=="int")
			{
				// TODO: For now int is a hard coded 64bit integer. this has to be target architecture dependant.
				return llvm::Type::getIntNTy(m_llvmCodeGenerator.getContext(),64);
			}*/
			else
			{
				return m_codeGeneratorErrors.getErrorType("Use of unknown Type: '"+sTypeName+"'.");
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		VarData const * CSymbolCodeGen::getVarFromName( std::string const & name ) const
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
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CSymbolCodeGen::addVar(VarData const & var)
		{
			vSymbolTable.emplace_back(var);
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CSymbolCodeGen::clearSymbolTable()
		{
			vSymbolTable.clear();
		}
	}
}