#include <unilang/compiler/code_generator/statements/StatementCodeGen.hpp>

#include <unilang/compiler/log/log.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*llvm::Value * CCodeGenerator::operator()(ast::SWhileStatement const & x)
		{
			LOG_SCOPE;

			getBuilder()->
			BOOST_ASSERT(current != 0);
			std::size_t loop = current->size();         // mark our position
			if (!(*this)(x.m_expCondition))
				return false;
			current->op(op_jump_if, 0);                 // we shall fill this (0) in later
			std::size_t exit = current->size()-1;       // mark its position
			if (!(*this)(x.m_body))
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
		value m_expCondition = (*this)(x.m_expCondition);
		if (!m_expCondition.is_valid())
			return false;
		conditional_branch(m_expCondition, body_block, exit_block);
		f.add(body_block);
		set_insert_point(body_block);

		if (!(*this)(x.m_body))
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