#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#endif

#include "llvm/Module.h"

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
		llvm::Value * code_generator::operator()(ast::function_call const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name in the global module table.
			llvm::Function *CalleeF = module->getFunction(x.idf.name);
			if (!CalleeF)
			{
				return ErrorV("Unknown function '"+x.idf.name+"' referenced.");
			}
  
			// If argument number mismatch error.
			if (CalleeF->arg_size() != x.arguments.size())
			{
				std::stringstream sstr;
				sstr << "Expected " << CalleeF->arg_size() << " arguments but " << x.arguments.size() << " are given.";
				return ErrorV("Incorrect number of arguments passed to '"+x.idf.name+"' ! "+sstr.str());
			}

			// argument type match?
			std::vector<llvm::Value*> ArgsV;
			auto itArg = CalleeF->arg_begin();
			for(ast::expression const & ex: x.arguments)
			{
				ArgsV.push_back((*this)(ex));
				if(!ArgsV.back())
				{
					std::stringstream sstr;
					sstr << ex;
					return ErrorV("Invalid argument returned from '" +sstr.str()+ "'.");
				}

				if(ArgsV.back()->getType()!=(*itArg).getType())
				{
					std::string type_str;
					llvm::raw_string_ostream rso(type_str);
					rso << "Trying to call function '" << x.idf.name << "' with argument number " << (*itArg).getArgNo() << " with type '";
					ArgsV.back()->getType()->print(rso);
					rso << "' but function expects a value of type '";
					(*itArg).getType()->print(rso);
					rso << "'.";
					return ErrorV("Argument type mismatch! "+rso.str());
				}
				++itArg;
			}
  
			return builder.CreateCall(CalleeF, ArgsV, "call");
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
        if (type.name().equals("int")) {
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