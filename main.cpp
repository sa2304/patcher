#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

using Offset = uint32_t;
using Patch = map<Offset, vector<char>>;

int main(int argc, char **argv) {
  if (argc < 4) {
    cerr << "Usage "s << argv[0] << " <filename> <patchname> <outfile>"s << endl;
    return -1;
  }

  string filename{argv[1]}, patchname{argv[2]}, outfilename{argv[3]};
  ifstream file{filename, ios::binary | ios::ate};
  if (!file.is_open()) {
    cerr << "Can't read "s << quoted(filename) << endl;
    return -1;
  }
  ifstream patch{patchname};
  if (!patch.is_open()) {
    cerr << "Can't read "s << quoted(patchname) << endl;
    return -1;
  }
  ofstream outfile{outfilename, ios::binary | ios::trunc};
  if (!outfile.is_open()) {
    cerr << "Can't write "s << quoted(outfilename) << endl;
    return -1;
  }

  auto size = file.tellg();
  vector<char> data(size);
  file.seekg(0);
  file.read(data.data(), size);

  Patch p;
  while (patch) {
    string line;
    getline(patch, line);
    char *ptr = line.data();
    Offset offset = strtol(ptr, &ptr, 16);
    if (0 < offset) {
      int val;
      while (ptr < line.data() + line.size()) {
        val = strtol(ptr, &ptr, 16);
        p[offset].push_back(val);
      }
    }
  }
  size_t bytes_patched = 0;
  for (const auto&[offset, bytes]: p) {
    for (char c: bytes) {
      ++bytes_patched;
    }
    copy(bytes.begin(), bytes.end(), next(data.begin(), offset));
  }

  outfile.write(data.data(), size);
  cout << dec << size << " bytes written, "s << bytes_patched << " bytes patched"s << endl;

  return 0;
}
