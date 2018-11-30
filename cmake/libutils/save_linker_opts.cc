#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
  //cout << "Got " << argc << " arguments" << endl;
  //cout << "Output file: " << argv[1] << endl;

  ofstream out(argv[1]);

  // Note: argv[2] is the compiler/linker command

  for(unsigned pos=3; pos < argc; pos++)
  {
    if(string(argv[pos]) == "-o")
    {
      pos ++;
      continue;
    }

    //cout << "-- opt: " << argv[pos] << endl;
    out << argv[pos] <<endl;
  }
  out.flush();
  return 0;
}
