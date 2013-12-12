#pragma once

#include <unilang/compiler/ast/ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::module,
	(std::vector<unilang::ast::meta_entity>, _metaEntities)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::namespace_declaration,
	(unilang::ast::identifier, _idfName)
	(std::vector<unilang::ast::meta_entity>, _metaEntities)
)