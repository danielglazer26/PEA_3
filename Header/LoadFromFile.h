#ifndef ETAP2_LOADFROMFILE_H
#define ETAP2_LOADFROMFILE_H


#include <fstream>

class LoadFromFile {
public:
    bool openFile();

    int getDataFromFile();

    ~LoadFromFile() {
        file.close();
    }
    bool openFileName(const std::string& nameOfFile);

private:

    std::fstream file;
};


#endif //ETAP2_LOADFROMFILE_H
