#pragma once

#include "../ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::module,
	(std::list<unilang::ast::meta_entity>, _metaEntities)
)