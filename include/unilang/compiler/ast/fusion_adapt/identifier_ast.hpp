#pragma once

#include <unilang/compiler/ast/identifier_ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>


BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::identifier,
	(std::string, m_sName)
)
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::namespaced_identifier,
	(bool, m_bGlobalNamespace)
	(std::vector<std::string>, m_sNames)
)