#pragma once

#include <unilang/compiler/ast/Base.hpp>
#include <unilang/compiler/ast/Identifier.hpp>
#include <unilang/compiler/ast/Expression.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>

#include <vector>
#include <ostream>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SIfStatement;
		//struct SWhileStatement;
		//struct SReturnStatement;
		struct SStatementVector;
	}
}

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! A statement.
		//#########################################################################
		struct SStatement :
			public SASTBase,
			boost::spirit::extended_variant<	boost::recursive_wrapper<SIfStatement>,
												//boost::recursive_wrapper<SWhileStatement>,
												//boost::recursive_wrapper<SReturnStatement>,
												SAssignment,
												SExpression,
												boost::recursive_wrapper<SStatementVector>
											>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SStatement();
			SStatement(SIfStatement const & val);
			//SStatement(SWhileStatement const & val);
			//SStatement(SReturnStatement const & val);
			SStatement(SAssignment const & val);
			SStatement(SExpression const & val);
			SStatement(SStatementVector const & val);
		};
		std::ostream& operator<<(std::ostream& out, SStatement const & x);
		
		//#########################################################################
		//! A statement list of multiple statements.
		//#########################################################################
		struct SStatementVector :
			std::vector<SStatement>,
			public SASTBase
		{
		};
		std::ostream& operator<<(std::ostream& out, SStatementVector const & x);

		//#########################################################################
		//! If-statement.
		//#########################################################################
		struct SIfStatement :
			public SASTBase
		{
			SExpression m_expCondition;
			SStatementVector _thenStatementList;
			boost::optional<SStatementVector> _elseOptionalStatementList;
		};
		std::ostream& operator<<(std::ostream& out, SIfStatement const & x);

		//#########################################################################
		//! While statement.
		//#########################################################################
		/*struct SWhileStatement :
			public SASTBase
		{
			SExpression m_expCondition;
			SStatement m_body;
		};*/

		//#########################################################################
		//! Return statement.
		//#########################################################################
		/*struct SReturnStatement :
			public SASTBase
		{
		};*/
	}
}