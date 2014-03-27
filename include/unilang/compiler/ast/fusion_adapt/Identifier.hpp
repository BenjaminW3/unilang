#pragma once

#include <unilang/compiler/ast/Identifier.hpp>

#include <boost/fusion/include/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SIdentifier,
	(std::string, m_sName)
)
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SNamespacedIdentifier,
	(bool, m_bGlobalNamespace)
	(std::vector<std::string>, m_sNames)
)