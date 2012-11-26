#include "../code_generator.hpp"

#include "../../log/log.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#endif

#include "llvm/Function.h"

#if defined(_MSC_VER)
#pragma warning(push)
#endif

#include "../types.hpp"

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

			// create list of parameter types
			std::vector<llvm::Type*> vpParameterTypes;
			for( ast::type_declaration const & typeDecl : x._parameter_types)
			{
				//FIXME: mutable not saved.
				vpParameterTypes.push_back(getTypeByName(typeDecl._identifier._name));
			}
			
			// create return type
			llvm::Type* pReturnType = nullptr;
			if(x._return_types.size()==0)
			{
				pReturnType = llvm::Type::getVoidTy(context);
			}
			else
			{
				// FIXME: only one return value/type
				pReturnType = getTypeByName((*x._return_types.begin())._identifier._name);
			}
			
#ifdef IMPLEMENT_VAR_ARG
			llvm::FunctionType *FT = llvm::FunctionType::get(pReturnType,	vpParameterTypes, x._bIsVarArg);
#else
			llvm::FunctionType *FT = llvm::FunctionType::get(pReturnType,	vpParameterTypes, false);
#endif

			std::string const mangledName (x.build_mangled_name());
			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, mangledName/*x._identifier._name*/, module.get());

			// If F conflicted, there was already something named 'Name'. So LLVM creates a new unique name for the just declared function.
			if (F->getName() != mangledName/*x._identifier._name*/)
			{
				// Delete the one we just made and get the existing one.
				F->eraseFromParent();
				F = getFunctionFromName(mangledName/*x._identifier._name*/);

				// If F already has a body, reject this.
				if (!F->empty())
				{
					return ErrorFunction("Redeclaration/definition of function '"+mangledName/*x._identifier._name*/+"' !");
				}
	
				// If F took a different number of args, reject it.
				if (F->arg_size() != x._parameter_types.size())
				{
					return ErrorFunction("Definition of function "+mangledName/*x._identifier._name*/+"' with different number of parameters then previous declaration!");	// TODO: Log numbers
				}

				unsigned int uiArg=0;
				for (llvm::Function::arg_iterator AI = F->arg_begin(); AI != F->arg_end();	++AI, ++uiArg)
				{
					llvm::Type * pDefType = getTypeByName(x._parameter_types[uiArg]._identifier._name);
					//FIXME: mutable not checked.
					if(AI->getType()!=pDefType)
					{
						std::stringstream sstr;
						sstr << "Definition of function "+mangledName/*x._identifier._name*/+"' differs from declaration in " << uiArg+1;
						return ErrorFunction(sstr.str()+". parameter type. '"+getLLVMTypeName(AI->getType())+"' was declared but '"+getLLVMTypeName(pDefType)+ "' used in definition.");
					}
				}
			}

			return F;
		}
	}
}