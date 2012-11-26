#include "statement_code_gen.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * code_generator::operator()(ast::while_statement const& x)
		{
			LOG_SCOPE;

			builder.
			BOOST_ASSERT(current != 0);
			std::size_t loop = current->size();         // mark our position
			if (!(*this)(x._condition))
				return false;
			current->op(op_jump_if, 0);                 // we shall fill this (0) in later
			std::size_t exit = current->size()-1;       // mark its position
			if (!(*this)(x._body))
				return false;
			current->op(op_jump,
				int(loop-1) - int(current->size()));    // loop back
			(*current)[exit] = current->size()-exit;    // now we know where to jump to (to exit the loop)
			return true;
		}*/

		/*
		function f = get_current_function();

		basic_block cond_block = make_basic_block("while.cond", f);
		basic_block body_block = make_basic_block("while.body");
		basic_block exit_block = make_basic_block("while.end");

		branch(cond_block);
		set_insert_point(cond_block);
		value _condition = (*this)(x._condition);
		if (!_condition.is_valid())
			return false;
		conditional_branch(_condition, body_block, exit_block);
		f.add(body_block);
		set_insert_point(body_block);

		if (!(*this)(x._body))
			return false;

		if (!body_block.has_terminator())
			branch(cond_block); // loop back

		// Emit exit block
		f.add(exit_block);
		set_insert_point(exit_block);

		return true;
		*/
	}
}