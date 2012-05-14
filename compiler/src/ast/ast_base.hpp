#pragma once

#include <string>

namespace unilang 
{
	namespace ast
	{
		struct ast_base abstract
		{
			std::string::iterator m_itBegin;
			std::string::iterator m_itEnd;
		};
	}
}