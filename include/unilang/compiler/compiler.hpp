#pragma once

#include <unilang/compiler/Export.hpp>

#include <string>
#include <memory>
#include <vector>

//-------------------------------------------------------------------------
//! \mainpage unilang
//! 
//! \section intro_sec Introduction
//! \verbinclude README.txt
//! 
//! \section install_sec The Language
//!
//! \subsection lang1 Design Decisions
//! Every identifier is case sensitive.
//! During the design of the language one essential rule was that whitespace should never have any semantic. This means that there can not be types with multiple words like <tt>unsigned long long</tt> in c++.
//! Separation of identifiers and other things must be done with seperation characters like ':', ';', ',' or '|'.
//! There are no braceless forms of if, for, etc. Due to this rule the <tt>{}</tt> have to be given explicitly.
//! Variable definition has to be done via <tt>var:i32{50};</tt>. You can not initialize a variable with normal paranthesis to prevent the 'most vexing parse' http://en.wikipedia.org/wiki/Most_vexing_parse problem of c++.
//! Assignments are done via <tt>var := 51;</tt>. Assignments are statements so they do not return a value. Comparisons are done via <tt>if(a=b)</tt>. This solves the "if (a=b)" (assignment instead of comparison) problem that other languages have.
//! Integer literals can be written in multiple bases. Standard decimal ('28'), hexadecimal ('0x1C'), binary ('0b11100') or octal ('0q34'). The prefix q is used instead of o to avoid the prefix o being mistaken for a zero.
//! 
//! \subsection Lexical Elements
//!
//! \subsubsection Input
//!	The input files are interpreted as a sequence of unicode codepoits encoded in UTF-8. Each code point is distinct; for instance, upper and lower case letters are different characters.
//!
//! \subsubsection Whitespace
//!	Any of the following Unicode characters is considered whitespace: U+0020 (space, ' '), U+0009 (tab, '\t'), U+000A (LF, '\n'), U+000D (CR, '\r').
//!	Unilang is a "free-form" language, meaning that all forms of whitespace serve only to separate tokens in the grammar, and have no semantic significance.
//!
//! \subsubsection Literals
//! \paragraph Number Literals
//! \subparagraph Number Literals
//! Integer literals can be encoded in hexadecimal (0x), octal (0q), binary (0b) with prefixes or by default in decimal system.
//! Integer literals can be interrupted by underscores for ease of reading and alignment. This is completely syntax sugar.
//! <tt>
//! bin_digit = '0' | '1'
//! oct_digit = bin_digit | '2' | '3' | '4' | '5' | '6' | '7' 
//! nonzero_dec_digit: '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9';
//! dec_digit: '0' | nonzero_dec;
//! hex_digit : 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | dec_digit;
//! 
//! IntegerLiteral = :		"0x"[hex_digit | '_']+ int_suffix? 
//!						|	"0q"[oct_digit | '_']+ int_suffix?
//!						|	"0b"[bin_digit | '_']+ int_suffix?
//!						|	[dec_digit | '_']+ int_suffix?
//FIXME: implement integer suffix
//!	 </tt>
//!
//! \subsubsection Identifiers
//!	Identifiers consist of at least one letter followed by multiple letters or numbers. The underscore is considered a letter.
//! <tt>Identifier = [a-zA-Z_][a-zA-Z_0-9]*</tt>
//!
//! \subsubsection lang2 Comments
//! There are two types of comments available. 
//! On the one hand there are C++ style comments <tt>// commented text</tt>. Those comment out everything from the two slashes to the end of the line.
//! On the other hand you can use C-style comments <tt>/* commented text */</tt>. They can be used in a line and also over multiple lines e.g. <tt>iErrorCode = /*3 +*/ 4;</tt>.
//! Singleline- and multiline-comments can also be nested like <tt>iErrorCode = /*3 /*+ 8*/ +*/ 4;</tt>.
//! Comments can be used everywhere except inside a literal token. E.g. the code <tt>iErr/*rr*/orCode = 4;</tt> will not compile because now 'iErr' and 'orCode' are two different identifiers. This results in in an invalid expression. 
//! The same rule applies to <tt>iErrorCode = 1/*0*/0;</tt> because '1' and '0' represent two different numbers after lexing and there is no connecting operator between them.
//! The contents of strings are not tokenized. Consequently, comment openings occurring within a string usually do not begin a comment. 
//! However there is one exception. Comments inside a string can get recognized if a opening multiline-comment is added before the string e.g. <tt>/*sText:"this is a string containing a closing comment */";</tt>. This line ends being recognized as <tt> '";' </tt> only.
//! This is due to contents of comments not being tokenized either. So string delimiters within a comment do not affect the recognition of comment closings '*/' and nested '/*' or '//' comment openings. 
//! Single-line comments '//' occurring within a multiline-comment '/*' are not ignored. This is different from the way it is handeled in C++ and other languages.
//! So the code <tt> /* function:()->(a:i64) // This is a comment at the end of a method being commented out fully. */ </tt> is missing a closing multiline comment because the singleline-comment makes the lexer ignore the existing one.
//! This is on purpose so that commented out multiline comments are fully permissive for being recursively commented out themselves.
//! <tt>SingleLineComment = "//" non_eol* </tt>
//! <tt>MultiLineComment = "/*" MultiLineCommentBody* "*/"</tt>
//! <tt>MultiLineCommentBody = (MultiLineComment | character)* </tt>
//!
//! \subsubsection Keywords
//!	The following list of keywords are reserved and can not be used as identifiers:
//! <tt> if, else, namespace</tt>
// TODO: while, return, break, continue, default, for, import, interface, class, struct, switch
//!
//! \subsubsection Operators, Delimiters and other Symbols
//!	The following list of tokens represent special operators, delimiters or symbols:
//! <tt> 
//!	* multiplication
//!	/ division
//!	% modulo
//!	+ addition
//!	- subtraction
//!	++ increment
//!	-- decrement
//TODO:	^ exponent
//!	& bitwise-and
//!	# bitwise-xor
//!	| bitwise-or
//!	<< left-shift
//!	>> right-shift
//!	<< left-shift
//!	:= assignment
//!	*= multiplication-assignment
//!	/= division-assignment
//!	%= modulo-assignment
//TODO:	^= exponent-assignment
//!	&= bitwise-and-assignment
//!	#= bitwise-xor-assignment
//!	|= bitwise-or-assignment
//!	<<= left-shift-assignment
//!	>>= right-shift-assignment
//!	+= addition-assignment
//!	-= subtraction-assignment
//!	<<= left-shift-assignment
//TODO: Predefined for bool type:
//!	|| logical-or
//!	&& logical-and
//!	= equality
//!	!= inequality
//!	< less-then
//!	<= less-then-equal
//!	> greater-then
//!	>= greater-then-equality
//!	! not
//!	~ complement
//!	:: double-colon
//!	: colon
//!	~ complement
//!	-> arrow
//! ( opening-parenthesis
//! ) closing-parenthesis
//! { opening-brace
//! } closing-brace
//TODO: [ opening-bracket
//TODO: ] closing-bracket
//!	, comma
//!	; semicolon
//!	? question-mark</tt>
//!
//! \subsection lang3 Types
//! Unilang defines a few fundamental types. There is only the differentation between floating point and integer types of different precision.
// TODO: The basic integer type is the target dependant int.
//! Size dependant integer types implemented are i64, i32, i16, i8 and i1 commonly known as boolean type.
//! Floating point types supported are f64 and f32.
//!
//! There is no implicit type conversion. Each conversion has to be made explicitly. 
//! Numeric types can be converted into each other but be aware of the precision loss while casting between floating point numbers and the risc of truncation when casting between integers.
//! Integer upcasts from all integer types with more then one bit are always done by using sign extension. Bools are always unsigned so that true == 1 and false == 0.
//! Integer downcasts are done by trunction so there is no implicit mapping from integer values bigger then one to true in the case of a boolean cast!
//! E.g. <tt>bBool:i1{12};</tt> will evaluate to <tt>false</tt>!
//! Condition expressions must have boolean type. If you want C semantics you have to add "!= 0" explicitly.
//!
//! For integral operands the << and >> operators (left respective right shift) are logical shifts. So there is no sign extension. The lhs value is shifted the specified number of bits with zero fill.
//! For integral operands the / operator yields the algebraic quotient with any fractional part discarded. This is often called truncation towards zero.
//! For integral operands the % operators return value is always 0 or positive.
//! For float operands the % operators return value has the same sign as the dividend.
//!
//! \subsection lang4 Variables
//! All data Objects are zero initialised on creation.
//! Every variable is const by default. This means that they are only named values unable to change the underlying data.
//! Numeric values have a default constructor using the 0 value of the given type.
//! E.g.: <tt>my64BitInteger:i64;</tt> defines a constant 64 Bit "0" value. 
//! To make it changeable add the qualifier symbol '~' in front of the typename.
//! E.g.: <tt>my64BitInteger:~i64;</tt>. Now you are able to change the calue in following statements. 
//! To give a special value as construction parameter you can use the initialiser-list syntax. E.g.: <tt>my64BitInteger:i64{42};</tt> defines a constant integer.
//! 
//! \subsection lang5 Functions
//! Functions can be defined like <tt>successor:(i:i64){ret:i+1}->(suc:i64{ret});</tt>. Here we are getting a single 64-bit integer input, compute its successor, save it in a new variable called ret and return this in the return clause as suc.
//! Note that there is no return statement. All values returned have to be given in the return clause. The return clause is executed after the function body completed.
//! The parameter and return list is in the same scope as the function body. 
//! The return clause can include arbitrary expressions so we could have written: <tt>successor:(i:i64)->(suc:i64{i+1});</tt>. This is an inline definition because we did not even need a function body at all.
//! But it can be even shorter. The results of a function can be given names but there is no need to. 
//! The <tt>ret:i64{i+1}</tt> expression is only a standard variable definition that is returning the newly created value. So it also could look like <tt>successor:(i:i64)->(i64{i+1});</tt>
//! The type(s) the function returns are automatically inferred from the expressions written into the return clause.
//! So the shortest way to write the successor function is: <tt>successor:(i:i64)->(i+1);</tt>. TODO: NOT IMPLEMENTED!
//! Functions can have multiple return values. TODO: NOT IMPLEMENTED!
//! Function return expressions are executed from left to right.
//! Function call parameters are evaluated from left to right.
//! Functions can be overloaded on the argument types and their purity state.
//! Calling of a unpure function has to be done explicitly by a <tt>~</tt> following the function name.
//! The following example makes this clear:
//! <tt>func:()->(i64{4})
//! func:~()->(i64{4})// It is unpure, even if it does not make use of it.
//! pure:()
//! {
//!		a = func();		// Calls the pure function. If no pure function defined -> error.
//!		a = func~();	// Error, unpure function call in pure function not allowed.
//! }->(a:i64)
//! unpure:~()
//! {
//!		a = func();		// Calls the pure function. If no pure function defined -> error.
//!		a = func~();	// Calls the unpure function. If no unpure function defined -> error.
//! }->(a:i64)</tt>
//!
//! \subsection lang6 Executable modules
//! The main module needs to have a function with the signature <tt>entrypoint:~()->(i64)</tt> 
//! This means it has to be unpure and have a single i64 errorcode return value. Return 0 if there was no error.
//! This function has to be in the global namespace. It must not be inside a namespace.
//!
//! \subsection lang7 namespaces
//! Namespaces add the possibility to disambiguate between functions with the same identifier.
//! Unnamed namespaces are not supported. '::' is used as the scope resolution operator. 
//! Namespaces can be defined as follows: <tt>namespace:namespace_name1 { /* code */ }</tt>.
//! Namespaces can be nested where you can define one namespace inside another namespace as follows: <tt>namespace:namespace_name1 { /* code1 */ namespace:namespace_name2 { /* code2 */ } }</tt>.
//! You can access members of nested namespaces by using the resultion operator as follows: <tt>namespace_name1::namespace_name2::function();</tt>.
//! Scope resolution is by default done from the current namespace to the global namespace. If you are in the nested namespace <tt>nest1::nest2</tt> and you try to call a function of the last example then the search for the function will be made in three steps.
//! At first in the current namespace <tt>nest1::nest2::namespace_name1::namespace_name2::function</tt> then for <tt>nest1::namespace_name1::namespace_name2::function</tt> and then <tt>namespace_name1::namespace_name2::function</tt>.
//! If the scope resolution operator is used as the namespace prefix the name(space) following it is said to be found from global namespace, such names are called qualified. E.g.: <tt>::nest1::namespace_name2::function();</tt> is searched for only once in the global namespace directly under the given scopes.
//! Discontiguous Namespaces are allowed. A namespace can be defined in several parts and so a namespace is made up of the sum of its separately defined parts. The separate parts of a namespace can be spread over multiple files.
//! Multiple namespaces in different hierarchies can have the same name so <tt>namespace:math { /* code1 */ namespace:math { /* code2 */ } }</tt> and <tt>namespace:math { /* code1 */ namespace:const { /* code2 */ } } namespace:const { /* code3 */ }</tt> are both valid.
//! You can not define members externally into different namespaces like: <tt>namespace:B {} namespace:A { B::f()->() }</tt>
//! 
// $ - The stringify operator
// if you write $ in front of a variable or expression its content is converted to a string. This can be used for Debug purposes.
//!
//-------------------------------------------------------------------------

