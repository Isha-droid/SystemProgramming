#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <utility>

using namespace std;

const int BASE_ADDRESS = 100;
const int INT_SIZE = sizeof(int);
const int FLOAT_SIZE = sizeof(float);
const int DOUBLE_SIZE = sizeof(double);
const int CHAR_SIZE = sizeof(char);

pair<string, int> parseVariable(const string& varname) {
    size_t pos = varname.find('[');
    if (pos != string::npos) {
        string baseName = varname.substr(0, pos);
        int size = stoi(varname.substr(pos + 1, varname.length() - pos - 2));
        return make_pair(baseName, size);
    } else {
        return make_pair(varname, 1); 
    }
}

void populateSymbolTable(const string& filename, unordered_map<string, pair<int, string>>& symbolTable) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int currentAddress = BASE_ADDRESS;

    while (getline(file, line)) {
        istringstream iss(line);
        string datatype, varname;
        iss >> datatype >> varname;

        pair<string, int> parsedVar = parseVariable(varname);
        string baseName = parsedVar.first;
        int arraySize = parsedVar.second;

        int bytes = 0;
        if (datatype == "int") {
            bytes = INT_SIZE;
        } else if (datatype == "float") {
            bytes = FLOAT_SIZE;
        } else if (datatype == "double") {
            bytes = DOUBLE_SIZE;
        } else if (datatype == "char") {
            bytes = CHAR_SIZE;
        }

        for (int i = 0; i < arraySize; ++i) {
            string arrayElementName = baseName + (arraySize > 1 ? "[" + to_string(i) + "]" : "");
            symbolTable[arrayElementName] = make_pair(bytes, "0x" + to_string(currentAddress));
            currentAddress += bytes;
        }
    }

    file.close();
}

int main() {
    string filename = "variables.txt"; 
    unordered_map<string, pair<int, string>> symbolTable;

    populateSymbolTable(filename, symbolTable);

    string userInput;
    while (true) {
        cout << "Enter variable name to fetch information (type 'exit' to quit): ";
        cin >> userInput;

        if (userInput == "exit") {
            break;
        }

        if (symbolTable.find(userInput) != symbolTable.end()) {
            pair<int, string> varInfo = symbolTable[userInput];
            cout << "Variable: " << userInput << ", Bytes: " << varInfo.first << ", Address: " << varInfo.second << endl;
        } else {
            cout << "Variable '" << userInput << "' not found in symbol table." << endl;
        }
    }

    return 0;
}
