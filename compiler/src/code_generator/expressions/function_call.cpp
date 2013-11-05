#include "exp_code_gen.hpp"

#include "../types.hpp"
#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"
#include "../namespace/namespace_code_gen.hpp"

#include "../../ast/function_ast.hpp"

#include "../../log/log.hpp"

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

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

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
		llvm::Value * expression_code_generator::operator()(ast::function_call const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			ast::function_declaration funcDecl;
			funcDecl._idfName = x._idfName.getMangledName();

			// argument types
			std::vector<llvm::Value*> ArgsV;
			for(ast::expression const & ex : x._vArgumentExpressions)
			{
				ArgsV.push_back((*this)(ex));
				if(!ArgsV.back())
				{
					std::stringstream sstr;
					sstr << ex;
					return m_codeGeneratorErrors.ErrorValue("Invalid argument returned from '" +sstr.str()+ "'.");
				}

				ast::type_declaration ty;
				ty._idfName = llvmTypeToUnilangTypeName(ArgsV.back()->getType());

				funcDecl._vParameterTypes.push_back(ty);
			}
			// Look up the mangled name in the global module table.
			std::string const sMangledName (funcDecl.build_mangled_name());
			llvm::Function * const pCalledFunction (m_llvmCodeGenerator.getFunctionFromNameInNamespaceHierarchy(sMangledName, m_namespaceCodeGenerator.getCurrentNamespaceHierarchy()));
			if (!pCalledFunction)
			{
				return m_codeGeneratorErrors.ErrorValue("Unknown function '"+sMangledName+"' referenced.");
			}
  
			// If argument number mismatch error.
			/*if (CalleeF->arg_size() != x._vArgumentExpressions.size())
			{
				std::stringstream sstr;
				sstr << "Expected " << CalleeF->arg_size() << " arguments but " << x._vArgumentExpressions.size() << " are given.";
				return ErrorValue("Incorrect number of arguments passed to '"+x._idfName._name+"' ! "+sstr.str());
			}

			// argument type match?
			auto itArg = CalleeF->arg_begin();
			for(ast::expression const & ex: x._vArgumentExpressions)
			{
				if(ArgsV.back()->getType()!=(*itArg).getType())
				{
					std::stringstream sstr;
					sstr << "Trying to call function '" << x._idfName._name << "' with argument number " << (*itArg).getArgNo();
					return ErrorValue("Argument type mismatch! "+sstr.str()+" with type '"+getLLVMTypeName(ArgsV.back()->getType())+"' but function expects a value of type '"+getLLVMTypeName((*itArg).getType())+"'.");
				}
				++itArg;
			}*/
  
			return m_llvmCodeGenerator.getBuilder()->CreateCall(pCalledFunction, ArgsV, "call");
		}
		/*
		if (printf == null) {
			// declare printf
			LLVMOpaqueType[] printfArgTypes = {
				I.LLVMPointerType(I.LLVMInt8Type(), 0)
			};
			LLVMOpaqueType printfFunctionType =
				I.LLVMFunctionType(I.LLVMInt32Type(),
								   printfArgTypes,
								   1,
								   1);
			printf = I.LLVMAddFunction(module, "printf", printfFunctionType);
		}
		MLType type = typer.visit(node);
		LLVMOpaqueValue arg =
			(LLVMOpaqueValue)((Visit)node.get(0)).visit(this);
		if (type.name().equals("i64")) {
			if (dn == null) {
				dn = I.LLVMBuildGlobalString(builder, "%d\n", "dn");
			}
			LLVMOpaqueValue[] gepArray = {
				I.LLVMConstInt(I.LLVMInt32Type(), 0, 0),
				I.LLVMConstInt(I.LLVMInt32Type(), 0, 0)
			};
			LLVMOpaqueValue dn2 =
				I.LLVMBuildInBoundsGEP(builder, dn, gepArray, 2, "");
			LLVMOpaqueValue[] callArgs = new LLVMOpaqueValue[2];        
			callArgs[0] = dn2;
			callArgs[1] = arg;
			I.LLVMBuildCall(builder, printf, callArgs, 2, "");
		} else if (type.name().equals("string")) {
			if (sn == null) {
				sn = I.LLVMBuildGlobalString(builder, "%s\n", "sn");
			}
			LLVMOpaqueValue[] gepArray = {
			   I.LLVMConstInt(I.LLVMInt32Type(), 0, 0),
			   I.LLVMConstInt(I.LLVMInt32Type(), 0, 0)
			};
			LLVMOpaqueValue sn2 =
				I.LLVMBuildInBoundsGEP(builder, sn, gepArray, 2, "");
			LLVMOpaqueValue[] callArgs = new LLVMOpaqueValue[2];
			callArgs[0] = sn2;
			callArgs[1] = arg;
			I.LLVMBuildCall(builder, printf, callArgs, 2, "");
		}
		return null;
		*/
	}
}