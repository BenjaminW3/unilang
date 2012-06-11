#include "code_generator.hpp"

#include "../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::variable_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// has name?
			bool bHasName = x.name.is_initialized();

			// redefinition?
			if(bHasName && getVarFromName(x.name.get().name))
			{
				return ErrorV("Variable with the name '"+x.name.get().name+"' has already been definied with type '"+x.type.type_name.name+"' !");
			}

			llvm::Function * TheFunction = builder.GetInsertBlock()->getParent();

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
			llvm::Value * InitVal = nullptr;
			
			// FIXME: hard coded types
			if(x.type.type_name.name == "int")
			{
				if (x.parameters.is_initialized())
				{
					if(x.parameters.get().size()==1)
					{
						// FIXME: convert initialisation parameter expression to type needed
						InitVal = (*this)(*x.parameters.get().begin());
						if (!InitVal)
						{
							std::stringstream sstr;
							sstr << *x.parameters.get().begin();
							auto sVarName = bHasName ? " '"+x.name.get().name+"'" : "";
							// TODO: output real parameter number
							return ErrorV("Invalid 1. initialisation parameter'"+sstr.str()+"' for variable"+sVarName+" of type '"+x.type.type_name.name+"'");
						}
					}
					else
					{
						std::stringstream sstr;
						sstr << x.parameters.get().size();
						return ErrorV("Variable of type '"+x.type.type_name.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
					}
				}
				else
				{
					InitVal = llvm::ConstantInt::get(context, llvm::APInt(64, 0, true)); // 64bit, initial 0, signed
				}
			}
			if(x.type.type_name.name == "float")
			{
				if (x.parameters.is_initialized())
				{
					if(x.parameters.get().size()==1)
					{
						// FIXME: convert initialisation parameter expression to type needed
						InitVal = (*this)(*x.parameters.get().begin());
						if (!InitVal)
						{
							std::stringstream sstr;
							sstr << *x.parameters.get().begin();
							auto sVarName = bHasName ? " '"+x.name.get().name+"'" : "";
							// TODO: output real parameter number
							return ErrorV("Invalid 1. initialisation parameter'"+sstr.str()+"' for variable"+sVarName+" of type '"+x.type.type_name.name+"'");
						}
					}
					else
					{
						std::stringstream sstr;
						sstr << x.parameters.get().size();
						return ErrorV("Variable of type '"+x.type.type_name.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
					}
				}
				else
				{
					// If not specified, use 0.0.
					InitVal = llvm::ConstantFP::get(context, llvm::APFloat(0.0));
				}
			}

			// allocate in function head
			llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, getTypeByName(x.type.type_name.name), bHasName ? x.name.get().name : "");
			builder.CreateStore(InitVal, Alloca);

			// remember this variable in symbol table
			if(bHasName)
			{
				symbolTable.push_back(code_generator::VarData(x.name.get().name, Alloca, x.type.mutableQualifier));
			}

			return Alloca;
		}
	}
}