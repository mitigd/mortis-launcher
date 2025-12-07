#ifndef VERSION_H
#define VERSION_H

namespace Core {
    struct Version {
        static constexpr int MAJOR = 0;
        static constexpr int MINOR = 2;
        static constexpr int PATCH = 3;
        
        static constexpr const char* STRING = "0.2.3";
    };
}

#endif // VERSION_H