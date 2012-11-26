#include "alloc_code_gen.hpp"

#include "../../log/log.hpp"

#include "../types.hpp"

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

			llvm::Function * TheFunction = builder.GetInsertBlock()->getParent();
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