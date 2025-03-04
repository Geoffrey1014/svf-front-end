# SVF-Frontend
This is the frontend of svf.

## Install
1. install gcc, g++, cmake
2. git clone https://github.com/p-ranav/argparse.git
3. git clone https://github.com/tree-sitter/tree-sitter; cd tree-sitter; make; cd ..
4. git clone https://github.com/tree-sitter/tree-sitter-c
5. mkdir build; cd build; cmake ..
6. CST dot file: ./svf_frontend file-path -c
7. dot file to the png file: dot -Tpng cst.dot -o cst.png; dot -Tpng cfg2.dot -o cfg2.png
