#include "llvm_code_gen.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/LLVMContext.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
//#include <llvm/ExecutionEngine/ExecutionEngine.h>
//#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/DataLayout.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Module.h>

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Host.h>		//getHostCPUName
#include <llvm/MC/SubtargetFeature.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "../../log/log.hpp"

#include <iostream>

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::LLVMContext& llvm_code_generator::getContext() const
		{
			return llvm::getGlobalContext(); 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::IRBuilder<>> llvm_code_generator::getBuilder()
		{
			return builder;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> llvm_code_generator::getModule() const
		{
			return module;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * llvm_code_generator::getFunctionFromName( std::string const & name )
		{
			llvm::Function* callee = getModule()->getFunction(name);
			if (!callee)
			{
				return ErrorFunction("Function '"+name+"' has not been decalred.");
			}

			return callee;
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm_code_generator::llvm_code_generator()
			:builder(std::make_shared<IRBuilderType>(getContext())),
			module(new llvm::Module("unilang-JIT", getContext()))
		{
			LOG_SCOPE_DEBUG;

			//llvm::InitializeNativeTarget();
			//llvm::llvm_start_multithreaded();	// only needed if concurrent threads are used
		}
		//-----------------------------------------------------------------------------
		// http://llvm.1065342.n5.nabble.com/how-to-get-TargetData-td21590.html
		//-----------------------------------------------------------------------------
		void llvm_code_generator::optimize() const
		{
			LOG_SCOPE_DEBUG;

			std::string const sTripleStr = llvm::sys::getDefaultTargetTriple(); // "i686-unknown-win32"
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

			llvm::DataLayout const * const targetData = targetMachine->getDataLayout();

			llvm::PassManager OurPM;
			// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
			OurPM.add(new llvm::DataLayout(*targetData));
			//
			OurPM.add(llvm::createVerifierPass());

			OurPM.add(llvm::createTypeBasedAliasAnalysisPass());
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
		void llvm_code_generator::verify_module() const
		{
			std::string ErrStr;
			if(llvm::verifyModule(*module.get(), llvm::VerifierFailureAction::ReturnStatusAction, &ErrStr))
			{
				throw std::runtime_error("verifyModule failure! "+ErrStr);
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		void llvm_code_generator::print_bytecode() const
		{
			LOG_SCOPE_DEBUG;

			module->dump();
		}
	}
}