#include "header.h"
#include "task_receive.h"

int read_n(int fd, char* buffer, int n)
{
    int    nleft;
    int    nread;
    char   *ptr;
	fd_set rset;
	int maxfd, nready;
	struct timeval timeout;

	ptr = buffer;
    nleft = n;
	while(nleft > 0) 
	{
		errno = 0;

		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		maxfd = fd + 1;
		timeout.tv_sec = g_read_timeout;
		timeout.tv_usec = 0;
		nready = select(maxfd, &rset, NULL, NULL, &timeout);

		if(nready < 0)
		{
			return -1;
		}
		else if(nready == 0)
		{
			return 0;
		}
		else
		{
			nread = read(fd, buffer, nleft);
			if(nread < 0) 
			{
				if(errno == EINTR) nread = 0;
				else               
				{
						return -1;
				}

			}
			else if(nread == 0) break;
			nleft -= nread;
			ptr   += nread;
		}
	}
	return(n - nleft);
}

int write_n(int fd, char* buffer, int n) 
{
	int nleft;
	int nwritten;
	char*  ptr;
	fd_set wset;
	int maxfd, nready;
	struct timeval timeout;

	ptr = buffer;
	nleft = n;
	while(nleft > 0) 
	{
		errno = 0;
		FD_ZERO(&wset);
		FD_SET(fd, &wset);
		maxfd = fd + 1;
		timeout.tv_sec = g_write_timeout;
		timeout.tv_usec = 0;
		nready = select(maxfd, NULL, &wset, NULL, &timeout);
		if(nready < 0)
		{
			return -1;
		}
		else if(nready == 0)
		{
			return 0;
		}
		else
		{
			nwritten = write(fd, buffer, nleft);
			if(nwritten <= 0) 
			{
				if(errno == EINTR) 
				{
					nwritten = 0;
				}
				else   return -1;
			}
			if(errno == EPIPE) return -1;
			nleft -= nwritten;
			ptr   += nwritten;
		}  
	}
	return (n);
}

