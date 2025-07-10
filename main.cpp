#define LOWP
#include "calculator.h"
#include "test.h"

int main(int argc, char** argv)
{
    if(!ExecuteTestCases())
    {
        std::cerr << "An error occurred...\n";
        return 1;
    }
    g_mapUserDefinedConstants.clear();
    PrintInstructions();
    return 0;
}