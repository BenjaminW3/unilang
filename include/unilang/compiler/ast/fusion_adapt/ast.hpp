#pragma once

#include <unilang/compiler/ast/AST.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SModule,
	(std::vector<unilang::ast::SMetaEntity>, m_vMetaEntities)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SNamespaceDeclaration,
	(unilang::ast::SIdentifier, m_idfName)
	(std::vector<unilang::ast::SMetaEntity>, m_vMetaEntities)
)