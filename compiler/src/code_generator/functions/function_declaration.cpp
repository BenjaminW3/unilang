#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"

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
		llvm::Function * code_generator::operator()(ast::function_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// create list of parameters types
			std::vector<llvm::Type*> vpParameterTypes;
			for( ast::type_declaration const & typeDecl : x.parameter_types)
			{
				//FIXME: mutable not saved.
				vpParameterTypes.push_back(getTypeByName(typeDecl.type_identifier.name));
			}
			
			// create return type
			llvm::Type* pReturnType = nullptr;
			if(x.return_types.size()==0)
			{
				pReturnType = llvm::Type::getVoidTy(context);
			}
			else
			{
				// FIXME: only one return value/type
				pReturnType = getTypeByName((*x.return_types.begin()).type_identifier.name);
			}

			llvm::FunctionType *FT = llvm::FunctionType::get(pReturnType,	vpParameterTypes, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x.idf.name, module.get());

			// If F conflicted, there was already something named 'Name'.  If it has a body, don't allow redefinition or reextern.
			if (F->getName() != x.idf.name)
			{
				// Delete the one we just made and get the existing one.
				F->eraseFromParent();
				F = getFunctionFromName(x.idf.name);

				// If F already has a body, reject this.
				if (!F->empty())
				{
					return ErrorFunction("Redefinition of function '"+x.idf.name+"' !");
				}
	
				// If F took a different number of args, reject it.
				if (F->arg_size() != x.parameter_types.size())
				{
					return ErrorFunction("Definition of function "+x.idf.name+"' with different number of parameters then previous declaration!");	// TODO: Log numbers
				}

				unsigned int uiArg=0;
				for (llvm::Function::arg_iterator AI = F->arg_begin(); AI != F->arg_end();	++AI, ++uiArg)
				{
					llvm::Type * pDefType = getTypeByName(x.parameter_types[uiArg].type_identifier.name);
					//FIXME: mutable not checked.
					if(AI->getType()!=pDefType)
					{
						std::string type_str;
						llvm::raw_string_ostream rso(type_str);
						rso << "Definition of function "+x.idf.name+"' differs from declaration in " << uiArg+1 << ". parameter type. '";
						AI->getType()->print(rso);
						rso << "' was declared but '";
						pDefType->print(rso);
						rso << "' used in definition.";
						return ErrorFunction(rso.str());
					}
				}
			}

			// FIXME: declared types do not have names! But function_definition has names
			/*// Set names for all parameters.
			unsigned int Idx = 0;
			for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != x.parameter_types.size();	++AI, ++Idx) 
			{
				if(x.parameter_types[Idx].identifier.is_initialized())
				{
					// FIXME check for conflicting parameter names
					AI->setName(x.parameter_types[Idx].identifier.name.get().name);
				}
			}*/

			return F;
		}
	}
}