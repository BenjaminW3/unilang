#pragma once

#include <unilang/compiler/ast/OperatorsFwd.hpp>
#include <unilang/compiler/ast/Base.hpp>
#include <unilang/compiler/ast/Identifier.hpp>
#include <unilang/compiler/lexer/Literals.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <vector>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SExpression;
		struct SUnaryExpr;
		struct SFunctionCall;
		struct SVariableDefinition;
		//struct SAssignment;
	}
}

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! A primary expression.
		//#########################################################################
		struct SPrimaryExpr :
			public SASTBase,
			boost::spirit::extended_variant<	std::string,
												lexer::SFloatLiteral,
												lexer::SIntLiteral,
												bool,
												SIdentifier,
												boost::recursive_wrapper<SExpression>
											>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SPrimaryExpr();
			SPrimaryExpr(std::string const & val);
			SPrimaryExpr(lexer::SFloatLiteral const & val);
			SPrimaryExpr(lexer::SIntLiteral const & val);
			SPrimaryExpr(bool const & val);
			SPrimaryExpr(SIdentifier const & val);
			SPrimaryExpr(SExpression const & val);
			SPrimaryExpr(SPrimaryExpr const & rhs);
		};
		std::ostream& operator<<(std::ostream& out, SPrimaryExpr const & x);

		//#########################################################################
		//! A operand.
		//#########################################################################
		struct SOperand :
			public SASTBase,
			boost::spirit::extended_variant<	SPrimaryExpr,
												boost::recursive_wrapper<SUnaryExpr>,
												boost::recursive_wrapper<SFunctionCall>,
												boost::recursive_wrapper<SVariableDefinition>//,
												//boost::recursive_wrapper<SAssignment>
											>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SOperand();
			SOperand(SPrimaryExpr const & val);
			SOperand(SUnaryExpr const & val);
			SOperand(SFunctionCall const & val);
			SOperand(SVariableDefinition const & val);
			//SOperand(SAssignment const & val);
			SOperand(SOperand const & rhs);
		};
		std::ostream& operator<<(std::ostream& out, SOperand const & x);

		//#########################################################################
		//! A unary expression.
		//#########################################################################
		struct SUnaryExpr :
			public SASTBase
		{
			OPERATOR_TYPE m_uiOperatorID;
			SOperand m_opOperand;
		};
		std::ostream& operator<<(std::ostream& out, SUnaryExpr const & x);

		//#########################################################################
		//! A operation.
		//#########################################################################
		struct SOperation :
			public SASTBase
		{
			OPERATOR_TYPE m_uiOperatorID;
			SOperand m_opOperand;
		};
		std::ostream& operator<<(std::ostream& out, SOperation const & x);

		//#########################################################################
		//! An expression.
		//#########################################################################
		struct SExpression :
			public SASTBase
		{
			SOperand m_firstOperand;
			std::vector<SOperation> m_vRestOperands;
		};
		std::ostream& operator<<(std::ostream& out, SExpression const & x);
		std::ostream& operator<<(std::ostream& out, std::vector<SExpression> const & x);

		//#########################################################################
		//! A function call.
		//#########################################################################
		struct SFunctionCall :
			public SASTBase
		{
			SNamespacedIdentifier m_idfName;
			bool m_bHasUnpureQualifier;
			std::vector<SExpression> m_vArgumentExpressions;
		};
		std::ostream& operator<<(std::ostream& out, SFunctionCall const & x);

		//#########################################################################
		//! A type declaration.
		//#########################################################################
		struct SVariableTypeDeclaration :
			public SASTBase
		{
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration();
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration(SIdentifier const & type_identifier);
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration(bool bHasMutableQualifier, SIdentifier const & type_identifier);

			//-------------------------------------------------------------------------
			//! \return Unique names for different types.
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

			bool m_bHasMutableQualifier;
			SIdentifier m_idfName;
		};
		std::ostream& operator<<(std::ostream& out, SVariableTypeDeclaration const & x);

		//#########################################################################
		//! A variable declaration.
		//#########################################################################
		struct SVariableDeclaration :
			public SASTBase
		{
			boost::optional<SIdentifier> m_optionalIdentifier;
			SVariableTypeDeclaration m_type;
		};
		std::ostream& operator<<(std::ostream& out, SVariableDeclaration const & x);

		//#########################################################################
		//! A variable definition.
		//#########################################################################
		struct SVariableDefinition :
			public SASTBase
		{
			SVariableDeclaration m_declaration;
			std::vector<SExpression> m_vParameterExpressions;
		};
		std::ostream& operator<<(std::ostream& out, SVariableDefinition const & x);

		//#########################################################################
		//! Assignment consists of a identifier, the operator and an expression.
		//#########################################################################
		struct SAssignment :
			public SASTBase
		{
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SAssignment();
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SAssignment(SIdentifier lhs, OPERATOR_TYPE op, SExpression rhs);

			SIdentifier m_lhs;
			OPERATOR_TYPE m_uiOperatorID;
			SExpression m_rhs;
		};
		std::ostream& operator<<(std::ostream& out, SAssignment const & x);
	}
}