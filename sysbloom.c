#define _POSIX_C_SOURCE 200809L
#include <sys/utsname.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <sbl.h>
#include <ctype.h>


const char* DIST_SRC   = "/etc/os-release";
const char* UPTIME_SRC = "/proc/uptime";
const char* CPU_SRC    = "/proc/cpuinfo";
const char* MEM_SRC    = "/proc/meminfo";

struct utsname unameData;
char hostname[HOST_NAME_MAX + 1];

void print_seperator();

int main() 
{	
	FILE* dist_fp;
	FILE* uptime_fp;
	FILE* mem_fp;
	FILE* cpu_fp;
	char buf[4096];
	char dist[256];
	char mem_ava_buf[256];
	char mem_tot_buf[256];
	char cpu_buf[256];
	long int mem_tot = 0;
	long int mem_ava = 0;
	int uptime = 0;
	const char* distro_prefix = "PRETTY_NAME=\"";	

	/* Distro read */
	if ((dist_fp = fopen(DIST_SRC, "r")) == NULL) {
		DIE("Error reading distro source");
	}

	/* Mem read */
	if ((mem_fp = fopen(MEM_SRC, "r")) == NULL) {
		DIE("Error reading mem source");
	}

	/* Cpu read */
	if ((cpu_fp = fopen(CPU_SRC, "r")) == NULL) {
		DIE("Error reading cpu source");
	}

	gethostname(hostname, sizeof(hostname));
	char* user = getenv("USER");
	printf("%s@%s\n", user, hostname);
	print_seperator();	

	int c;
	while ((c = fgetc(dist_fp)) != EOF)
	{
	    if (c == '\n') {
		memset(buf, 0, sizeof(buf));
		continue;
	    }

	    if (strcmp(buf, distro_prefix) == 0) {
		if (c == '"') break;

		size_t dist_size = strlen(dist);
		if (dist_size + 1 < sizeof(dist)) {
		    dist[dist_size] = c;
		    dist[dist_size + 1] = '\0';
		}
		continue;
	    }

	    size_t len = strlen(buf);
	    if (len + 1 < sizeof(buf)) {
		buf[len] = c;
		buf[len + 1] = '\0';
	    }
	}	
	memset(buf, 0, sizeof(buf));

	printf("OS:        %s\n", dist);

	uname(&unameData);
	printf("Kernel:   %s\n", unameData.release);

	
	// SHELL GET ENV
	char* shell = getenv("SHELL");
	printf("Shell:    %s\n", shell);


	// TERM GET ENV
	char* term = getenv("TERM");
	printf("Terminal: %s\n", term);

	/* Uptime read */
	if ((uptime_fp = fopen(UPTIME_SRC, "r")) == NULL) {
		DIE("Error reading uptime source");
	}

	
	while ((c = fgetc(uptime_fp)) != EOF)
	{
	    if (isspace(c)) {
	    	uptime = atoi(buf);
		break;
	    }
	    size_t len = strlen(buf);
	    if (len + 1 < sizeof(buf)) {
		buf[len] = c;
		buf[len + 1] = '\0';
	    }
	}	
	memset(buf, 0, sizeof(buf));


	printf("Uptime:   %d minutes\n", (uptime / 60));


	// CPU INFO
	int model_name_found = 0;
	int semi_colon_seen = 0;
	while ((c = fgetc(cpu_fp)) != EOF)
	{
	    if (c == '\n') {
		memset(buf, 0, sizeof(buf));
		if (model_name_found == 0) {
			continue;
		}
		break;
	    }


	    if (strcmp(buf, "model name") == 0) 
	    {
		model_name_found = 1;
		if (semi_colon_seen == 0 && isspace(c)) continue; 		
		if (c == ':') {
			semi_colon_seen = 1;
			continue;
		}
		

		size_t len = strlen(cpu_buf);
		if (len + 1 < sizeof(cpu_buf)) {
			cpu_buf[len] = c;
			cpu_buf[len + 1] = '\0';
	    	}
		continue;
	    }

	    
	    size_t len = strlen(buf);
	    if (len + 1 < sizeof(buf)) {
		buf[len] = c;
		buf[len + 1] = '\0';
	    }
	}	
	memset(buf, 0, sizeof(buf));
	
	char* trimmed = ltrim(cpu_buf);
	strcpy(cpu_buf, trimmed);
	printf("CPU Model: %s\n", cpu_buf);


	// MEM INFO
	while ((c = fgetc(mem_fp)) != EOF)
	{
	    if (c == '\n') {
		memset(buf, 0, sizeof(buf));
		continue;
	    }


	    if (strcmp(buf, "MemTotal:") == 0) {
		if (isspace(c)) continue;
		if (c == 'k' || c == 'b') {
			mem_tot = atoi(mem_tot_buf);
			continue;
		}

		size_t len = strlen(mem_tot_buf);
		if (len + 1 < sizeof(mem_tot_buf)) {
			mem_tot_buf[len] = c;
			mem_tot_buf[len + 1] = '\0';
	    	}
		continue;
	    }

	    if (strcmp(buf, "MemAvailable:") == 0) {
		if (isspace(c)) continue;
		if (c == 'k' || c == 'b') {
			mem_ava = atoi(mem_ava_buf);
			continue;
		}		

		size_t len = strlen(mem_ava_buf);
		if (len + 1 < sizeof(mem_ava_buf)) {
			mem_ava_buf[len] = c;
			mem_ava_buf[len + 1] = '\0';
	    	}

		continue;
	    }

	    size_t len = strlen(buf);
	    if (len + 1 < sizeof(buf)) {
		buf[len] = c;
		buf[len + 1] = '\0';
	    }
	}	
	memset(buf, 0, sizeof(buf));


	printf("Memory Total:  %ld MB\n", mem_tot / 1000 );
	printf("Memory Avaliable:  %ld MB\n", mem_ava / 1000);

	fclose(dist_fp);
	return 0;
}

void print_seperator()
{
	printf("------------------------\n");
}
