#include "code_generator.hpp"

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
		llvm::Value *code_generator::ErrorV(std::string Str) 
		{
			m_bErrorOccured = true;
			LOG("ERROR: "+Str); 
			return nullptr; 
		}
		llvm::Value *code_generator::FatalErrorV(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("FATAL ERROR: "+Str);
			return nullptr; 
		}
		llvm::Value *code_generator::InternalErrorV(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("INTERNAL ERROR: "+Str); 
			return nullptr; 
		}

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
		llvm::AllocaInst * code_generator::CreateEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName)
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
			return TmpB.CreateAlloca(pType, 0,  VarName.c_str());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(bool const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(unsigned int const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(64), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(int const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(64), uint64_t(x), true));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::identifier const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			code_generator::VarData * V = getVarFromName(x.name);
			if(!getVarFromName(x.name))
			{
				return ErrorV("Undeclared variable name: '"+x.name+"' !");
			}
			else
			{
				if(!V->getAllocaInst())
				{
					return InternalErrorV("Variable is not allocated: '"+x.name+"' !");
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
		void code_generator::addStringConverters()
		{
			/*std::vector<llvm::Type*> Doubles(	x.arguments.size(),	llvm::Type::getDoubleTy(context));
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::get(context),	Doubles, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x.idf.name, module.get());

			llvm::Constant *double_to_string_func = module->getOrInsertFunction("double_to_string",
												llvm::Type::getVoidTy(),
												llvm::PointerType::getUnqual(llvm::Type::Int8Ty),
												llvm::Type::Int32Ty,
												llvm::Type::Int32Ty,
												nullptr);

			 llvm::Value* v = llvm::ConstantArray::get(llvmContext, myString.c_str());*/
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::unaryOp const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x.operand_.apply_visitor(*this);
			if(!L)
			{
				std::stringstream sstr;
				sstr << x.operand_;
				return ErrorV("Invalid value returned from '"+sstr.str()+"'");
			}

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
						return L; // + does not change anything
					}
				case ast::op_stringify:
					{
						//throw std::runtime_error("Not implemented!");

						if(L->getType()->isDoubleTy())
						{
							llvm::Value *Ui = builder.CreateFPToUI(L, llvm::IntegerType::get(context, 8), "FpToUiTmp");
							if(!Ui)
							{
								return InternalErrorV("CreateFPToUI returned invalid value!");
							}
							return builder.CreateAdd(Ui, (*this)(unsigned int(3*16)), "AddTmp");
						}
						else
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							rso << "String conversion for type '";
							L->getType()->print(rso);
							rso << "' not implemented!";
							return FatalErrorV(rso.str());
						}
						/*return builder.CreateGlobalString(L, "stringifytmp");*/
					}
				default:
					{
						return FatalErrorV("Unknown operation!");
					}
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::expression const & x)	// FIXME not FP Operations
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x.first.apply_visitor(*this);
			if (L == 0)
			{
				return ErrorV("Left hand side of operation code could not be evaluated!");
			}

			for( ast::operation const & op : x.rest)
			{
				llvm::Value *R = op.operand_.apply_visitor(*this);
				if (R == 0)
				{
					R = ErrorV("Right hand side of operation code could not be evaluated!");
				}
  
				switch (op.operator_)
				{
				case ast::op_plus:
					{
						L = builder.CreateFAdd(L, R, "addtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_minus:
					{
						L = builder.CreateFSub(L, R, "subtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_times:
					{
						L = builder.CreateFMul(L, R, "multmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_divide:
					{
						L = builder.CreateFDiv(L, R, "divtmp");		// FIXME CreateUDiv
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_equal:
					{
						L = builder.CreateFCmpUEQ(L, R, "equtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_not_equal:
					{
						L = builder.CreateFCmpUNE(L, R, "nequtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_less:
					{
						L = builder.CreateFCmpULT(L, R, "lttmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_less_equal:
					{
						L = builder.CreateFCmpULE(L, R, "ltequtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_greater:
					{
						L = builder.CreateFCmpUGT(L, R, "gttmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_greater_equal:
					{
						L = builder.CreateFCmpUGE(L, R, "gtequtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_and:
					{
						L = builder.CreateAnd(L, R, "andtmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				case ast::op_or:
					{
						L = builder.CreateOr(L, R, "ortmp");
						// Convert bool 0/1 to double 0.0 or 1.0
						L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"booltmp");
					}
				default: 
					{
						L = FatalErrorV("Unknown operation!");
					}
				}
			}
			return L;
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
		llvm::Type *code_generator::ErrorType(std::string Str) 
		{
			m_bErrorOccured = true;
			LOG("ERROR: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Type* code_generator::getTypeByName(std::string sTypeName)
		{
			// FIXME: hard coded types
			if(sTypeName=="float")
			{
				return llvm::Type::getDoubleTy(context);
			}
			else if(sTypeName=="int")
			{
				return llvm::Type::getInt64Ty(context);
			}
			else
			{
				return ErrorType("Use of unknown Type: '"+sTypeName+"'.");
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function_declaration const & x)	//FIXME return type not double
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// create list of argument types
			std::vector<llvm::Type*> vpArgumentTypes;
			for( ast::type_declaration const & typeDecl : x.argument_types)
			{
				vpArgumentTypes.push_back(getTypeByName(typeDecl.type_name.name));
			}
			
			// create return type
			// FIXME: only one return value/type
			llvm::Type* pReturnType = getTypeByName((*x.return_types.begin()).type_name.name);

			llvm::FunctionType *FT = llvm::FunctionType::get(pReturnType,	vpArgumentTypes, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x.idf.name, module.get());

			// If F conflicted, there was already something named 'Name'.  If it has a body, don't allow redefinition or reextern.
			if (F->getName() != x.idf.name)
			{
				// Delete the one we just made and get the existing one.
				F->eraseFromParent();
				F = module->getFunction(x.idf.name);
				//std::cout << "Function name conflict! "	<<	x.idf.name	<<	" already existed inside the module. Taking the existing one instead!" << std::endl;
				// If F already has a body, reject this.
				if (!F->empty())
				{
					return static_cast<llvm::Function*>(ErrorV("Redefinition of function '"+x.idf.name+"' !"));
				}
    
				// If F took a different number of args, reject it.
				if (F->arg_size() != x.argument_types.size())
				{
					return static_cast<llvm::Function*>(ErrorV("Definition of function "+x.idf.name+"' with different number of arguments then previous declaration!"));	// TODO: Log numbers
				}

				// TODO: type comparison ?
				/*for (llvm::Function::arg_iterator AI = F->arg_begin(); AI != F->arg_end();	++AI)
				{
					AI->getType()
				}*/
			}

			// Set names for all arguments.
			/*unsigned int Idx = 0;

			for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != x.argument_types.size();	++AI, ++Idx) 
			{
				if(x.arguments[Idx].name.is_initialized())
				{
					// FIXME check for conflicting argument names

					AI->setName(x.arguments[Idx].name.get().name);
				}
			}*/

			return F;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function_definition const & x)
		{
			LOG_SCOPE_DEBUG;

			symbolTable.clear();

			// purity test
			/*if (x.decl.pureQualifier && x.body)
			{
				return static_cast<llvm::Function*>(ErrorV("Pure function '"+x.decl.idf.name+"' contains unpure expressions!"));
			}*/
  
			// build function declaration
			ast::function_declaration decl;
			decl.idf = x.idf;
			decl.pureQualifier = x.pureQualifier;
			for(ast::variable_definition const & x : x.argument_definitions)
			{
				decl.argument_types.push_back(x.type);
			}
			for(ast::variable_definition const & x : x.return_value_definitions)
			{
				decl.return_types.push_back(x.type);
			}
			llvm::Function *TheFunction = (*this)(decl);
			if (TheFunction == 0)
			{
				return static_cast<llvm::Function*>(ErrorV("Unable to build function declaration for function definition: '"+x.idf.name+"'!"));
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", TheFunction);
			builder.SetInsertPoint(BB);

			// add the arguments
			llvm::Function::arg_iterator AI = TheFunction->arg_begin();
			for (unsigned Idx = 0, e = x.argument_definitions.size(); Idx != e; ++Idx, ++AI)
			{
				// TODO: Think about order? First all definitions, then parameter passing | one parameter definition and parameter passing a time
				llvm::Value * V = (*this)(x.argument_definitions[Idx]);
				if (!V)
				{
					return static_cast<llvm::Function*>(ErrorV("Unable to create argument!"));
				}

				// Store the initial value into the var.
				builder.CreateStore(AI, V);
			}

			// add the return values
			std::vector<llvm::Value *> retValues;
			for (unsigned int Idx = 0; Idx != x.return_value_definitions.size(); ++Idx)
			{
				retValues.push_back((*this)(x.return_value_definitions[Idx]));
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
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::code_generator(ast::module const & AST)
			:m_bErrorOccured(false),
			context(llvm::getGlobalContext()),
			builder(context),
			module(new llvm::Module("unilang-JIT", context))
		{
			LOG_SCOPE_DEBUG;

			llvm::InitializeNativeTarget();
			llvm::llvm_start_multithreaded();

			for(ast::meta_entity const & meta : AST.metaEntities)
			{
				meta.apply_visitor(*this);
			}

			if(m_bErrorOccured)
			{
				throw std::runtime_error("Error occured during compilation of module!");
			}
			
			std::string ErrStr;
			if(llvm::verifyModule(*module.get(), llvm::VerifierFailureAction::ReturnStatusAction, &ErrStr))
			{
				throw std::runtime_error("verifyModule failure! "+ErrStr);
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

			typedef unsigned int (*FuncPtr)();
			FuncPtr fptr = reinterpret_cast<FuncPtr>(ee->getPointerToFunction(func));

			std::cout << fptr() << std::endl;

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
			LOG_SCOPE_DEBUG;

			module->dump();
		}
	}
}