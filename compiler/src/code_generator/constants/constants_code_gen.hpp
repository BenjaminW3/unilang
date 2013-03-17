#pragma once

#include <stdint.h>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class Constant;
}

namespace unilang
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class llvm_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class constants_code_generator :	//public boost::static_visitor<llvm::Constant*>,
											virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			constants_code_generator( llvm_code_generator & llvmCodeGenerator );

			typedef llvm::Constant * result_type;

			llvm::Constant * operator()(long double const & x);
			llvm::Constant * operator()(double const & x);
			llvm::Constant * operator()(float const & x);
			llvm::Constant * operator()(uint64_t const & x);
			llvm::Constant * operator()(int64_t const & x);
			llvm::Constant * operator()(uint32_t const & x);
			llvm::Constant * operator()(int32_t const & x);
			llvm::Constant * operator()(bool const & x);

		private:
			llvm_code_generator & m_llvmCodeGenerator;
		};
	}
}