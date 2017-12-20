#ifndef CPP2JSON_HPP
#define CPP2JSON_HPP

#define CPP2JSON_XSTRING(x) CPP2JSON_STRING(x)
#define CPP2JSON_STRING(x) #x

#define CPP2JSON_EXCLUDE __attribute__((annotate("cpp2json_exclude_definition")))
#define CPP2JSON_ARRAY( size ) __attribute__( (annotate(CPP2JSON_STRING(cpp2json_array#size))) )

#endif // CPP2JSON_HPP
