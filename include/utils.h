#ifndef UTILS_H
#define UTILS_H
#include <argparse/argparse.hpp>

// Function to parse command line arguments using argparse
void parse_command_line(argparse::ArgumentParser& program, int argc, char *argv[]) {

  program.add_argument("filename")
    .help("The name of the file to read");

  program.add_argument("-v", "--verbose")
  .help("increase output verbosity")
  .default_value(false)
  .implicit_value(true);

  program.add_argument("-c", "--output-cst")
  .help("specify the output file for CST.")
  .default_value(false)
  .implicit_value(true);

  program.add_argument("-i", "--intermedial")
  .help("specify the output file for intermedial code.")
  .default_value(false)
  .implicit_value(true);

  program.add_argument("-o", "--output")
  .default_value(std::string("-"))
  .required()
  .help("specify the output file.");


  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    exit(1);
  }

  return;
}

// write cst to file
void write_cst_to_file(const char *filename, TSTree *tree) {

  // Open the file and get the file descriptor
    int file_descriptor = open(filename, O_WRONLY | O_CREAT, 0644);
    if (file_descriptor == -1) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }


  ts_tree_print_dot_graph(tree, file_descriptor);
  close(file_descriptor);
}

// read cpp file function
std::string* read_file(const std::string &filename) {
  std::cout << "Reading file: " << filename << std::endl;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return nullptr;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return new std::string(buffer.str());
}
#endif