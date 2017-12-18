#ifndef CPP2JSON_HPP
#define CPP2JSON_HPP

#define CPP2JSON_EXCLUDE_TAG "cpp2json_exclude"
#define CPP2JSON_EXCLUDE __attribute__((annotate(CPP2JSON_EXCLUDE_TAG)))
#define CPP2JSON_ARRAY( size ) __attribute__((annotate("cpp2json_array(" + #size + ")")))

#endif // CPP2JSON_HPP
