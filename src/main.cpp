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

argparse::ArgumentParser program("svf_frontend");

int main(int argc, char *argv[]) {
  
  // Parse command line arguments
  parse_command_line(program, argc, argv);

   // Read C++ code from file
  std::string *source_code = read_file(program.get<std::string>("filename"));
  if (source_code == nullptr) {
    return 1;
  }

  // Create a new parser
  TSParser *parser = ts_parser_new();
  // Get the language from the parser
  const TSLanguage *language = tree_sitter_c();
  ts_parser_set_language(parser, language);

  const char * source_code_ptr = source_code->c_str();
  TSTree *tree = ts_parser_parse_string(parser, nullptr, source_code_ptr, strlen(source_code_ptr));

  if (program["--output-cst"] == true) {
    write_cst_to_file("cst.dot", tree);
  }

  // Get the root node of the syntax tree
  TSNode root_node = ts_tree_root_node(tree);


  ASTBuilder ast_builder(source_code, language);
  Ir* ast_root = ast_builder.build(root_node);

  if (program["--verbose"] == true) {
    std::cout << "\n======== AST:\n" << ast_root->prettyPrint("") << std::endl;
    std::cout << "\n==== AST toString():\n" << ast_root->toString() << std::endl;
  }
  IrTransUnit* unit = dynamic_cast<IrTransUnit*>(ast_root);
  std::cout << "\n==== llBuilder:\n" << std::endl;
  unit->getLlBuilder();

  // Clean up
  delete ast_root;


  std::cout << "\n======== Src:" << std::endl;
  std::cout << *source_code << std::endl;


  // Clean up
  ts_tree_delete(tree);
  ts_parser_delete(parser);

  return 0;
}
