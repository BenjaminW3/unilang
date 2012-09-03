#pragma once

namespace unilang 
{
	namespace ast
	{
		struct ast_base abstract
		{
			size_t id;

			virtual bool isPure() const
			{
				return false;
			}
		};
	}
}