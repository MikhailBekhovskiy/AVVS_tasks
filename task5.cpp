#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Found {
public:
    bool isTrue;
    int line;
    int column;

    bool operator==(const Found &other) {
        return isTrue == other.isTrue &&
               line == other.line &&
               column == other.column;
    }
};

Found findPatternSimple(ifstream &infile, string &pattern) {
    string line;
    uint64_t i = 0;
    while(getline(infile, line)) {
        uint64_t found = line.find(pattern);
        if (found != string::npos) {
            return Found{true, i, found};
        }
        ++i;
    }
    return Found{false, 0, 0};
}

Found findPatternPar(ifstream &infile, string &pattern) {
    vector<string> lines;
    string line;
    while (getline(infile, line)) lines.emplace_back(line);
    infile.clear();
    infile.seekg(0, ios::beg);
    Found result{false, 0, 0};

#pragma omp parallel for schedule(auto)
    for (uint64_t i = 0; i < lines.size(); ++i) {
        if (result.isTrue) continue;

        uint64_t found = lines[i].find(pattern);
        if (found != string::npos) {
            #pragma omp critical
            if (!result.isTrue) {
                result = Found{true, i, found};
            }
        }
    }
    return result;
}

int main() {
    ifstream infile("input.txt");

    string pattern = "hello";

    Found res1 = findPatternSimple(infile, pattern);
    infile.seekg(0, infile.beg);
    Found res2 = findPatternPar(infile, pattern);
    if (res1 == res2) {
        if (res1.isTrue) {
            cout << "Found. Positioned at " << res1.line << ":" << res1.column << endl;
        }
        else {
            cout << "Was unable to find the requested substring" << endl;
        }
    }
    else {
        cout << "Function outputs do not match. Some problem occurred." << endl;
        cout << res1.line << ":" << res1.column << endl;
        cout << res2.line << ":" << res2.column << endl;

    }

    return 0;
}