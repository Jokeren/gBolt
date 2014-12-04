#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define TAG '8'

int main()
{
	char filename[100], line[1000];
	scanf("%s",filename);
	FILE *in = fopen(filename, "r");
	if (in==NULL)
		exit(-1);
	int count = 0, flag = 0;
	while (1)
	{
		fgets(line,999,in);
		if(feof(in))
			break;

		if(line[0]=='t') {
			flag = 0;
		} else if(line[0]=='v' && flag==0) {
			int line_len = strlen(line);
			if(line[line_len-3]== '8' && isspace(line[line_len-4]))
			{
				++count;
				flag = 1;
			}
		}
	}
	printf("%d\n",count);
	return 0;
}
