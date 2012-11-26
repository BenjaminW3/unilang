#pragma once

#include "../llvm/llvm_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"
#include "../expressions/exp_code_gen.hpp"
#include "../allocation/alloc_code_gen.hpp"
#include "../errors.hpp"

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
		class statement_code_generator :	public virtual code_generator_errors,
											public virtual llvm_code_generator,
											public virtual symbol_code_generator,
											public virtual expression_code_generator,
											public virtual allocation_code_generator,
											//public boost::static_visitor<bool>,
											virtual boost::noncopyable
		{
		public:
			statement_code_generator()
				:allocation_code_generator(*this)
			{};

		public:
			typedef bool result_type;

			bool operator()(ast::statement_list const & x);
			bool operator()(ast::statement const & x);
			bool operator()(ast::expression const & x);
			bool operator()(ast::if_statement const & x);
			//bool operator()(ast::while_statement const & x);
			//bool operator()(ast::return_statement const & x);
		};
	}
}