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
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			ast_base();

			size_t _id;
			
			//-------------------------------------------------------------------------
			//! Base mathode that can be overwritten.
			//! This base method always returns false.
			//! \return If the object is pure.
			//-------------------------------------------------------------------------
			virtual bool isPure() const
			{
				return false;
			}
		};
	}
}