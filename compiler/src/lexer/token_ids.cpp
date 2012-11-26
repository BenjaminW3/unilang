#include "token_ids_def.hpp"

namespace unilang
{
	namespace lexer
	{
		namespace tokens
		{
			/*ETokenIDs operator|(ETokenIDs const & a, ETokenIDs const & b)	{return static_cast<ETokenIDs>(static_cast<size_t>(a) | static_cast<size_t>(b));}
			ETokenIDs & operator|=(ETokenIDs & a, ETokenIDs const & b)		{a = a|b; return a;}
			ETokenIDs operator&(ETokenIDs const & a, ETokenIDs const & b)	{return static_cast<ETokenIDs>(static_cast<size_t>(a) & static_cast<size_t>(b));}
			ETokenIDs & operator&=(ETokenIDs & a, ETokenIDs const & b)		{a = a & b; return a;}
			ETokenIDs operator~(ETokenIDs const & a)						{return static_cast<ETokenIDs>(~int(a));}
			ETokenIDs & operator ++(ETokenIDs & a)							{ a = static_cast<ETokenIDs>(static_cast<size_t>(a) + 1); return a; }
			ETokenIDs operator ++(ETokenIDs & a, int)						{ETokenIDs b = a; ++a; return b;}*/

			inline std::ostream& operator<<(std::ostream& out, ETokenIDs const& x)
			{
				switch(x)
				{
					case ETokenIDs::identifier:				out << "identifier"; break;
					case ETokenIDs::comment:				out << "comment"; break;
					case ETokenIDs::whitespace:				out << "whitespace"; break;
					case ETokenIDs::comma:					out << ","; break;
					case ETokenIDs::colon:					out << ":"; break;
					case ETokenIDs::semicolon:				out << ";"; break;
					case ETokenIDs::tilde:					out << "~"; break;
					case ETokenIDs::question_mark:			out << "?"; break;
					case ETokenIDs::opening_parenthesis:	out << "("; break;
					case ETokenIDs::closing_parenthesis:	out << ")"; break;
					case ETokenIDs::opening_brace:			out << "{"; break;
					case ETokenIDs::closing_brace:			out << "}"; break;
					//case ETokenIDs::opening_bracket:		out << "["; break;
					//case ETokenIDs::closing_bracket:		out << "]"; break;
					case ETokenIDs::arrow:					out << "->"; break;
					case ETokenIDs::if_:					out << "if"; break;
					case ETokenIDs::else_:					out << "else"; break;
					//case ETokenIDs::while_:				out << "while"; break;
					//case ETokenIDs::return_:				out << "return"; break;

					case ETokenIDs::invalid:				out << "invalid"; break;

					case ETokenIDs::equal:					out << "=="; break;
					case ETokenIDs::not_equal:				out << "!="; break;
					case ETokenIDs::less:					out << "<"; break;
					case ETokenIDs::less_equal:				out << "<="; break;
					case ETokenIDs::greater:				out << ">"; break;
					case ETokenIDs::greater_equal:			out << ">="; break;

					case ETokenIDs::logical_or:				out << "||"; break;
					case ETokenIDs::logical_and:			out << "&&"; break;

					case ETokenIDs::bit_or:					out << "|"; break;
					case ETokenIDs::bit_xor:				out << "~"; break;
					case ETokenIDs::bit_and:				out << "&"; break;
					case ETokenIDs::shift_left:				out << "<<"; break;
					case ETokenIDs::shift_right:			out << ">>"; break;
					case ETokenIDs::times:					out << "*"; break;
					case ETokenIDs::divide:					out << "/"; break;
					case ETokenIDs::mod:					out << "%"; break;

					case ETokenIDs::plus:					out << "+"; break;
					case ETokenIDs::minus:					out << "-"; break;

					case ETokenIDs::not_:					out << "!"; break;
					case ETokenIDs::stringify:				out << "$"; break;
					//case ETokenIDs::compl_:				out << "!"; break;

					case ETokenIDs::plus_plus:				out << "++"; break;
					case ETokenIDs::minus_minus:			out << "--"; break;

					case ETokenIDs::assign:					out << "="; break;
					case ETokenIDs::bit_or_assign:			out << "|="; break;
					case ETokenIDs::bit_xor_assign:			out << "~="; break;
					case ETokenIDs::bit_and_assign:			out << "&="; break;
					case ETokenIDs::shift_left_assign:		out << "<<="; break;
					case ETokenIDs::shift_right_assign:		out << ">>="; break;
					case ETokenIDs::times_assign:			out << "*="; break;
					case ETokenIDs::divide_assign:			out << "/="; break;
					case ETokenIDs::mod_assign:				out << "%="; break;
					case ETokenIDs::plus_assign:			out << "+="; break;
					case ETokenIDs::minus_assign:			out << "-="; break;

					case ETokenIDs::_lit_ufloat:				out << "_lit_ufloat"; break;
					case ETokenIDs::_lit_uint:				out << "_lit_uint"; break;
					case ETokenIDs::_lit_boolean:			out << "_lit_boolean"; break;

					default: out << "unknown-token-ETokenIDs"; break;
				}
				return out;
			}
		}
	}
}