#pragma once

#include "../llvm/llvm_code_gen.hpp"

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
		class constants_code_generator :	public virtual llvm_code_generator,
											//public boost::static_visitor<llvm::Constant*>,
											virtual boost::noncopyable
		{
		public:
			typedef llvm::Constant * result_type;

			llvm::Constant * operator()(long double const & x);
			llvm::Constant * operator()(double const & x);
			llvm::Constant * operator()(float const & x);
			llvm::Constant * operator()(uint64_t const & x);
			llvm::Constant * operator()(int64_t const & x);
			llvm::Constant * operator()(uint32_t const & x);
			llvm::Constant * operator()(int32_t const & x);
			llvm::Constant * operator()(bool const & x);
		};
	}
}