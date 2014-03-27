#pragma once

#include <unilang/compiler/ast/Base.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! An identifier.
		//#########################################################################
		struct SIdentifier :
			public SASTBase
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SIdentifier();
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SIdentifier(std::string const & name);

			std::string m_sName;
		};
		std::ostream& operator<<(std::ostream& out, SIdentifier const & x);

		//#########################################################################
		//! A namespaced identifier.
		//#########################################################################
		struct SNamespacedIdentifier :
			public SASTBase
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SNamespacedIdentifier();

			std::string getMangledName() const;

			bool m_bGlobalNamespace;
			std::vector<std::string> m_sNames;
		};
		std::ostream& operator<<(std::ostream& out, SNamespacedIdentifier const & x);
	}
}