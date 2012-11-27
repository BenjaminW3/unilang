#pragma once

#include "../errors.hpp"

#include <boost/noncopyable.hpp>

#include <memory>

// forward declarations
namespace llvm
{
	class Module;
	class Value;
	class LLVMContext;

	class ConstantFolder;
	template <bool preserveNames>
	class IRBuilderDefaultInserter;
	template<bool preserveNames, typename T, typename Inserter>
	class IRBuilder;
}

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class llvm_code_generator : virtual public code_generator_errors,
									virtual boost::noncopyable
		{
			// This typedef must be held the same as llvm::IRBuilder<> so that this always reflects the default arguments.
			// We have to do this because we can not forward declare templates with default parameters without warnings/errors.
			typedef llvm::IRBuilder<true, llvm::ConstantFolder, llvm::IRBuilderDefaultInserter<true>> IRBuilderType;
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			llvm_code_generator();

			//-------------------------------------------------------------------------
			//! Optimizes the bytecode.
			//-------------------------------------------------------------------------
			void optimize() const;

			//-------------------------------------------------------------------------
			//! Throws an exception if there are errors in the module
			//-------------------------------------------------------------------------
			void verify_module() const;

			//-------------------------------------------------------------------------
			//! Prints out the bytecode.
			//-------------------------------------------------------------------------
			void print_bytecode() const;

			//-------------------------------------------------------------------------
			//! \return The LLVMContext used to build the module.
			//-------------------------------------------------------------------------
			llvm::LLVMContext & getContext() const;
			//-------------------------------------------------------------------------
			//! \return The IRBuilder used to build the module.
			//-------------------------------------------------------------------------
			std::shared_ptr<IRBuilderType> getBuilder();
			//-------------------------------------------------------------------------
			//! \return The generated module.
			//-------------------------------------------------------------------------
			std::shared_ptr<llvm::Module> getModule() const;

			//-----------------------------------------------------------------------------
			//! \return The Function with the given name.
			//-----------------------------------------------------------------------------
			llvm::Function * getFunctionFromName( std::string const & name );

		private:
			std::shared_ptr<IRBuilderType> builder;

			std::shared_ptr<llvm::Module> module;
		};
	}
}