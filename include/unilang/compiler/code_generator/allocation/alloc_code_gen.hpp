#pragma once

#include <string>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class AllocaInst;
	class Value;
	class Function;
	class Type;
}

namespace unilang
{ 
	// forward declarations
	namespace ast
	{
		struct variable_declaration;
		struct variable_definition;
	}

	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class llvm_code_generator;
		class symbol_code_generator;
		class expression_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class allocation_code_generator :	//public boost::static_visitor<llvm::AllocaInst*>,
											virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			allocation_code_generator(	code_generator_errors & codeGeneratorErrors,
										llvm_code_generator & llvmCodeGenerator,
										symbol_code_generator & symbolCodeGenerator,
										expression_code_generator & expressionCodeGenerator );

			typedef llvm::AllocaInst * result_type;

			llvm::AllocaInst * operator()(ast::variable_declaration const & x);
			llvm::AllocaInst * operator()(ast::variable_definition const & x);

		private:
			//-----------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.
			//! This is used for mutable variables etc.
			//! Only used internally.
			//-----------------------------------------------------------------------------
			static llvm::AllocaInst * createEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & sVarName);

			//-----------------------------------------------------------------------------
			//! Tries to cast the given value to the given type.
			//! Implemented casts: fXX -> iXX, iXX -> fXX
			//! \return The casted value. 0 if cast was not possible.
			//-----------------------------------------------------------------------------
			llvm::Value * create_cast(llvm::Value * pVal, llvm::Type * const pDestinationType);

		private:
			code_generator_errors & m_codeGeneratorErrors;
			llvm_code_generator & m_llvmCodeGenerator;
			symbol_code_generator & m_symbolCodeGenerator;
			expression_code_generator & m_expressionCodeGenerator;
		};
	}
}