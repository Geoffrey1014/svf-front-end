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

// Include the C parser header
extern "C" const TSLanguage *tree_sitter_c();


int main(int argc, char *argv[]) {
  
  // Parse command line arguments
  argparse::ArgumentParser program("svf_frontend");
  parse_command_line(program, argc, argv);

   // Read C++ code from file
  std::string *source = read_file(program.get<std::string>("filename"));
  if (source == nullptr) {
    return 1;
  }

  // Create a new parser
  TSParser *parser = ts_parser_new();
  // Get the language from the parser
  const TSLanguage *language = tree_sitter_c();
  ts_parser_set_language(parser, language);

  const char * source_code = source->c_str();
  TSTree *tree = ts_parser_parse_string(parser, nullptr, source_code, strlen(source_code));

  // print s-expression
  // std::cout << ts_node_string(ts_tree_root_node(tree)) << std::endl << std::endl; 

  if (program["--output-cst"] == true) {
    write_cst_to_file("cst.dot", tree);
  }

  // Get the root node of the syntax tree
  TSNode root_node = ts_tree_root_node(tree);


  ASTBuilder ast_builder(source, language);
  ast_builder.build(root_node);

  std::cout << "\n======== Src:" << std::endl;
  std::cout << *source << std::endl;


  // Clean up
  ts_tree_delete(tree);
  ts_parser_delete(parser);

  return 0;
}
