#include "../code_generator.hpp"

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
			if (!(*this)(x.condition))
				return false;
			current->op(op_jump_if, 0);                 // we shall fill this (0) in later
			std::size_t exit = current->size()-1;       // mark its position
			if (!(*this)(x.body))
				return false;
			current->op(op_jump,
				int(loop-1) - int(current->size()));    // loop back
			(*current)[exit] = current->size()-exit;    // now we know where to jump to (to exit the loop)
			return true;
		}*/
	}
}