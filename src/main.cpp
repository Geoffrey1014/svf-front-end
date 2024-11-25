#include <tree_sitter/api.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include "utils.h"
#include <memory>
#include "ASTBuilder.h"

// Include the C++ parser header
extern "C" const TSLanguage *tree_sitter_cpp();


int main(int argc, char *argv[]) {
  
  // Parse command line arguments
  std::string filename = parse_command_line(argc, argv);


  // Create a new parser
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cpp());

   // Read C++ code from file
  std::string *source = read_file(filename);
  if (source == nullptr) {
    return 1;
  }

  const char * source_code = source->c_str();
  TSTree *tree = ts_parser_parse_string(parser, nullptr, source_code, strlen(source_code));

  write_cst_to_file("cst.dot", tree);

  // Get the root node of the syntax tree
  TSNode root_node = ts_tree_root_node(tree);

  // Print the root node type
  // std::cout << "Root node type: " << ts_node_type(root_node) << std::endl;

  // Print the syntax tree as an S-expression.
  char *string = ts_node_string(root_node);
  // printf("Syntax tree: %s\n", string);


  ASTBuilder ast_builder(source);
  ast_builder.build(root_node);

  std::cout << "\n======== Src:" << std::endl;
  std::cout << *source << std::endl;


  // Clean up
  ts_tree_delete(tree);
  ts_parser_delete(parser);

  return 0;
}
