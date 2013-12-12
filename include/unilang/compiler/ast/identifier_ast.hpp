#pragma once

#include <unilang/compiler/ast/ast_base.hpp>

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
		struct identifier :	public ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			identifier();
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			identifier(std::string const & name);

			bool isPure() const override;

			std::string _name;
		};
		std::ostream& operator<<(std::ostream& out, identifier const& x);
		//#########################################################################
		//! A namespaced identifier.
		//#########################################################################
		struct namespaced_identifier :	public ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			namespaced_identifier();

			bool isPure() const override;

			std::string getMangledName() const;

			bool m_bGlobalNamespace;
			std::vector<std::string> m_sNames;
		};
		std::ostream& operator<<(std::ostream& out, namespaced_identifier const& x);
	}
}