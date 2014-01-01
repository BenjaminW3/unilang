#pragma once

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! The base class for all ast objects.
		//! It holds the id for annotations.
		//#########################################################################
		struct ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			ast_base();
			
			//-------------------------------------------------------------------------
			//! Base mathode that can be overwritten.
			//! This base method always returns false.
			//! \return If the object is pure.
			//-------------------------------------------------------------------------
			virtual bool isPure() const
			{
				return false;
			}

			size_t _id;
		};
	}
}