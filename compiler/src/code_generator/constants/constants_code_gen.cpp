#include "constants_code_gen.hpp"

#include "../../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(long double const & x)
		{
			// FIXME: downcast from long double -> double
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(double const & x)
		{
			return llvm::ConstantFP::get(context, llvm::APFloat(x));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(float const & x)
		{
			return llvm::ConstantFP::get(context, llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(uint64_t const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(64), x, false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(int64_t const & x)
		{
			if(x<0)
			{
				return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(64), uint64_t(std::abs(x)), true));
			}
			else
			{
				return (*this)(uint64_t(x));
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(uint32_t const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(32), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(int32_t const & x)
		{
			if(x<0)
			{
				return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(32), uint64_t(std::abs(x)), true));
			}
			else
			{
				return (*this)(uint32_t(x));
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(bool const & x)
		{
			return llvm::ConstantInt::get(context, llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
	}
}