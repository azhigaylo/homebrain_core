#include <iostream>
#include "utils.h"
#include "IxRunnable.h"

using namespace std;
  
int main()
{
    IxRunnable task_1("task_1");

	cout << "IxRunnable test !\n";

    task_1.task_run();
	
	sleep_s( 2 );  
	
    return 0;
}