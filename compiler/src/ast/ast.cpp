#include "ast.hpp"

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		meta_entity::meta_entity() : base_type() {}
		meta_entity::meta_entity(function_declaration const & val) : base_type(val) {}
		meta_entity::meta_entity(function_definition const & val) : base_type(val) {}
		meta_entity::meta_entity(namespace_declaration const & val) : base_type(val) {}

		std::ostream& operator<<(std::ostream& out, meta_entity const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<function_declaration>(x); break;
				case 1: out << boost::get<function_definition>(x); break;
				case 2: out << boost::get<namespace_declaration>(x); break;
				default: out << "undefined meta_entity"; break;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, std::vector<meta_entity> const& x)
		{
			bool bFirstRet = false;
			for(meta_entity const & me : x)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << std::endl;}

				out << me;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, module const& x)
		{
			out << x._metaEntities;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, namespace_declaration const& x)
		{
			out << "namespace : " << x._idfName
				<< "{" << std::endl
				<< x._metaEntities << std::endl
				<< "}" << std::endl;
			return out;
		}
	}
}