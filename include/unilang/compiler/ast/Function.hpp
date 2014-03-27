#pragma once

#include <unilang/compiler/ast/Base.hpp>
#include <unilang/compiler/ast/Identifier.hpp>
#include <unilang/compiler/ast/Statement.hpp>

#include <ostream>
#include <vector>

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! A Function declaration.
		//#########################################################################
		struct SFunctionDeclaration :
			public SASTBase
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SFunctionDeclaration();
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SFunctionDeclaration( 
				SIdentifier const & idfName,
				bool bHasUnpureQualifier, 
				std::vector<SVariableDeclaration> const & vParameterTypes,
				std::vector<SVariableDeclaration> const & vReturnTypes);

			//-------------------------------------------------------------------------
			//! \return Unique names for functions with different parameters. add:(i32,i32)
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

			SIdentifier m_idfName;
			bool m_bHasUnpureQualifier;
			std::vector<SVariableDeclaration> m_vParameterDeclarations;
			std::vector<SVariableDeclaration> m_vReturnDeclarations;
		};
		std::ostream& operator<<(std::ostream& out, SFunctionDeclaration const & x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct SFunctionDefinition :
			public SASTBase
		{
			SIdentifier m_idfName;
			bool m_bHasUnpureQualifier;
			std::vector<SVariableDeclaration> m_vParameterDeclarations;
			SStatementVector m_body;
			std::vector<SExpression> m_vReturnExpressions;
		};
		std::ostream& operator<<(std::ostream& out, SFunctionDefinition const & x);
	}
}