#include "example.h"
#include <iostream>
#define iter 10

int main()
{

	int size = 512;
	float in[size*iter];
	float a[size];

	float out[size*iter];

	for(int i=0;i<size;i++)
	{
		in[i]= i;
		a[i] =i;
	}

	transfer_kernel(in,out,size);

	int var ;
	for(int j=0; j<iter;j++)
	{
	for(int i=0;i<size;i++)
	{
		if(out[i] != a[i] )
        {
        std::cout <<"design fails" << " i " << i << " out "<< out[i] << " a " << a[i] << std::endl;
			return 1;
	
        }
    }
	}
	return 0;

}
