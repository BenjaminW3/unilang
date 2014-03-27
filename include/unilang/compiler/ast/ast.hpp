#pragma once

#include <unilang/compiler/ast/Base.hpp>
#include <unilang/compiler/ast/Identifier.hpp>
#include <unilang/compiler/ast/Expression.hpp>
#include <unilang/compiler/ast/Statement.hpp>
#include <unilang/compiler/ast/Function.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>

#include <ostream>
#include <vector>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SNamespaceDeclaration;
	}
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining all the abstract syntax tree objects.
	//-------------------------------------------------------------------------
	namespace ast
	{
		//#########################################################################
		//! A SMetaEntity can be multiple things.
		//#########################################################################
		struct SMetaEntity :
			public SASTBase,
			boost::spirit::extended_variant<	SFunctionDeclaration, 
												SFunctionDefinition, 
												boost::recursive_wrapper<SNamespaceDeclaration>
											>
		{ 
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SMetaEntity();
			SMetaEntity(SFunctionDeclaration const & val);
			SMetaEntity(SFunctionDefinition const & val);
			SMetaEntity(SNamespaceDeclaration const & val);
		};
		std::ostream& operator<<(std::ostream& out, SMetaEntity const & x);
		std::ostream& operator<<(std::ostream& out, std::vector<SMetaEntity> const & x);

		//#########################################################################
		//! A namespace declaration.
		//#########################################################################
		struct SNamespaceDeclaration :
			public SASTBase
		{
			SIdentifier m_idfName;
			std::vector<SMetaEntity> m_vMetaEntities;
		};
		std::ostream& operator<<(std::ostream& out, SNamespaceDeclaration const & x);

		//#########################################################################
		//! A module is the root of the abstract syntax tree.
		//#########################################################################
		struct SModule :
			public SASTBase
		{
			std::vector<SMetaEntity> m_vMetaEntities;
		};
		std::ostream& operator<<(std::ostream& out, SModule const & x);
	}
}