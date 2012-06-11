Unilang

unilang is a is a modern-style mixed functional and imperative programming language.
LLVM as a backend is used to JIT-compile programs to fast native code.


Supported Systems

The unilang compiler should be supported by all systems supperted by boost::spirit and llvm 3.0.


License

All rights reserved.


Author

Benjamin W.

During design of the language one essential rule was that whitespace should never have any semantic.
This means that there can not be types with multiple words like "unsigned long long" in c++.
Separation of identifiers and other things must be done with seperation characters like ':',';',',','|' etc...
There are also no short forms of if, for, etc. due to this rule. the {} have to be given explicitly.

The main module needs a function with the signature "<() -> ( uint() )> entrypoint"
This means it has to have a single uint errorcode return value.

Every variable is const by default.
To make it changeable add the qualifier symbol '~' in front of the typename

$ - The stringify operator
if you write $ in front of a variable or expression its content is converted to a string. This can be used for Debug purposes.