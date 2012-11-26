#pragma once

#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"
#include "../constants/constants_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"
#include "../allocation/alloc_code_gen.hpp"

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
		class expression_code_generator :	public virtual llvm_code_generator,
											public virtual constants_code_generator,
											public virtual symbol_code_generator,
											public virtual allocation_code_generator,
											//public boost::static_visitor<llvm::Value*>,
											virtual boost::noncopyable
		{
		public:
			expression_code_generator()
				:allocation_code_generator(*this)
			{};

		protected:
			//-----------------------------------------------------------------------------
			//! \return The expression created with the shunting yard algorithm.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateExpression(	size_t min_precedence,
											llvm::Value * lhs,
											std::list<ast::operation>::const_iterator& rest_begin,
											std::list<ast::operation>::const_iterator rest_end);
			//-----------------------------------------------------------------------------
			//! \return The value returned from the execution of 'L op R'.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateBinaryOperation(llvm::Value * L, llvm::Value * R, operators::EOperators op);

		public:
			typedef llvm::Value * result_type;

			llvm::Value * operator()(ast::primary_expr const& x);
			llvm::Value * operator()(ast::identifier const & x);
			llvm::Value * operator()(ast::unary_expr const & x);
			llvm::Value * operator()(ast::function_call const & x);
			llvm::Value * operator()(ast::expression const & x);
			//llvm::Value * operator()(ast::operand const & x);
			llvm::Value * operator()(ast::assignment const & x);

			llvm::Value * operator()(long double const & x);
			llvm::Value * operator()(double const & x);
			llvm::Value * operator()(float const & x);
			llvm::Value * operator()(uint64_t const & x);
			llvm::Value * operator()(int64_t const & x);
			llvm::Value * operator()(uint32_t const & x);
			llvm::Value * operator()(int32_t const & x);
			llvm::Value * operator()(bool const & x);

			llvm::Value * operator()(ast::variable_definition const & x);
		};
	}
}