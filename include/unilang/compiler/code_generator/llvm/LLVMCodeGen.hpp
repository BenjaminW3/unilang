#pragma once

#include <vector>
#include <string>

// base classes
#include <boost/noncopyable.hpp>

// members
#include <memory>

// forward declarations
namespace llvm
{
	class Module;
	class Value;
	class LLVMContext;
	class Function;

	class ConstantFolder;
	template <bool preserveNames>
	class IRBuilderDefaultInserter;
	template<bool preserveNames, typename T, typename Inserter>
	class IRBuilder;
}
namespace unilang
{
	namespace code_generator
	{
		class CErrorCodeGen;
		class CModuleCodeGen;
	}
}

namespace unilang
{ 
	//-------------------------------------------------------------------------
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! This class holds a module and the llvm context and builder to operate on it.
		//#########################################################################
		class CLLVMCodeGen :
			virtual boost::noncopyable
		{
			// This typedef must be held the same as llvm::IRBuilder<> so that this always reflects the default arguments.
			// We have to do this because we can not forward declare templates with default parameters without warnings/errors.
			typedef llvm::IRBuilder<true, llvm::ConstantFolder, llvm::IRBuilderDefaultInserter<true>> IRBuilderType;
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			CLLVMCodeGen(
				CErrorCodeGen & codeGeneratorErrors,
				CModuleCodeGen & namespaceCodeGenerator );

			//-------------------------------------------------------------------------
			//! \return The LLVMContext used to build the m_ruleModule.
			//-------------------------------------------------------------------------
			llvm::LLVMContext & getContext() const;
			//-------------------------------------------------------------------------
			//! \return The IRBuilder used to build the m_ruleModule.
			//-------------------------------------------------------------------------
			std::shared_ptr<IRBuilderType> getBuilder();

			//-------------------------------------------------------------------------
			//! \return The generated m_ruleModule.
			//-------------------------------------------------------------------------
			std::shared_ptr<llvm::Module> getModule() const;
			//-------------------------------------------------------------------------
			//! Optimizes the bytecode.
			//-------------------------------------------------------------------------
			void optimizeModule() const;
			//-------------------------------------------------------------------------
			//! Throws an exception if there are errors in the m_ruleModule
			//-------------------------------------------------------------------------
			void verifyModule() const;
			//-------------------------------------------------------------------------
			//! Prints out the bytecode.
			//-------------------------------------------------------------------------
			void printModuleBytecode() const;
			
			//-------------------------------------------------------------------------
			//! Searches for the given function from inside the deepest namespace to the top of the herarchy.
			//! \param sMangledName The mangled function name to search for (with namespaces).
			//! \param vsNamespaceHierarchy The namespace hierarchy being searched through for the function.
			//! \return The Function with the given name. If none is found an error is signified.
			//-------------------------------------------------------------------------
			llvm::Function * getFunctionFromNameInNamespaceHierarchy( std::string const & sMangledName, std::vector<std::string> vsNamespaceHierarchy );
			//-------------------------------------------------------------------------
			//! \param sMangledName The mangled function name to search for (with namespaces).
			//! \return If the function exists. No error is signified if not found.
			//-------------------------------------------------------------------------
			bool hasFunctionFromMangledName( std::string const & sMangledName ) const;
			//-------------------------------------------------------------------------
			//! \param sMangledName The mangled function name to search for (with namespaces).
			//! \return The Function with the given name. If none is found an error is signified.
			//-------------------------------------------------------------------------
			llvm::Function * getFunctionFromMangledName( std::string const & sMangledName );

		private:
			std::shared_ptr<IRBuilderType> m_builder;

			std::shared_ptr<llvm::Module> m_llvmModule;

			CErrorCodeGen & m_codeGeneratorErrors;
			CModuleCodeGen & m_namespaceCodeGenerator;
		};
	}
}