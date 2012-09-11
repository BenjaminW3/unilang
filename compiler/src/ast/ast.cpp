#include "ast.hpp"

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		meta_entity::meta_entity() : base_type() {}
		meta_entity::meta_entity(function_declaration const & val) : base_type(val) {}
		meta_entity::meta_entity(function_definition const & val) : base_type(val) {}

		std::ostream& operator<<(std::ostream& out, meta_entity const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<function_declaration>(x); break;
				case 1: out << boost::get<function_definition>(x); break;
				default: out << "undefine-statement"; break;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, module const& x)
		{
			bool bFirstRet = false;
			for(meta_entity const & me : x.metaEntities)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << std::endl;}

				out << me;
			}
			return out;
		}
	}
}