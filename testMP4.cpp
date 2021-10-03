#include "MP4.cpp"
#include <iostream>
#include <iomanip>
#include <chrono>


int main(int argc, char* argv[])
{
    if ( argc == 1 ) return 1;
    
    auto createStart = std::chrono::high_resolution_clock::now();

    std::cout << "**** CREATE MP4 ****\n\n";

    MP4::MP4 MP4(argv[1]);

    auto testStart = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(testStart - createStart);
    std::cout << "\nCreate Time: " << ms_int.count() << "ms\n";
    std::cout << "**** TEST MP4 ****\n\n";
    
    MP4.printHierarchy();
    MP4.printHierarchyData();

    if (false) {
    }

    auto end = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - testStart);
    std::cout << "\nTest Time : " << ms_int.count() << "ms\n";
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - createStart);
    std::cout << "Total Time: " << ms_int.count() << "ms\n";
    std::cout << "**** END ****\n\n";

    return 0;
}