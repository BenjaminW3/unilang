#include "constants_code_gen.hpp"

#include "../llvm/llvm_code_gen.hpp"

#include "../../log/log.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
//#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
//#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
//#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/Constants.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		constants_code_generator::constants_code_generator( llvm_code_generator & llvmCodeGenerator )
			:m_llvmCodeGenerator(llvmCodeGenerator)
		{
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(long double const & x)
		{
			// FIXME: downcast from long double -> double
			return llvm::ConstantFP::get(m_llvmCodeGenerator.getContext(), llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(double const & x)
		{
			return llvm::ConstantFP::get(m_llvmCodeGenerator.getContext(), llvm::APFloat(x));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(float const & x)
		{
			return llvm::ConstantFP::get(m_llvmCodeGenerator.getContext(), llvm::APFloat(double(x)));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(uint64_t const & x)
		{
			return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(64), x, false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(int64_t const & x)
		{
			if(x<0)
			{
				return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(64), uint64_t(std::abs(x)), true));
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
			return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(32), uint64_t(x), false));
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Constant * constants_code_generator::operator()(int32_t const & x)
		{
			if(x<0)
			{
				return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(32), uint64_t(std::abs(x)), true));
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
			return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
	}
}