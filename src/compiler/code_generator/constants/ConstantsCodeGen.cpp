#include <unilang/compiler/code_generator/constants/ConstantsCodeGen.hpp>

#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>

#include <unilang/compiler/ast/Expression.hpp>

#include <unilang/compiler/log/log.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::Type' : assignment operator could not be generated
#endif

#include <llvm/IR/Constants.h>

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
		CConstantsCodeGen::CConstantsCodeGen(
			CLLVMCodeGen & llvmCodeGenerator ) :
			m_llvmCodeGenerator(llvmCodeGenerator)
		{
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Constant * CConstantsCodeGen::operator()(lexer::SFloatLiteral const & x)
		{
			return llvm::ConstantFP::get(m_llvmCodeGenerator.getContext(), llvm::APFloat(static_cast<double>(x.m_uiFullPrecisionValue)));	// FIXME: Convert floating point literals to the required bit width.
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Constant * CConstantsCodeGen::operator()(lexer::SIntLiteral const & x)
		{
			return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(static_cast<unsigned int>(x.m_uiNumBitsType), static_cast<uint64_t>(x.m_uiFullPrecisionValue), x.m_bSignedType));	// FIXME: Allow integer literals with required bit width > 64.
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Constant * CConstantsCodeGen::operator()(bool const & x)
		{
			return llvm::ConstantInt::get(m_llvmCodeGenerator.getContext(), llvm::APInt(unsigned int(1), uint64_t(x), false));
		}
	}
}