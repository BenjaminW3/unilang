#include "code_generator.hpp"

#include "../log/log.hpp"

#include "llvm/Support/raw_ostream.h"
//#include "llvm/Module.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
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
		llvm::AllocaInst * expression_code_generator::CreateEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName)
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
			const std::string sTempName = "EntryBlockAlloca_" + VarName;
			return TmpB.CreateAlloca(pType, 0,  sTempName.c_str());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::variable_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// has name?
			const bool bHasName = x.name.is_initialized();

			// redeclaration?
			if(bHasName)
			{
				VarData const * const existantVar = getVarFromName(x.name.get().name);
				if(existantVar)
				{
					std::string type_str;
					llvm::raw_string_ostream rso(type_str);
					existantVar->getAllocaInst()->getType()->print(rso);
					return ErrorValue("Variable with the name '"+x.name.get().name+"' has already been declared with type '"+type_str+"'.");
				}
			}
			// TODO: really needed? We already can have typenames equal to variable names
			// shadowing keyword?
			if(bHasName && (x.name.get().name == "if" || x.name.get().name == "else" || x.name.get().name == "while" || x.name.get().name == "return"))
			{
				return ErrorValue("Variable with the name '"+x.name.get().name+"' is shadowing the keyword with same identifier.");
			}

			llvm::Function * TheFunction = builder.GetInsertBlock()->getParent();
			if(!TheFunction)
			{
				return ErrorValue("Unable to get the allocation insert point function for variable '"+x.name.get().name+"'.");
			}

			// allocate in function head
			llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, getTypeByName(x.type.type_identifier.name), bHasName ? x.name.get().name : "");
			if(!Alloca)
			{
				return ErrorValue("Unable to allocate variable '"+x.name.get().name+"'.");
			}

			// remember this variable in symbol table
			if(bHasName)
			{
				vLocalSymbolTable.push_back(VarData(x.name.get().name, Alloca, x.type.mutableQualifier));
			}

			return Alloca;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::variable_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			
			const bool bHasName = x.decl.name.is_initialized();

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
			llvm::Value * InitVal = nullptr;
			
			// TODO: hard coded types
			if(x.decl.type.type_identifier.name == "int")
			{
				if(x.parameters.size()==0)
				{
					InitVal = (*this)(int(0)); // 64bit, initial 0, signed
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						return ErrorValue("InitVal for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' could not be generated.");
					}
				}
				else if(x.parameters.size()==1)
				{
					InitVal = (*this)(*x.parameters.begin());
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						// TODO: output real parameter number
						return ErrorValue("Invalid 1. initialisation parameter '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"'");
					}
					else if(!InitVal->getType()->isIntegerTy())
					{
						// convert if it is float
						if(InitVal->getType()->isFloatingPointTy())
						{	
							InitVal = builder.CreateFPToSI(InitVal, getTypeByName(x.decl.type.type_identifier.name), "FPtoSI");
						}
						// else error
						else
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							InitVal->getType()->print(rso);

							std::stringstream sstr;
							sstr << *x.parameters.begin();

							const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";

							return ErrorValue("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' is of type '"+ type_str +"'.");
						}
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x.parameters.size();
					return ErrorValue("Variable of type '"+x.decl.type.type_identifier.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}
			if(x.decl.type.type_identifier.name == "float")
			{
				if(x.parameters.size()==0)
				{
					// If not specified, use 0.0.
					InitVal = (*this)(long double(0.0));
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						return ErrorValue("InitVal for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' could not be generated.");
					}
				}
				else if(x.parameters.size()==1)
				{
					InitVal = (*this)(*x.parameters.begin());
					if (!InitVal)
					{
						std::stringstream sstr;
						sstr << *x.parameters.begin();
						const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";
						return ErrorValue("Invalid 1. initialisation parameter'"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"'");
					}
					else if(!InitVal->getType()->isFloatingPointTy())
					{
						// convert if it is int
						if(InitVal->getType()->isIntegerTy())
						{	
							InitVal = builder.CreateSIToFP(InitVal, getTypeByName(x.decl.type.type_identifier.name), "SItoFP");
						}
						// else error
						else
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							InitVal->getType()->print(rso);

							std::stringstream sstr;
							sstr << *x.parameters.begin();

							const auto sVarName = bHasName ? " '"+x.decl.name.get().name+"'" : "";

							return ErrorValue("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+x.decl.type.type_identifier.name+"' is of type '"+ type_str +"'.");
						}
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x.parameters.size();
					return ErrorValue("Variable of type '"+x.decl.type.type_identifier.name+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}

			// build declaration (allocation, etc.)
			llvm::Value * pDecl = (*this)(x.decl);
			if(!pDecl)
			{
				return ErrorValue("Unable to declare variable '"+x.decl.name.get().name+"' of type '"+x.decl.type.type_identifier.name+"'.");
			}

			builder.CreateStore(InitVal, pDecl);

			return pDecl;	// maybe InitVal would be more appropriate
		}
	}
}