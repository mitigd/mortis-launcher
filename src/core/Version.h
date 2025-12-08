#ifndef VERSION_H
#define VERSION_H

namespace Core {
    struct Version {
        static constexpr int MAJOR = 0;
        static constexpr int MINOR = 2;
        static constexpr int PATCH = 5;
        
        static constexpr const char* STRING = "0.2.5";
    };
} // namespace Core

#endif // VERSION_H