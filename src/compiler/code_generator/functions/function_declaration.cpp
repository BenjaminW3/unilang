#include <unilang/compiler/code_generator/functions/function_code_gen.hpp>

#include <unilang/compiler/code_generator/types.hpp>
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/namespace/namespace_code_gen.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>

#include <unilang/compiler/ast/function_ast.hpp>
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
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

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
		llvm::Function * function_code_generator::operator()(ast::function_declaration const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			std::string const sMangledName (m_namespaceCodeGenerator.getCurrentMangledNamespaceName()+x.build_mangled_name());

			// Create list of parameter types.
			std::vector<llvm::Type*> vpParameterTypes;
			for( auto const & varDecl : x._vParameterDeclarations)
			{
				ast::variable_type_declaration const & typeDecl(varDecl._type);
				llvm::Type * const pType (m_symbolCodeGenerator.getTypeByName(typeDecl._idfName.m_sName));
				if(!pType)
				{
					std::string sVariableName(varDecl._optionalIdentifier.is_initialized() ? varDecl._optionalIdentifier.get().m_sName : "");
					return m_codeGeneratorErrors.ErrorFunction("Function parameter type '"+typeDecl._idfName.m_sName+"' of variable '" + sVariableName + "' in function '"+sMangledName+"' is not valid.");
				}
				//FIXME: mutable state for function parameters is not stored along with the llvm::Type.
				vpParameterTypes.push_back(pType);
			}
			
			// Create the return types.
			llvm::Type* pReturnType (nullptr);
			if(x._vReturnDeclarations.size()==0)
			{
				pReturnType = llvm::Type::getVoidTy(m_llvmCodeGenerator.getContext());
			}
			else if(x._vReturnDeclarations.size()==1)
			{
				pReturnType = m_symbolCodeGenerator.getTypeByName((*x._vReturnDeclarations.begin())._type._idfName.m_sName);
			}
			else
			{
				// FIXME: implement multiple return values
				return m_codeGeneratorErrors.ErrorFunction("Multiple return values not implemented! Function '"+sMangledName+"' is not valid.");
			}
			if(!pReturnType)
			{
				return m_codeGeneratorErrors.ErrorFunction("Function return type '"+(*x._vReturnDeclarations.begin())._type._idfName.m_sName+"' for function '"+sMangledName+"' is not valid.");
			}
			
			// Create llvm function type.
			llvm::FunctionType * const FT (llvm::FunctionType::get(pReturnType,	vpParameterTypes, false));

			// Create llvm function.
			llvm::Function *F (llvm::Function::Create(FT, llvm::Function::ExternalLinkage, sMangledName, m_llvmCodeGenerator.getModule().get()));

			// If F conflicted, there was already something with exactly the same name. So LLVM creates a new unique name for the just declared function.
			if (F->getName() != sMangledName)
			{
				// Delete the one we just made and get the existing one.
				F->eraseFromParent();
				F = m_llvmCodeGenerator.getFunctionFromMangledName(sMangledName);

				// We can declare functions as often as needed even after its definition
				//// If F already has a body, reject this.
				//if (!F->empty())
				//{
				//	return ErrorFunction("Redefinition of function '"+sMangledName+"' !");
				//}

				// The following tests should never return an error because the mangled name should reflect the types
				size_t uiArg (0);
				for (llvm::Function::arg_iterator AI (F->arg_begin()); AI != F->arg_end();	++AI, ++uiArg)
				{
					llvm::Type const * const pDefType (m_symbolCodeGenerator.getTypeByName(x._vParameterDeclarations[uiArg]._type._idfName.m_sName));
					if(!pDefType)
					{
						std::stringstream sstr;
						sstr << "Definition of function " << sMangledName << "' uses undefined type '" << x._vParameterDeclarations[uiArg]._type._idfName.m_sName << "' as "<< uiArg+1;
						return m_codeGeneratorErrors.ErrorFunction(sstr.str()+" parameter type. ");
					}
					//FIXME: mutable not checked.
					if(AI->getType()!=pDefType)
					{
						std::stringstream sstr;
						sstr << "Definition of function " << sMangledName << "' differs from declaration in " << uiArg+1;
						return m_codeGeneratorErrors.ErrorFunction(sstr.str()+". parameter type. '"+getLLVMTypeName(AI->getType())+"' was declared but '"+getLLVMTypeName(pDefType)+ "' used in definition.");
					}
				}
			}

			return F;
		}
	}
}