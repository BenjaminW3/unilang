#pragma once

//#include "../../ast/ast.hpp"

//#include <boost/variant/apply_visitor.hpp>


// forward declarations
namespace llvm
{
	class AllocaInst;
	class Value;
	class Function;
	class Type;
}

#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"
#include "../constants/constants_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"
//#include "../expressions/exp_code_gen.hpp"

#include <boost/noncopyable.hpp>

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
		class expression_code_generator;
		//#########################################################################
		//! 
		//#########################################################################
		class allocation_code_generator :	public virtual code_generator_errors,
											public virtual llvm_code_generator,
											public virtual constants_code_generator,
											public virtual symbol_code_generator,
											//public virtual expression_code_generator,
											//public boost::static_visitor<llvm::AllocaInst*>,
											virtual boost::noncopyable
		{
		public:
			allocation_code_generator(expression_code_generator & exp_code_gen)
				:_exp_code_gen(exp_code_gen){}

		protected:
			//-----------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.
			//! This is used for mutable variables etc.
			//-----------------------------------------------------------------------------
			static llvm::AllocaInst * createEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName);
			
			//-----------------------------------------------------------------------------
			//! \return An invalid alloca for use in error situations.
			//-----------------------------------------------------------------------------
			llvm::AllocaInst *ErrorAllocaInst(std::string str, EErrorLevel err = EErrorLevel::Standard );

			//-----------------------------------------------------------------------------
			//! Tries to cast the given value to the given type.
			//! Implemented casts: fXX -> iXX, iXX -> fXX
			//! \return The casted value. 0 if cast was not possible.
			//-----------------------------------------------------------------------------
			llvm::Value * create_cast(llvm::Value * pVal, llvm::Type * const pDestinationType);

		public:
			typedef llvm::AllocaInst * result_type;

			llvm::AllocaInst * operator()(ast::variable_declaration const & x);
			llvm::AllocaInst * operator()(ast::variable_definition const & x);

		private:
			expression_code_generator & _exp_code_gen;
		};
	}
}