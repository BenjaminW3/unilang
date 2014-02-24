#include <unilang/compiler/code_generator/allocation/alloc_code_gen.hpp>

#include <unilang/compiler/code_generator/types.hpp>
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>
#include <unilang/compiler/ast/expression_ast.hpp>

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
		llvm::AllocaInst * allocation_code_generator::operator()(ast::variable_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Is this a named allocation?
			bool const bHasName		(x._optionalIdentifier.is_initialized());
			std::string const sName	(bHasName ? x._optionalIdentifier.get().m_sName : "");

			// If it has a name...
			if(bHasName)
			{
				// ... is this a redeclaration?
				VarData const * const existantVar (m_symbolCodeGenerator.getVarFromName(sName));
				if(existantVar)
				{
					return m_codeGeneratorErrors.ErrorAllocaInst("Variable with the name '"+sName+"' has already been declared with type '"+getLLVMTypeName(existantVar->getAllocaInst()->getAllocatedType())+"'.");
				}

				// TODO: really needed? Can we have variable names equal to reserved names?
				// ... is it Shadowing a keyword?
				if(sName == "if" || sName == "else" /*|| sName == "while" || sName == "return"*/ || sName == "namespace")
				{
					return m_codeGeneratorErrors.ErrorAllocaInst("Variable with the name '"+sName+"' is shadowing the keyword with same identifier.");
				}

				// TODO: Can we have variable names equal to type names?
			}

			// Get the current function.
			llvm::Function * const pCurrentFunction(m_llvmCodeGenerator.getBuilder()->GetInsertBlock()->getParent());
			if(!pCurrentFunction)
			{
				return m_codeGeneratorErrors.ErrorAllocaInst("Unable to get the function the allocation for variable '"+sName+"' should be inserted into.");
			}
			// Get the variable type.
			llvm::Type * const pType(m_symbolCodeGenerator.getTypeByName(x._type._idfName.m_sName));
			if(!pType)
			{
				return m_codeGeneratorErrors.ErrorAllocaInst("Unable to get type of variable '"+sName+"'.");
			}
			// Allocate the variable in the function head.
			llvm::AllocaInst * const Alloca(createEntryBlockAlloca(pCurrentFunction, pType, sName));
			if(!Alloca)
			{
				return m_codeGeneratorErrors.ErrorAllocaInst("Unable to allocate variable '"+sName+"'.");
			}

			// Remember this variable in symbol table.
			if(bHasName)
			{
				m_symbolCodeGenerator.addVar(VarData(sName, Alloca, x._type._bHasMutableQualifier));
			}

			return Alloca;
		}
	}
}