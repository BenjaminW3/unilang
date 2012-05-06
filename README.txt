The main module needs a function with the signature "entrypoint () : ( uint() )"
This means it has to have a single uint errorcode return value.

Every variable is const by default.
To make it changeable add the qualifier symbol '~' in front of the typename