#ifndef CPP2JSON_HPP
#define CPP2JSON_HPP

#define CPP2JSON_EXCLUDE __attribute__((annotate("cpp2json_exclude_definition")))
#define CPP2JSON_ARRAY( size ) __attribute__((annotate("cpp2json_considere_as_array(" + #size + ")")))

#endif // CPP2JSON_HPP