// forward declarations
//-------------------------------------------------------------------------
//! The llvm namespace.
//-------------------------------------------------------------------------
namespace llvm
{
	class Module;
}

//-------------------------------------------------------------------------
//! The namespace defining the unilang language.
//-------------------------------------------------------------------------
namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining the compiler.
	//! 
	//! To compile a file call compile_file and use the returned module to execute it just-in-time with the execution_engine.
	//-------------------------------------------------------------------------
	namespace compiler
	{
		//-------------------------------------------------------------------------
		//! Defines the verbosity of the debug output.
		//-------------------------------------------------------------------------
		enum EDebugOutputOptions
		{
			Standard	= 0,
			//SourceCode	= 1,
			Unoptimized	= 2,
			Optimized	= 4,
			All			= /*SourceCode |*/ Unoptimized | Optimized,
		};
		
		//-------------------------------------------------------------------------
		//! Reads the source from the given file.
		//!
		//! \param sSourceCodeFilePath The path to the source code file.
		//! \return The source code.
		//-------------------------------------------------------------------------
		U_EXPORT std::string readSourceFromFile( std::string const & sSourceCodeFilePath );

		//-------------------------------------------------------------------------
		//! Compiles the given source.
		//! The returned module will be optimized.
		//!
		//! \param sSourceCode The source code.
		//! \param output The verbosity of the debug output.
		//! \return The llvm::Module being created.
		//-------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> compileSource( std::string const & sSourceCode, EDebugOutputOptions const output = EDebugOutputOptions::Standard );

		//-------------------------------------------------------------------------
		//! Compiles the given file.
		//! The returned module will be optimized.
		//!
		//! \param sSourceCodeFilePath The path to the source code file.
		//! \param output The verbosity of the debug output.
		//! \return The llvm::Module being created.
		//-------------------------------------------------------------------------
		U_EXPORT std::shared_ptr<llvm::Module> compileSourceFromFile( std::string const & sSourceCodeFilePath, EDebugOutputOptions const output = EDebugOutputOptions::Standard );

		//-------------------------------------------------------------------------
		//! Compiles the given files.
		//! The returned modules will be optimized.
		//!
		//! \param vsSourceCodeFilePaths The paths to the source code files.
		//! \param output The verbosity of the debug output.
		//! \return The llvm::Modules being created.
		//-------------------------------------------------------------------------
		U_EXPORT std::vector<std::shared_ptr<llvm::Module>> compileSourcesFromFiles( std::vector<std::string> const & vsSourceCodeFilePaths, EDebugOutputOptions const output = EDebugOutputOptions::Standard );
	}
}