#include <unistd.h>
#include "circularBuffer.h"
#include <unistd.h>

int	main()
{
	buffer_init();
	fork();

	while (read(0, *buf, size_t nbytes))
	{
		if (read == "SINGLE") // strcmp
		{
		}
		else if (read == "PIPED")
		{
			fork();
			waitpid();
		}
		else if (read == "CONCURRENT")
		{
			
		}
		else if (read == "EXIT")
	}
}
