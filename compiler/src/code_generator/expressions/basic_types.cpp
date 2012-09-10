#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(long double const & x)
		{
			// FIXME: downcast from long double -> double
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(unsigned int const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(uiIntSize), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(int const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(uiIntSize), uint64_t(x), true));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(bool const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
	}
}