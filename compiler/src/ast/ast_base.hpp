#pragma once

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! The base class for all ast objects.
		//! It holds the id for annotations.
		//#########################################################################
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