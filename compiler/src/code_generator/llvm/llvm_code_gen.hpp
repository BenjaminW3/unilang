#pragma once

#if defined(_MSC_VER)
#pragma warning(push)
//#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4244)		// conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// conversion from 'int' to 'unsigned int', signed/unsigned mismatch
#pragma warning(disable: 4512)		// assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#endif

#include "llvm/LLVMContext.h"
#include "llvm/IRBuilder.h"

// predefinitions
namespace llvm
{
	class Module;
	class Value;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "../errors.hpp"

#include <boost/noncopyable.hpp>

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
			//! \return The generated module.
			//-------------------------------------------------------------------------
			std::shared_ptr<llvm::Module> getModule() const;
			//-------------------------------------------------------------------------
			//! \return The LLVMContext used to build the module.
			//-------------------------------------------------------------------------
			llvm::LLVMContext& getContext() const;
			//-------------------------------------------------------------------------
			//! \return The IRBuilder used to build the module.
			//-------------------------------------------------------------------------
			llvm::IRBuilder<>& getBuilder();

			//-----------------------------------------------------------------------------
			//! \return The Function with the given name.
			//-----------------------------------------------------------------------------
			llvm::Function * getFunctionFromName( std::string const & name );

		protected:
			llvm::LLVMContext & context;
			llvm::IRBuilder<> builder;

			std::shared_ptr<llvm::Module> module;
		};
	}
}