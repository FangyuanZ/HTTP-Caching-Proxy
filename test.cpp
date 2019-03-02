#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string>
 
//以下六个参数改为自己需要的即可
const char *proxyAddr="10.1.1.2";
const int proxyPort = 8080;
const char *user="domain\\name";
const char *passwd="kklklkl";
const char *desthost="www.baidu.com";
const int destport=80;
static void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
 
int main() {
	int sock_fd;
	struct sockaddr_in addr;
	struct hostent *hptr;
	char str[32];
	
	/*	if((hptr = gethostbyname(proxyAddr)) == NULL)
	    {
	        printf(" gethostbyname error for host:%s\n", proxyAddr);
	        return 0; 
	    }
	*/    
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket() fail.");
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(proxyPort);
	inet_aton(proxyAddr, &addr.sin_addr);
	//memcpy(&addr.sin_addr.s_addr,hptr->h_addr,sizeof(addr.sin_addr.s_addr));
	//printf("proxy ip=%s\n",inet_ntop(hptr->h_addrtype,hptr->h_addr,str,sizeof(str)));
	if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(sock_fd);
		perror("connect() fail.");
	}
	printf("-=-=-=-=-=-=-=We have connect the proxy ok!!!!-=-=-=-=-=-=-=\n");
	char tmp[10240+1];
	char authstr[10240+1];
	memset(tmp,0x0,sizeof(tmp));
	memset(authstr,0x0,sizeof(authstr));
	sprintf(tmp,"%s:%s",user,passwd);
	printf("=======%s",tmp);
	to64frombits((unsigned char*)authstr,(unsigned char*)tmp,strlen(tmp));
	printf("--=====%s",authstr);
	snprintf(tmp,sizeof(tmp),"CONNECT %s:%d HTTP/1.0\r\nProxy-Authorization: Basic %s\r\n\r\n", desthost, destport, authstr);
	send(sock_fd, tmp, strlen(tmp), 0);
	printf("-=-=-=-=-=wait for recv-=-=-=-=-=\n");
	
	memset(tmp,0x0,sizeof(tmp));
	recv(sock_fd, tmp, sizeof(tmp), 0);
	printf("recv=%s",tmp);
	//通过代理get，此处的url一定要是全路径带上http://，否则访问不到
	snprintf(tmp,sizeof(tmp),"GET %s HTTP/1.0\r\n", "http://www.baidu.com/");
	strcpy(tmp+strlen(tmp),"Accept:*/*\r\n");
	strcpy(tmp+strlen(tmp),"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.6)");
	strcpy(tmp+strlen(tmp),"Accept-Language:zh-cn\r\n");
	strcpy(tmp+strlen(tmp),"Connection:close\r\n\r\n");
	send(sock_fd, tmp, strlen(tmp), 0);
	memset(tmp,0x0,sizeof(tmp));
	sleep(5);
	recv(sock_fd, tmp, sizeof(tmp), 0);
	printf("recv2=%s",tmp);
	close(sock_fd);
	return 0;
}
 
//base64编码函数
static void to64frombits(unsigned char *out, const unsigned char *in, int inlen)
{
	const char base64digits[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	for (; inlen >= 3; inlen -= 3)
	{
		*out++ = base64digits[in[0] >> 2];
		*out++ = base64digits[((in[0] << 4) & 0x30) | (in[1] >> 4)];
		*out++ = base64digits[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
		*out++ = base64digits[in[2] & 0x3f];
		in += 3;
	}
	if (inlen > 0)
	{
		unsigned char fragment;
		*out++ = base64digits[in[0] >> 2];
		fragment = (in[0] << 4) & 0x30;
		if (inlen > 1)
		fragment |= in[1] >> 4;
		*out++ = base64digits[fragment];
		*out++ = (inlen < 2) ? '=' : base64digits[(in[1] << 2) & 0x3c];
		*out++ = '=';
	}
	*out = '\0';
}

