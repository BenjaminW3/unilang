#include "code_generator.hpp"

#include <boost/foreach.hpp>

#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"

#include "../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::VarData * code_generator::getVarFromName( std::string const & name )
		{
			auto it = std::find_if(symbolTable.begin(), symbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(it!=symbolTable.end())
			{
				return &(*it);
			}
			return nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * code_generator::CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &VarName)	// FIXME pass in type
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
			return TmpB.CreateAlloca(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 0,  VarName.c_str());	// TODO: getGlobalContext()?
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(bool const & x)
		{
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
			//return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(unsigned int const & x)
		{
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
			//return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(32), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(int const & x)
		{
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
			//return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(32), uint64_t(x), true));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::identifier const & x)
		{
			LOG_SCOPE;
			LOG(x);

			code_generator::VarData * V = getVarFromName(x.name);
			if(!getVarFromName(x.name))
			{
				throw std::runtime_error("Unknown variable name!");
			}
			else
			{
				if(!V->getAllocaInst())
				{
					throw std::runtime_error("Variable is not allocated!");
				}
				else
				{
					return builder.CreateLoad(V->getAllocaInst(), x.name.c_str());
				}
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::unaryOp const & x)
		{
			LOG_SCOPE;
			LOG(x);

			llvm::Value *L = x.operand.apply_visitor(*this);
			switch (x.operator_)
			{
				case ast::op_negative: 
					{
						return builder.CreateNeg(L, "nottmp");
					}
				case ast::op_not: 
					{
						return builder.CreateNot(L, "nottmp");
					}
				case ast::op_positive: 
					{
						throw std::runtime_error("Unimplemented operation '+' !");
						//return builder.CreatePos(L, "nottmp");
					}
				default: 
					{
						throw std::runtime_error("Unknown operation!");
					}
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::binaryOp const & x)	// FIXME not FP Operations
		{
			LOG_SCOPE;
			LOG(x);

			llvm::Value *L = x.operand1.apply_visitor(*this);
			llvm::Value *R = x.operand2.apply_visitor(*this);
			if (L == 0 || R == 0)
			{
				throw std::runtime_error("Operation code could not be evaluated!");
			}
  
			switch (x.operator_)
			{
			case ast::op_plus:
				{
					L = builder.CreateFAdd(L, R, "addtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_minus:
				{
					L = builder.CreateFSub(L, R, "subtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_times:
				{
					L = builder.CreateFMul(L, R, "multmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_divide:
				{
					L = builder.CreateFDiv(L, R, "divtmp");		// FIXME CreateUDiv
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_equal:
				{
					L = builder.CreateFCmpUEQ(L, R, "equtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_not_equal:
				{
					L = builder.CreateFCmpUNE(L, R, "nequtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_less:
				{
					L = builder.CreateFCmpULT(L, R, "lttmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_less_equal:
				{
					L = builder.CreateFCmpULE(L, R, "ltequtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_greater:
				{
					L = builder.CreateFCmpUGT(L, R, "gttmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_greater_equal:
				{
					L = builder.CreateFCmpUGE(L, R, "gtequtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_and:
				{
					L = builder.CreateAnd(L, R, "andtmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			case ast::op_or:
				{
					L = builder.CreateOr(L, R, "ortmp");
					// Convert bool 0/1 to double 0.0 or 1.0
					return builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
				}
			default: 
				{
					throw std::runtime_error("Unknown operation!");
				}
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::function_call const & x)
		{
			LOG_SCOPE;
			LOG(x);

			// Look up the name in the global module table.
			llvm::Function *CalleeF = module->getFunction(x.idf.name);
			if (CalleeF == 0)
			{
				throw std::runtime_error("Unknown function referenced! '"+x.idf.name+"'");
			}
  
			// If argument mismatch error.
			if (CalleeF->arg_size() != x.arguments.size())
			{
				throw std::runtime_error("Incorrect # arguments passed to '"+x.idf.name+"' !");
			}

			std::vector<llvm::Value*> ArgsV;
			auto itArg = CalleeF->arg_begin();
			BOOST_FOREACH(ast::expression const & ex, x.arguments)
			{
				ArgsV.push_back((*this)(ex));
				if(!ArgsV.back())
				{
					throw std::runtime_error("Invalid argument returned!");
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
					throw std::runtime_error("Argument type mismatch! "+rso.str());
				}
				++itArg;
			}
  
			return builder.CreateCall(CalleeF, ArgsV, "calltmp");
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::expression const & x)
		{
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::assignment const & x)
		{
			LOG_SCOPE;
			LOG(x);

			// Look up the name.
			code_generator::VarData * V = getVarFromName(x.lhs.name);
			if(!V)
			{
				throw std::runtime_error("Unknown variable name!");
			}
			else
			{
				if(V->isConst())
				{
					throw std::runtime_error("Assignment to const variable is impossible! ");
				}
				else
				{
					if(!V->getAllocaInst())
					{
						throw std::runtime_error("Variable is not allocated!");
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *Val = (*this)(x.rhs);
						if(!Val)
						{
							throw std::runtime_error("Invalid right hand side of an assignment!");
						}
						// ->getPointerElementType()
						if(Val->getType()!=V->getAllocaInst()->getType()->getElementType())
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							rso << "Trying to assign a value of type '";
							Val->getType()->print(rso);
							rso << "' to a value of type '";
							V->getAllocaInst()->getType()->print(rso);
							rso << "'.";
							throw std::runtime_error("Assignment type mismatch! "+rso.str());
						}
						else
						{
							builder.CreateStore(Val, V->getAllocaInst());
							return Val;
						}
					}
				}
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::variable_definition const& x)
		{
			LOG_SCOPE;
			LOG(x);

			bool bHasName = x.name.is_initialized();

			if(bHasName && getVarFromName(x.name.get().name))
			{
				throw std::runtime_error("Invalid statement!");
			}

			llvm::Function *TheFunction = builder.GetInsertBlock()->getParent();

			// FIXME: initialisation

			// Emit the initializer before adding the variable to scope, this prevents
			// the initializer from referencing the variable itself, and permits stuff
			// like this:
			//  var a = 1 in
			//    var a = a in ...   # refers to outer 'a'.
			llvm::Value *InitVal = nullptr;
			/*if (x.rhs.is_initialized())
			{
				// FIXME: only one initialisation parameter
				InitVal = (*this)(x.rhs.get().[0]);
				if (!InitVal)
				{
					throw std::runtime_error("Invalid initialisation parameter!");
				}
			}
			else*/
			{
				// If not specified, use 0.0.
				InitVal = llvm::ConstantFP::get(context, llvm::APFloat(0.0));
			}/**/

			llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, bHasName ? x.name.get().name : "");
			builder.CreateStore(InitVal, Alloca);

			if(bHasName)
			{
				// Remember this variable.
				symbolTable.push_back(code_generator::VarData(x.name.get().name, Alloca, x.constQual.is_initialized() ? true : false));
			}

			return Alloca;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::statement const& x)
		{
			return boost::apply_visitor(*this, x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::statement_list const& x)
		{
			LOG_SCOPE;

			llvm::Value * ret = nullptr;
			BOOST_FOREACH(ast::statement const& s, x)
			{
				ret=(*this)(s);
				if (!ret)
				{
					throw std::runtime_error("Invalid statement!");
				}
			}
			return ret;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * code_generator::operator()(ast::while_statement const& x)
		{
			LOG_SCOPE;

			builder.
			BOOST_ASSERT(current != 0);
			std::size_t loop = current->size();         // mark our position
			if (!(*this)(x.condition))
				return false;
			current->op(op_jump_if, 0);                 // we shall fill this (0) in later
			std::size_t exit = current->size()-1;       // mark its position
			if (!(*this)(x.body))
				return false;
			current->op(op_jump,
				int(loop-1) - int(current->size()));    // loop back
			(*current)[exit] = current->size()-exit;    // now we know where to jump to (to exit the loop)
			return true;
		}*/
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * code_generator::operator()(ast::return_statement const& )
		{
			current->op(op_return);
			return true;
		}*/
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function_declaration const & x)	//FIXME return type not double
		{
			LOG_SCOPE;
			LOG(x);

			// Make the function type:  [double](doubles) etc.

			//llvm::ArrayType *AT = llvm::ArrayType::get(llvm::Type::getDoubleTy(context), x.return_values.size());

			std::vector<llvm::Type*> Doubles(	x.arguments.size(),	llvm::Type::getDoubleTy(context));
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context),	Doubles, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x.idf.name, module.get());

			// If F conflicted, there was already something named 'Name'.  If it has a body, don't allow redefinition or reextern.
			if (F->getName() != x.idf.name)
			{
				// Delete the one we just made and get the existing one.
				F->eraseFromParent();
				F = module->getFunction(x.idf.name);
				std::cout << "Function name conflict! "	<<	x.idf.name	<<	" already existed inside the module. Taking the existing one instead!";
				// If F already has a body, reject this.
				if (!F->empty()) 
				{
					throw std::runtime_error("Redefinition of function!");
				}
    
				// If F took a different number of args, reject.
				if (F->arg_size() != x.arguments.size()) 
				{
					throw std::runtime_error("Redefinition of function with different number of arguments!");	// Log numbers
				}

				// TODO: type comparison
			}

			// Set names for all arguments.
			unsigned int Idx = 0;

			for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != x.arguments.size();	++AI, ++Idx) 
			{
				if(x.arguments[Idx].name.is_initialized())
				{
					// FIXME check for conflicting argument names

					AI->setName(x.arguments[Idx].name.get().name);
				}
			}

			return F;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function const & x)
		{
			LOG_SCOPE;

			symbolTable.clear();
  
			llvm::Function *TheFunction = (*this)(x.decl);
			if (TheFunction == 0)
			{
				throw std::runtime_error("Unable to build function declaration for function definition: '"+x.decl.idf.name+"' !");
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", TheFunction);
			builder.SetInsertPoint(BB);

			// add the arguments
			llvm::Function::arg_iterator AI = TheFunction->arg_begin();
			for (unsigned Idx = 0, e = x.decl.arguments.size(); Idx != e; ++Idx, ++AI)
			{
				// TODO: Think about order? First all definitions, then parameter passing | one parameter definition and parameter passing a time
				llvm::Value * V = (*this)(x.decl.arguments[Idx]);
				if (!V)
				{
					throw std::runtime_error("Unable to create argument!");
				}

				// Store the initial value into the var.
				builder.CreateStore(AI, V);
			}

			// add the return values
			std::vector<llvm::Value *> retValues;
			for (unsigned int Idx = 0; Idx != x.decl.return_values.size(); ++Idx)
			{
				retValues.push_back((*this)(x.decl.return_values[Idx]));
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
				if(RetVal->getType() != TheFunction->getReturnType())
				{
					std::string type_str;
					llvm::raw_string_ostream rso(type_str);
					rso << "Trying to return '";
					RetVal->getType()->print(rso);
					rso << "' from a function returning '";
					TheFunction->getReturnType()->print(rso);
					rso << "'.";
					throw std::runtime_error("Return type mismatch! "+rso.str());
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
					builder.CreateAggregateRet(retValues.data(), (unsigned int)x.decl.return_values.size());
				}
				else
				{
					throw std::runtime_error("Unable to return multiple return values from a non aggregate return type function!");
				}
			}

			// Validate the generated code, checking for consistency.
			if(llvm::verifyFunction(*TheFunction, llvm::VerifierFailureAction::PrintMessageAction))
			{
				throw std::runtime_error("verifyFunction failure");
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
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::code_generator(ast::module const & AST)
			:context(llvm::getGlobalContext()),
			builder(context),
			module(new llvm::Module("unilang-JIT", context))
		{
			LOG_SCOPE;

			llvm::InitializeNativeTarget();
			llvm::llvm_start_multithreaded();

			BOOST_FOREACH(ast::meta_entity const & meta, AST.metaEntities)
			{
				meta.apply_visitor(*this);
			}
			
			std::string ErrStr;
			if(llvm::verifyModule(*module.get(), llvm::VerifierFailureAction::ReturnStatusAction, &ErrStr))
			{
				throw std::runtime_error(ErrStr);
			}
			std::cout << std::endl << "########unoptimized#########" << std::endl;
			module->dump();
			std::cout << "############################" << std::endl << std::endl;

			ErrStr = "";
			std::unique_ptr<llvm::ExecutionEngine> ee(llvm::EngineBuilder(module.get()).setErrorStr(&ErrStr).create());
			if(!ee)
			{
				throw std::runtime_error(ErrStr);
			}

			llvm::PassManager OurPM;
			// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
			OurPM.add(new llvm::TargetData(*ee->getTargetData()));
			//
			OurPM.add(llvm::createVerifierPass());
			// Provide basic AliasAnalysis support for GVN.
			OurPM.add(llvm::createBasicAliasAnalysisPass());
			// Promote allocas to registers.
			OurPM.add(llvm::createPromoteMemoryToRegisterPass());
			// 
			OurPM.add(llvm::createConstantPropagationPass());
			// Do simple "peephole" optimizations and bit-twiddling optzns.
			OurPM.add(llvm::createInstructionCombiningPass());
			// Dead code Elimination
            OurPM.add(llvm::createDeadCodeEliminationPass());
			// TailDuplication
			//OurPM.add(llvm::createTailDuplicationPass());
            // BlockPlacement
            OurPM.add(llvm::createBlockPlacementPass());
			// Reassociate expressions.
			OurPM.add(llvm::createReassociatePass());
			// Eliminate Common SubExpressions.
			OurPM.add(llvm::createGVNPass());
			// Simplify the control flow graph (deleting unreachable blocks, etc).
			OurPM.add(llvm::createCFGSimplificationPass());

			OurPM.run(*module.get());

			std::cout << std::endl << "#########optimized##########" << std::endl;
			module->dump();
			std::cout << "############################" << std::endl << std::endl;

			llvm::Function* func = ee->FindFunctionNamed("entrypoint");
			if(!func)
			{
				throw std::runtime_error("'entrypoint' not found!");
			}

			typedef double (*FuncPtr)(double);
			FuncPtr fptr = reinterpret_cast<FuncPtr>(ee->getPointerToFunction(func));

			std::cout << fptr(0) << std::endl;

			//ee->freeMachineCodeForFunction(vm_get_current_closure);
            ee->runStaticConstructorsDestructors(true);
            if(!ee->removeModule(module.get()))
			{
				throw std::runtime_error("Unable to remove module from ExecutionEngine!");
            }
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		void code_generator::print_assembler() const
		{
			LOG_SCOPE;

			module->dump();
		}
	}
}