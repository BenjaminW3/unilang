Unilang

unilang is a is a modern-style mixed functional and imperative programming language.
LLVM as a backend is used to JIT-compile programs to fast native code.


Supported Systems

The unilang compiler should be supported by all systems supperted by boost::spirit and llvm.


License

All rights reserved.


Author

Benjamin W.

The main module needs a function with the signature "entrypoint () : ( uint() )"
This means it has to have a single uint errorcode return value.

Every variable is const by default.
To make it changeable add the qualifier symbol '~' in front of the typename