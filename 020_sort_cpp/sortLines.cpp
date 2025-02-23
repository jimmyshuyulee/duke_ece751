#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void read_input(istream & stream) {
  string line;
  vector<string> input_lines;
  while (getline(stream, line)) {
    input_lines.push_back(line);
  }

  sort(input_lines.begin(), input_lines.end());

  for (int i = 0; i < input_lines.size(); ++i) {
    cout << input_lines[i] << endl;
  }
}

int read_file(string filename) {
  ifstream ifs(filename);
  if (ifs.is_open()) {
    read_input(ifs);
    ifs.close();
  }
  else {
    cerr << "Cannot open the file." << endl;
    return 0;
  }
  return 1;
}

int main(int argc, char ** argv) {
  if (argc == 1) {
    read_input(cin);
  }
  else {
    for (int i = 1; i < argc; ++i) {
      int success = read_file(argv[i]);
      if (!success) {
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}
