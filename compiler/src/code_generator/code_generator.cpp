#include "code_generator.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include "llvm/Support/TargetSelect.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Module.h"

#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Host.h"		//getHostCPUName
#include "llvm/MC/SubtargetFeature.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> code_generator::getModule() const
		{
			return module;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::LLVMContext& code_generator::getContext() const
		{
			return llvm::getGlobalContext(); 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::IRBuilder<>& code_generator::getBuilder()
		{
			return builder;
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value *code_generator::ErrorV(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("ERROR: "+Str); 
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value *code_generator::FatalErrorV(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("FATAL ERROR: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value *code_generator::InternalErrorV(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("INTERNAL ERROR: "+Str); 
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Type *code_generator::ErrorType(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("ERROR TYPE: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function *code_generator::ErrorFunction(std::string Str)
		{
			m_bErrorOccured = true;
			LOG("ERROR FUNCTION: "+Str);
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
				return llvm::Type::getIntNTy(context,uiIntSize);
			}
			else
			{
				return ErrorType("Use of unknown Type: '"+sTypeName+"'.");
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::VarData * code_generator::getVarFromName( std::string const & name )
		{
			// local search first!
			const auto itlocal = std::find_if(vLocalSymbolTable.begin(), vLocalSymbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(itlocal!=vLocalSymbolTable.end())
			{
				return &(*itlocal);
			}

			// then global search
			const auto itglobal = std::find_if(vGlobalSymbolTable.begin(), vGlobalSymbolTable.end(), 
				[&name](VarData const & var){ return (var.getIdentifier() == name); }
				);
			if(itglobal!=vGlobalSymbolTable.end())
			{
				return &(*itglobal);
			}
			return nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::getFunctionFromName( std::string const & name )
		{
			llvm::Function* callee = module->getFunction(name);
            if (!callee)
            {
				return ErrorFunction("Use of unknown function '"+name+"'.");
			}

			return callee;
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
		code_generator::code_generator(ast::module const & AST)
			:m_bErrorOccured(false),
			context(llvm::getGlobalContext()),
			builder(context),
			module(new llvm::Module("unilang-JIT", context))
		{
			LOG_SCOPE_DEBUG;

			//llvm::InitializeNativeTarget();
			//llvm::llvm_start_multithreaded();	// only needed if concurrent threads are used
			
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
		}
		//-----------------------------------------------------------------------------
		// http://llvm.1065342.n5.nabble.com/how-to-get-TargetData-td21590.html
		//-----------------------------------------------------------------------------
		void code_generator::optimize() const
		{
			LOG_SCOPE_DEBUG;

			std::string const sTripleStr = "i686-unknown-win32";//llvm::sys::getDefaultTargetTriple();
			std::cout << "Using target triple: '" << sTripleStr << "'" << std::endl;

			// Or just call llvm::InitializeAllTargetInfos() and llvm::InitializeAllTargets() for all targets enabled by your LLVM build.
			//llvm::LLVMInitializeX86TargetInfo();
			//llvm::LLVMInitializeX86Target();

			llvm::InitializeAllTargetInfos();
			llvm::InitializeAllTargets();
			llvm::InitializeAllTargetMCs();

			std::string sErr;
			llvm::Target const * const target = llvm::TargetRegistry::lookupTarget(sTripleStr, sErr);
			if(!sErr.empty())
			{
				throw std::runtime_error(sErr);
			}

			llvm::StringMap<bool> featureMap;
            llvm::SubtargetFeatures subtargetFeatures;

            subtargetFeatures.getDefaultSubtargetFeatures(llvm::Triple(sTripleStr));
            if (llvm::sys::getHostCPUFeatures(featureMap)) 
			{
				for (llvm::StringMapIterator<bool> it = featureMap.begin(); it != featureMap.end();) 
				{
					subtargetFeatures.AddFeature(it->getKey(), it->getValue());
				}
            }
            std::string sFeatures = subtargetFeatures.getString();
			std::cout << "Using features: '" << sFeatures << "'" << std::endl;
			
			std::cout << "Using HostCPUName: '" << llvm::sys::getHostCPUName() << "'" << std::endl;

			// Create TargetMachine
			llvm::TargetOptions targetOptions;
			// for debugging symbols targetOptions.NoFramePointerElim = true;


			std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(	sTripleStr,
																							llvm::sys::getHostCPUName(),
																							sFeatures,
																							targetOptions,
																							llvm::Reloc::Default,
																							llvm::CodeModel::Default,
																							llvm::CodeGenOpt::None));
			if(!targetMachine)
			{
				throw std::runtime_error("target->createTargetMachine failed!");
			}

			llvm::TargetData const * const targetData = targetMachine->getTargetData();

			llvm::PassManager OurPM;
			// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
			OurPM.add(new llvm::TargetData(*targetData));
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
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		void code_generator::print_bytecode() const
		{
			LOG_SCOPE_DEBUG;

			module->dump();
		}
	}
}