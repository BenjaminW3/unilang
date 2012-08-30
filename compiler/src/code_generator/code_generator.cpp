#include "code_generator.hpp"

#pragma warning(push)

#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4244)		// 'argument' : conversion from 'uint64_t' to 'const unsigned int', possible loss of data

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"

#pragma warning(pop)

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
			// FIXME: acces to variables defined in previous methods
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
		code_generator::code_generator(ast::module const & AST)
			:m_bErrorOccured(false),
			context(llvm::getGlobalContext()),
			builder(context),
			module(new llvm::Module("unilang-JIT", context))
		{
			LOG_SCOPE_DEBUG;

			llvm::InitializeNativeTarget();
			llvm::llvm_start_multithreaded();
			
			std::cout << std::endl << "##########CodeGen###########" << std::endl;
			for(ast::meta_entity const & meta : AST.metaEntities)
			{
				meta.apply_visitor(*this);
			}

			if(m_bErrorOccured)
			{
				throw std::runtime_error("Error occured during compilation of module!");
			}
			std::cout << "############################" << std::endl << std::endl;
			
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