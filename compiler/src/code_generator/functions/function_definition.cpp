#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/Verifier.h"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function_definition const & x)
		{
			LOG_SCOPE_DEBUG;

			vLocalSymbolTable.clear();

			// purity test
			/*if (x.decl.pureQualifier && x.body)
			{
				return static_cast<llvm::Function*>(ErrorV("Pure function '"+x.decl.idf.name+"' contains unpure expressions!"));
			}*/
  
			// build function declaration
			ast::function_declaration decl;
			decl.idf = x.idf;
			decl.unpureQualifier = x.unpureQualifier;
			for(ast::variable_declaration const & x : x.parameter_declarations)
			{
				decl.parameter_types.push_back(x.type);
			}
			for(ast::variable_definition const & x : x.return_value_definitions)
			{
				decl.return_types.push_back(x.decl.type);
			}
			llvm::Function *TheFunction = (*this)(decl);
			if (TheFunction == 0)
			{
				return static_cast<llvm::Function*>(ErrorV("Unable to build function declaration for function definition: '"+x.idf.name+"'!"));
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", TheFunction);
			builder.SetInsertPoint(BB);

			// add the parameters
			llvm::Function::arg_iterator AI = TheFunction->arg_begin();
			for (size_t Idx = 0, e = x.parameter_declarations.size(); Idx != e; ++Idx, ++AI)
			{
				// TODO: Think about order? l->r, r->l
				llvm::Value * V = (*this)(x.parameter_declarations[Idx]);
				if (!V)
				{
					const auto sVarName = x.parameter_declarations[Idx].name.is_initialized() ? " '"+x.parameter_declarations[Idx].name.get().name+"'" : "";
					return static_cast<llvm::Function*>(ErrorV("Unable to create parameter '"+ sVarName+"' from function '"+x.idf.name+"'"));
				}

				// create storage for the parameter
				builder.CreateStore(AI, V);
			}

			// add the return values
			std::vector<llvm::Value *> retValues;
			for (unsigned int Idx = 0; Idx != x.return_value_definitions.size(); ++Idx)
			{
				retValues.push_back((*this)(x.return_value_definitions[Idx]));
				if(!retValues.back())
				{
					const auto sVarName = x.return_value_definitions[Idx].decl.name.is_initialized() ? " '"+x.return_value_definitions[Idx].decl.name.get().name+"'" : "";
					return static_cast<llvm::Function*>(ErrorV("Unable to create return value '"+ sVarName+"' from function '"+x.idf.name+"'"));
				}
			}

			// add body
			(*this)(x.body);

			// return value(s)
			if(retValues.empty())
			{
				builder.CreateRetVoid();
			}
			else if(retValues.size() == 1)
			{
				llvm::Value *RetVal = builder.CreateLoad(retValues[0], "loadret");
				if(!RetVal)
				{
					return static_cast<llvm::Function*>(ErrorV("Unable to create load return from function '"+x.idf.name+"'"));
				}
				if(RetVal->getType() != TheFunction->getReturnType())
				{
					std::string type_str;
					llvm::raw_string_ostream rso(type_str);
					rso << "Trying to return '";
					RetVal->getType()->print(rso);
					rso << "' from a function '"+x.idf.name+" 'returning '";
					TheFunction->getReturnType()->print(rso);
					rso << "'.";
					return static_cast<llvm::Function*>(ErrorV("Return type mismatch! "+rso.str()));
				}
				/*if(!TheFunction->getReturnType()->isPointerTy())
				{
					if(retValues[0]->isDereferenceablePointer())
					{
						//llvm::GetElementPtrInst* gep = new llvm::GetElementPtrInst(retValues[0],);
						//const llvm::PointerType * ptr_type = dynamic_cast<const llvm::PointerType*>((retValues[0]->getType()));
						//ptr_type->
					}
				}*/
				builder.CreateRet(RetVal);
			}
			else
			{
				if(TheFunction->getReturnType()->isAggregateType())
				{
					// FIXME: handle multi-return value functions in result expressions.
					builder.CreateAggregateRet(retValues.data(), (unsigned int)x.return_value_definitions.size());
				}
				else
				{
					return static_cast<llvm::Function*>(ErrorV("Unable to return multiple return values from the non aggregate return type function '"+x.idf.name+"'!"));
				}
			}

			// Validate the generated code, checking for consistency.
			if(llvm::verifyFunction(*TheFunction, llvm::VerifierFailureAction::PrintMessageAction))
			{
				return static_cast<llvm::Function*>(InternalErrorV("verifyFunction failure '"+x.idf.name+"'!"));
			}

			return TheFunction;

			// FIXME on error
			/*
			{
				// Error reading body, remove function.
				TheFunction->eraseFromParent();
				throw std::runtime_error("Error generating return value!");
				*/
				/*
				This code does have a bug, though. Since the PrototypeAST::Codegen can return a previously defined forward declaration, our code can actually delete a forward declaration. There are a number of ways to fix this bug, see what you can come up with! Here is a testcase:

extern foo(a b);     # ok, defines foo.
def foo(a b) c;      # error, 'c' is invalid.
def bar() foo(1, 2); # error, unknown function "foo"

				*/
			//}
		}
	}
}