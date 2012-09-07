#include "code_generator.hpp"

#include "../log/log.hpp"

#include "llvm/Support/raw_ostream.h"
//#include "llvm/Module.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#endif

#include "llvm/Function.h"

#if defined(_MSC_VER)
#pragma warning(push)
#endif

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * code_generator::CreateEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName)
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
			const std::string sTempName = "EntryBlockAlloca_" + VarName;
			return TmpB.CreateAlloca(pType, 0,  sTempName.c_str());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::variable_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// has name?
			const bool bHasName = x.name.is_initialized();

			// redefinition?
			if(bHasName && getVarFromName(x.name.get().name))
			{
				return ErrorV("Variable with the name '"+x.name.get().name+"' has already been definied with type '"+x.type.type_identifier.name+"' !");
			}

			llvm::Function * TheFunction = builder.GetInsertBlock()->getParent();

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
		//llvm::Value * InitVal = nullptr;

			// allocate in function head
			llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, getTypeByName(x.type.type_identifier.name), bHasName ? x.name.get().name : "");
		//builder.CreateStore(InitVal, Alloca);

			// remember this variable in symbol table
			if(bHasName)
			{
				vLocalSymbolTable.push_back(code_generator::VarData(x.name.get().name, Alloca, x.type.mutableQualifier));
			}

			return Alloca;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::variable_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// has name?
			const bool bHasName = x.decl.name.is_initialized();

			// redefinition?
			if(bHasName && getVarFromName(x.decl.name.get().name))
			{
				return ErrorV("Variable with the name '"+x.decl.name.get().name+"' has already been definied with type '"+x.decl.type.type_identifier.name+"' !");
			}

			llvm::Function * TheFunction = builder.GetInsertBlock()->getParent();

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
			llvm::Value * InitVal = nullptr;
			
			// FIXME: hard coded types
			if(x.decl.type.type_identifier.name == "int")
			{
				if(x.parameters.size()==0)
				{
					InitVal = llvm::ConstantInt::get(context, llvm::APInt(64, 0, true)); // 64bit, initial 0, signed
				}
				else if(x.parameters.size()==1)
				{
					// TODO: if expression not int, convert initialisation parameter expression
					InitVal = (*this)(*x.parameters.begin());
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						// TODO: output real parameter number
						return ErrorV("Invalid 1. initialisation parameter '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"'");
					}
					else if(!InitVal->getType()->isIntegerTy())
					{
						std::string type_str;
						llvm::raw_string_ostream rso(type_str);
						InitVal->getType()->print(rso);

						std::stringstream sstr;
						sstr << *x.parameters.begin();

						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";

						return ErrorV("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' is of type '"+ type_str +"'.");
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x.parameters.size();
					return ErrorV("Variable of type '"+x.decl.type.type_identifier.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}
			if(x.decl.type.type_identifier.name == "float")
			{
				if(x.parameters.size()==0)
				{
					// If not specified, use 0.0.
					InitVal = llvm::ConstantFP::get(context, llvm::APFloat(0.0));
				}
				else if(x.parameters.size()==1)
				{
					// TODO: if expression not float, convert initialisation parameter expression
					InitVal = (*this)(*x.parameters.begin());
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						return ErrorV("Invalid 1. initialisation parameter'"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"'");
					}
					else if(!InitVal->getType()->isFloatingPointTy())
					{
						// FIXME: Type upcast possible?
						//L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"UItoFP");
						
						std::string type_str;
						llvm::raw_string_ostream rso(type_str);
						InitVal->getType()->print(rso);

						std::stringstream sstr;
						sstr << *x.parameters.begin();

						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";

						return ErrorV("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' is of type '"+ type_str +"'.");
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x.parameters.size();
					return ErrorV("Variable of type '"+x.decl.type.type_identifier.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}

			// allocate in function head
			llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, getTypeByName(x.decl.type.type_identifier.name), bHasName ? x.decl.name.get().name : "");
			builder.CreateStore(InitVal, Alloca);

			// remember this variable in symbol table
			if(bHasName)
			{
				vLocalSymbolTable.push_back(code_generator::VarData(x.decl.name.get().name, Alloca, x.decl.type.mutableQualifier));
			}

			return Alloca;
		}
	}
}