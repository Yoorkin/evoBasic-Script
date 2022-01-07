//
// Created by yorkin on 1/7/22.
//

#ifndef EVOBASIC_LOADER_H
#define EVOBASIC_LOADER_H
#include <list>
#include <string>

namespace evoBasic{

    namespace il{
        class Document;
    }

    class Context;

    class Loader {
        std::list<il::Document*> packages;
        std::list<std::string> waiting_package;
    public:
        void addToWaitingDeque(std::string package_path);
        void loadPackages();
        void loadPackagesSymbols(Context *context);
    };

}

#endif //EVOBASIC_LOADER_H
