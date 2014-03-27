#include <unilang/compiler/ast/AST.hpp>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SMetaEntity::SMetaEntity() : base_type() {}
		SMetaEntity::SMetaEntity(SFunctionDeclaration const & val) : base_type(val) {}
		SMetaEntity::SMetaEntity(SFunctionDefinition const & val) : base_type(val) {}
		SMetaEntity::SMetaEntity(SNamespaceDeclaration const & val) : base_type(val) {}

		std::ostream& operator<<(std::ostream& out, SMetaEntity const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<SFunctionDeclaration>(x); break;
				case 1: out << boost::get<SFunctionDefinition>(x); break;
				case 2: out << boost::get<SNamespaceDeclaration>(x); break;
				default: out << "undefined SMetaEntity"; break;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, std::vector<SMetaEntity> const & x)
		{
			bool bFirstRet = false;
			for(SMetaEntity const & me : x)
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
		std::ostream& operator<<(std::ostream& out, SModule const & x)
		{
			out << x.m_vMetaEntities;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SNamespaceDeclaration const & x)
		{
			out << "namespace : " << x.m_idfName
				<< "{" << std::endl
				<< x.m_vMetaEntities << std::endl
				<< "}" << std::endl;
			return out;
		}
	}
}