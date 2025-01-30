#include <string>

unsigned int compileShader(const char* source, unsigned int type);
unsigned int createRenderProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int createComputeProgram(unsigned int computeShader);