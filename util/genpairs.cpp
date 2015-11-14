#include <fstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " count file-name\n";
    return -1;
  }

  srand(time(NULL));

  size_t count = atoi(argv[1]);
  ofstream out(argv[2]);

  vector<int> keys(count);
  out << count << endl;

  size_t i = 0;

  while (i < count) {
    int key = rand();
    if (find(keys.begin(), keys.end(), key) != keys.end()) {
      continue;
    }

    int val = rand();

    out << key << "\t" << val << endl;

    i++;
  }

  return 0;
}
