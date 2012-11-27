#include "alloc_code_gen.hpp"

#include "../../log/log.hpp"

#include "../types.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
//#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/IRBuilder.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * allocation_code_generator::operator()(ast::variable_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// has name?
			const bool bHasName		(x._optionalIdentifier.is_initialized());
			std::string const sName (bHasName ? x._optionalIdentifier.get()._name : "");

			// redeclaration?
			if(bHasName)
			{
				VarData const * const existantVar = getVarFromName(sName);
				if(existantVar)
				{
					return ErrorAllocaInst("Variable with the name '"+sName+"' has already been declared with type '"+getLLVMTypeName(existantVar->getAllocaInst()->getAllocatedType())+"'.");
				}
			}
			// TODO: really needed? We already cant have typenames equal to variable names
			// shadowing keyword?
			if(bHasName && (sName == "if" || sName == "else" || sName == "while" || sName == "return"))
			{
				return ErrorAllocaInst("Variable with the name '"+sName+"' is shadowing the keyword with same identifier.");
			}

			llvm::Function * TheFunction = getBuilder()->GetInsertBlock()->getParent();
			if(!TheFunction)
			{
				return ErrorAllocaInst("Unable to get the allocation insert point function for variable '"+sName+"'.");
			}

			// allocate in function head
			llvm::AllocaInst * Alloca = createEntryBlockAlloca(TheFunction, getTypeByName(x._type._identifier._name), sName);
			if(!Alloca)
			{
				return ErrorAllocaInst("Unable to allocate variable '"+sName+"'.");
			}

			// remember this variable in symbol table
			if(bHasName)
			{
				vLocalSymbolTable.push_back(VarData(sName, Alloca, x._type._bHasMutableQualifier));
			}

			return Alloca;
		}
	}
}