#pragma once

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class Constant;
}
namespace unilang
{
	namespace lexer
	{
		struct SFloatLiteral;
		struct SIntLiteral;
	}
	namespace code_generator
	{
		class CLLVMCodeGen;
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
		//! 
		//#########################################################################
		class CConstantsCodeGen :
			//public boost::static_visitor<llvm::Constant*>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			CConstantsCodeGen( 
				CLLVMCodeGen & llvmCodeGenerator );

			typedef llvm::Constant * result_type;

			llvm::Constant * operator()(lexer::SFloatLiteral const & x);
			llvm::Constant * operator()(lexer::SIntLiteral const & x);
			llvm::Constant * operator()(bool const & x);

		private:
			CLLVMCodeGen & m_llvmCodeGenerator;
		};
	}
}