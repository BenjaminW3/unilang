#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(long double const & x)
		{
			// FIXME: downcast from long double -> double
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(uint64_t const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(uiIntSize), x, false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(bool const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
	}
}