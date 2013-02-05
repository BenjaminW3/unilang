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
#include "llvm/Transforms/IPO.h"		// createDeadArgEliminationPass, ...
#include "llvm/Transforms/Vectorize.h"	// createLoopVectorizePass
#include <llvm/Module.h>

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Host.h>			// getHostCPUName
#include <llvm/MC/SubtargetFeature.h>
#ifdef _DEBUG
#include <llvm/ADT/Statistic.h>			// EnableStatistics
#include <llvm/Support/raw_ostream.h>	// PrintStatistics
#endif

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
			llvm::Function* callee (getModule()->getFunction(name));
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

#ifdef _DEBUG
			llvm::EnableStatistics();
#endif

			std::string const sTripleStr = llvm::sys::getDefaultTargetTriple(); // "i686-unknown-win32"
			std::cout << "Using target triple: '" << sTripleStr << "'" << std::endl;

			// Or just call llvm::InitializeAllTargetInfos() and llvm::InitializeAllTargets() for all targets enabled by your LLVM build.
			//llvm::LLVMInitializeX86TargetInfo();
			//llvm::LLVMInitializeX86Target();

			llvm::InitializeAllTargetInfos();
			llvm::InitializeAllTargets();
			llvm::InitializeAllTargetMCs();

			std::string sErr;
			llvm::Target const * const target (llvm::TargetRegistry::lookupTarget(sTripleStr, sErr));
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
			std::string sFeatures (subtargetFeatures.getString());
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

			llvm::DataLayout const * const targetData (targetMachine->getDataLayout());

			// otimize functions
			// use some basic passes like : http://llvm.org/docs/tutorial/LangImpl4.html
			for ( auto functionListIterator(module->getFunctionList().begin()); functionListIterator != module->getFunctionList().end(); ++functionListIterator)
			{
				llvm::FunctionPassManager OurFPM(module.get());
				
				// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
				OurFPM.add(new llvm::DataLayout(*targetData));

				OurFPM.add(llvm::createVerifierPass());
				// Add TypeBasedAliasAnalysis before BasicAliasAnalysis so that BasicAliasAnalysis wins if they disagree. This is intended to help support "obvious" type-punning idioms. 
				// https://llvm.org/svn/llvm-project/llvm/trunk/lib/Transforms/IPO/PassManagerBuilder.cpp
				OurFPM.add(llvm::createTypeBasedAliasAnalysisPass());
				// Provide basic AliasAnalysis support for GVN.
				OurFPM.add(llvm::createBasicAliasAnalysisPass());
				// Do simple "peephole" optimizations and bit-twiddling optzns. Reassociate expressions.
				OurFPM.add(llvm::createReassociatePass());
				// deletes stores that are post-dominated by must-aliased stores and are not loaded used between the stores.
				OurFPM.add(llvm::createDeadStoreEliminationPass());
				// Promote allocas to registers.
				OurFPM.add(llvm::createPromoteMemoryToRegisterPass());
				// Break up alloca of aggregated type into multiple allocas. calls mem2reg internally
				//OurFPM.add(llvm::createScalarReplAggregatesPass());
				// 
				OurFPM.add(llvm::createConstantPropagationPass());
				// Eliminate Common SubExpressions.
				OurFPM.add(llvm::createGVNPass());
				// combines instructions. creates some dead code -> DCE Pass
				OurFPM.add(llvm::createInstructionCombiningPass());
				// removes dead code
				OurFPM.add(llvm::createDeadCodeEliminationPass());

				////// Loops order like in https://llvm.org/svn/llvm-project/llvm/trunk/lib/Transforms/IPO/PassManagerBuilder.cpp
				// rotate loops
				OurFPM.add(llvm::createLoopRotatePass());
				// inserts phi nodes at loop boundaries to simplify other loop optimizations
				OurFPM.add(llvm::createLCSSAPass());
				// simplifies instructions in a loop's body
				OurFPM.add(llvm::createLoopInstSimplifyPass());
				// loop invariant code motion and memory promotion pass
				OurFPM.add(llvm::createLICMPass());
				// use a single canonical induction variable per loop
				OurFPM.add(llvm::createIndVarSimplifyPass());
				// Recognize idioms like memset.
				OurFPM.add(llvm::createLoopIdiomPass());
				// Delete dead loops
				OurFPM.add(llvm::createLoopDeletionPass());
				// vectorizes loops
				OurFPM.add(llvm::createLoopVectorizePass());
				// unrolls loops
				OurFPM.add(llvm::createLoopUnrollPass());

				// Eliminate tail recursion
				OurFPM.add(llvm::createTailCallEliminationPass());
				// Simplify the control flow graph (deleting unreachable blocks, etc).
				OurFPM.add(llvm::createCFGSimplificationPass());
				// BlockPlacement
				OurFPM.add(llvm::createBlockPlacementPass());

				OurFPM.add(llvm::createEarlyCSEPass());

				OurFPM.add(llvm::createLowerExpectIntrinsicPass());

				OurFPM.doInitialization();

				llvm::Function * const fct(functionListIterator);
				OurFPM.run(*fct);
			}

			llvm::PassManager OurPM;
			// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
			OurPM.add(new llvm::DataLayout(*targetData));

			// module optimizations from "IPO.h"
			// It also discovers function arguments that are not captured by the function and marks them with the nocapture attribute.
			OurPM.add(llvm::createFunctionAttrsPass());
			// propagates constants from call sites into the bodies of functions.
			//OurPM.add(llvm::createIPConstantPropagationPass());
			OurPM.add(llvm::createIPSCCPPass());
			// createAlwaysInlinerPass only inlines functions marked with always_inline
			// removes arguments from functions which are not used by the body of the function.
			OurPM.add(llvm::createDeadArgEliminationPass());
			OurPM.add(llvm::createInstructionCombiningPass());// Clean up after IPCP & DAE https://llvm.org/svn/llvm-project/llvm/trunk/lib/Transforms/IPO/PassManagerBuilder.cpp
			OurPM.add(llvm::createCFGSimplificationPass());   // Clean up after IPCP & DAE
			// discovers functions that do not access memory, or only read memory, and gives them the readnone/readonly attribute.
			// promotes "by reference" arguments to be passed by value if the number of elements passed is small
			OurPM.add(llvm::createArgumentPromotionPass());
			// discovers identical functions and collapses them.
			OurPM.add(llvm::createMergeFunctionsPass());
			// heuristic to inline direct function calls to small functions.
			OurPM.add(llvm::createFunctionInliningPass());
			// removes duplicated globals
			OurPM.add(llvm::createConstantMergePass());
			// optimizes non-address taken internal globals
			OurPM.add(llvm::createGlobalOptimizerPass());
			// eliminate unreachable internal globals (functions or global variables)
			OurPM.add(llvm::createGlobalDCEPass());

			OurPM.run(*module.get());
			
#ifdef _DEBUG
			std::string type_str;
			llvm::raw_string_ostream rso(type_str);
			llvm::PrintStatistics(rso);
			std::cout << "LLVM statistics:" << std::endl;
			std::cout << rso.str() << std::endl;
#endif
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