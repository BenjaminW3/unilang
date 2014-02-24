#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>

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

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
//#include <llvm/ExecutionEngine/ExecutionEngine.h>
//#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include "llvm/Transforms/IPO.h"		// createDeadArgEliminationPass, ...
#include "llvm/Transforms/Vectorize.h"	// createLoopVectorizePass

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Host.h>			// getHostCPUName
#include <llvm/MC/SubtargetFeature.h>
#ifdef _DEBUG
#include <llvm/ADT/Statistic.h>			// EnableStatistics
#endif
#include <llvm/Support/raw_ostream.h>	// verifyModule

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/namespace/namespace_code_gen.hpp>

#include <unilang/compiler/log/log.hpp>

#include <iostream>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm_code_generator::llvm_code_generator(
			code_generator_errors & codeGeneratorErrors,
			namespace_code_generator & namespaceCodeGenerator) :
			builder(std::make_shared<IRBuilderType>(getContext())),
			llvmModule(std::make_shared<llvm::Module>("unilang-llvm", getContext())),
			m_codeGeneratorErrors(codeGeneratorErrors),
			m_namespaceCodeGenerator(namespaceCodeGenerator)
		{
			LOG_SCOPE_DEBUG;

			//llvm::InitializeNativeTarget();
			//llvm::llvm_start_multithreaded();	// only needed if concurrent threads are used
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::LLVMContext & llvm_code_generator::getContext() const
		{
			return llvm::getGlobalContext(); 
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::IRBuilder<>> llvm_code_generator::getBuilder()
		{
			return builder;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> llvm_code_generator::getModule() const
		{
			return llvmModule;
		}
		//-------------------------------------------------------------------------
		// http://llvm.1065342.n5.nabble.com/how-to-get-TargetData-td21590.html
		//-------------------------------------------------------------------------
		void llvm_code_generator::optimizeModule() const
		{
			LOG_SCOPE_DEBUG;
#ifdef _DEBUG
			// Enable build statistics in debug builds.
			llvm::EnableStatistics();
#endif
#ifdef USE_MCJIT
			std::string const sTripleStr (llvm::sys::getDefaultTargetTriple() + "-elf"); // Something like "i686-unknown-win32" FIXME:  + "-elf" is needed for MCJIT
#endif
			std::string const sTripleStr(llvm::sys::getDefaultTargetTriple()); // Something like "i686-unknown-win32"
			std::cout << "Using target triple: '" << sTripleStr << "'" << std::endl;

			llvm::InitializeAllTargetInfos();
			llvm::InitializeAllTargets();
			llvm::InitializeAllTargetMCs();

			std::string sErr;
			llvm::Target const * const target(llvm::TargetRegistry::lookupTarget(sTripleStr, sErr));
			if(!sErr.empty())
			{
				throw std::runtime_error(sErr);
			}

			llvm::StringMap<bool> featureMap;
			llvm::SubtargetFeatures subtargetFeatures;

			subtargetFeatures.getDefaultSubtargetFeatures(llvm::Triple(sTripleStr));
			if(llvm::sys::getHostCPUFeatures(featureMap))
			{
				for(llvm::StringMapIterator<bool> it = featureMap.begin(); it != featureMap.end();)
				{
					subtargetFeatures.AddFeature(it->getKey(), it->getValue());
				}
			}
			std::string sFeatures(subtargetFeatures.getString());
			std::cout << "Using features: '" << sFeatures << "'" << std::endl;

			std::cout << "Using HostCPUName: '" << llvm::sys::getHostCPUName().str() << "'" << std::endl;

			// Create TargetMachine
			llvm::TargetOptions targetOptions;
			// for debugging symbols targetOptions.NoFramePointerElim = true;

			std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(
				sTripleStr,
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

			llvm::DataLayout const * const targetData(targetMachine->getDataLayout());

			// otimize functions
			// use some basic passes like : http://llvm.org/docs/tutorial/LangImpl4.html
			for(auto functionListIterator(llvmModule->getFunctionList().begin()); functionListIterator != llvmModule->getFunctionList().end(); ++functionListIterator)
			{
				llvm::FunctionPassManager OurFPM(llvmModule.get());

				// Set up the optimizer pipeline.  Start with registering info about how the target lays out data structures.
				OurFPM.add(new llvm::DataLayout(*targetData));

				OurFPM.add(llvm::createVerifierPass());
				// Add TypeBasedAliasAnalysis before BasicAliasAnalysis so that BasicAliasAnalysis wins if they disagree. This is intended to help support "obvious" type-punning idioms. 
				// https://llvm.org/svn/llvm-project/llvm/trunk/lib/Transforms/IPO/PassManagerBuilder.cpp
				OurFPM.add(llvm::createTypeBasedAliasAnalysisPass());
				// Provide basic AliasAnalysis support for GVN.
				OurFPM.add(llvm::createBasicAliasAnalysisPass());
				// Optimize loops first.
				OurFPM.add(llvm::createLoopStrengthReducePass());
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
				//OurFPM.add(llvm::createBlockPlacementPass());	// deprecated???

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

			OurPM.run(*llvmModule.get());

#ifdef _DEBUG
			std::string type_str;
			llvm::raw_string_ostream rso(type_str);
			llvm::PrintStatistics(rso);
			std::cout << "LLVM statistics:" << std::endl;
			std::cout << rso.str() << std::endl;
#endif
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void llvm_code_generator::verifyModule() const
		{
			std::string sError;
			llvm::raw_string_ostream rso(sError);
			if(llvm::verifyModule(*llvmModule.get(), &rso))
			{
				throw std::runtime_error("verifyModule returned following errors : '" + rso.str() + "'.");
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void llvm_code_generator::printModuleBytecode() const
		{
			LOG_SCOPE_DEBUG;

			llvmModule->dump();
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Function * llvm_code_generator::getFunctionFromNameInNamespaceHierarchy( std::string const & sMangledName, std::vector<std::string> vsNamespaceHierarchy )
		{
			std::string const sMangledNamespaceName (m_namespaceCodeGenerator.buildMangledNamespaceName(vsNamespaceHierarchy));
			// If this is said to be searched for only in global namespace ...
			if(sMangledName.substr(0,2)=="::")
			{
				std::string const sMangledFunctionName (sMangledName.substr(2));
				return getFunctionFromMangledName(sMangledFunctionName);
			}
			// ... else reverse search through the namespaces.
			else
			{
				for(;;)
				{
					std::string const sMangledFunctionName (sMangledNamespaceName+sMangledName);
					// If we would call getFunctionFromMangledName directly we would set the error flag. Therefore special has method.
					if(hasFunctionFromMangledName(sMangledFunctionName))
					{
						// Can the availability change in betweeen hasFunctionFromMangledName and now?
						return getFunctionFromMangledName(sMangledFunctionName);
					}

					if(!vsNamespaceHierarchy.empty())
					{
						vsNamespaceHierarchy.pop_back();
					}
					else
					{
						break;
					}
				}
			}

			// if we would have using statements now they would be used to look into
			// we would have to look through all of them
			// using namespace Foo;
			// using namespace Goo;
			// cout << DoSomething(4, 3) << endl;
			// error C2668: 'DoSomething' : ambiguous call to overloaded function

			return m_codeGeneratorErrors.ErrorFunction("Function '" + sMangledName + "' has not been decalred.");
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool llvm_code_generator::hasFunctionFromMangledName( std::string const & sMangledName ) const
		{
			llvm::Function * pFunction (getModule()->getFunction(sMangledName));
			return (pFunction!=nullptr);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Function * llvm_code_generator::getFunctionFromMangledName( std::string const & sMangledName )
		{
			llvm::Function * pFunction (getModule()->getFunction(sMangledName));
			if (!pFunction)
			{
				return m_codeGeneratorErrors.ErrorFunction("Function '" + sMangledName + "' has not been decalred.");
			}

			return pFunction;
		}
	}
}