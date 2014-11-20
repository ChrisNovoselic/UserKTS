#include "stdafx.h"

extern int main_common (int , TCHAR* []);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    return main_common (argc, argv);
}
