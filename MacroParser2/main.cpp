#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Definition table to hold the definition of the macro
struct DefinitionTable {
    int index;
    std::string definition;
    DefinitionTable* next;
};

// Name table contains the name of the macro and its corresponding index in the Definition table
struct NameTable {
    int index;
    std::string name;
    DefinitionTable* dtIndex;
    NameTable* next;
};

// Initializing the table pointers
DefinitionTable* dtHead = nullptr;
NameTable* ntHead = nullptr;

// Counters for Definition Table (MDTC) and Name Table (MNTC)
int MDTC = 1, MNTC = 1;

DefinitionTable* findName(const std::string& name) {
    NameTable* temp = ntHead;
    while (temp != nullptr) {
        if (temp->name == name) {
            return temp->dtIndex;
        }
        temp = temp->next;
    }
    return nullptr;
}

void pass1(std::ifstream &file) {
    std::string line;
    NameTable* ntTemp = ntHead;
    DefinitionTable* dtTemp = dtHead;

    while (getline(file, line)) {
        if (line.find("MACRO") != std::string::npos) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;  // Skip "MACRO" keyword
            iss >> token;  // Macro name

            // Add to Name Table
            if (ntHead == nullptr) {
                ntHead = new NameTable{MNTC++, token, nullptr, nullptr};
                ntTemp = ntHead;
            } else {
                ntTemp->next = new NameTable{MNTC++, token, nullptr, nullptr};
                ntTemp = ntTemp->next;
            }

            // Add to Definition Table
            if (dtHead == nullptr) {
                dtHead = new DefinitionTable{MDTC++, token, nullptr};
                dtTemp = dtHead;
            } else {
                dtTemp->next = new DefinitionTable{MDTC++, token, nullptr};
                dtTemp = dtTemp->next;
            }
            ntTemp->dtIndex = dtTemp;

            // Read the macro body until "MEND"
            while (getline(file, line) && line.find("MEND") == std::string::npos) {
                dtTemp->next = new DefinitionTable{MDTC++, line, nullptr};
                dtTemp = dtTemp->next;
            }
        }
    }
}

void pass2(std::ifstream &file) {
    std::ofstream output("output.txt");
    std::string line;

    while (getline(file, line)) {
        DefinitionTable* temp = findName(line);
        if (temp != nullptr) {
            temp = temp->next;  // Skip the macro name itself
            while (temp != nullptr && temp->definition != "MEND") {
                output << temp->definition << std::endl;
                temp = temp->next;
            }
        } else {
            output << line << std::endl;  // Write non-macro lines directly to output
        }
    }
}

int main() {
    std::ifstream inputFile("input.asm");
    if (!inputFile) {
        std::cerr << "Failed to open the assembly file!" << std::endl;
        return 1;
    }

    std::cout << "Pass 1 in progress" << std::endl;
    pass1(inputFile);
    inputFile.clear();  // Clear EOF flag
    inputFile.seekg(0);  // Reset file pointer to the beginning

    std::cout << "Pass 2 in progress" << std::endl;
    pass2(inputFile);
    
    inputFile.close();
    std::cout << "Processing complete!" << std::endl;

    return 0;
}
