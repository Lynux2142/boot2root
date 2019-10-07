#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int string_length(char *param_1)

{
  char cVar1;
  int iVar2;

  iVar2 = 0;
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    param_1 = param_1 + 1;
    iVar2 = iVar2 + 1;
    cVar1 = *param_1;
  }
  return iVar2;
}

int strings_not_equal(char *param_1,char *param_2)
{
  char cVar1;
  int iVar2;
  int iVar3;
  int local_EAX_39;
  
  iVar2 = string_length(param_1);
  iVar3 = string_length(param_2);
  if (iVar2 == iVar3) {
    cVar1 = *param_1;
    while (cVar1 != '\0') {
      if (*param_1 != *param_2) goto LAB_08049057;
      param_1 = param_1 + 1;
      param_2 = param_2 + 1;
      cVar1 = *param_1;
    }
    local_EAX_39 = 0;
  }
  else {
LAB_08049057:
    local_EAX_39 = 1;
  }
  return local_EAX_39;
}

int phase_5(char *param_1)
{
  const char *c = "isrveawhobpnutfg";
  int iVar1;
  char local_c [7];

  iVar1 = string_length(param_1);
  if (iVar1 != 6)
    return (0);
  iVar1 = 0;
  do {
    local_c[iVar1] = c[(char)(param_1[iVar1] & 0xf)];
    iVar1 = iVar1 + 1;
  } while (iVar1 < 6);
  //printf("%s\n", local_c);
  local_c[6] = '\0';
  iVar1 = strings_not_equal(local_c,"giants");
  if (iVar1 != 0)
    return (0);
  else
    return (1);
}

int	main(void)
{
	char str[7];
	int ok = 1;
	const int begin = 2;

	for (int a = begin ; a < 8 ; ++a)
		for (int b = begin ; b < 8 ; ++b)
			for (int c = begin ; c < 8 ; ++c)
				for (int d = begin ; d < 8 ; ++d)
					for (int e = begin ; e < 8 ; ++e)
					{
						sprintf(str, "o%c%c%c%c%c", 0 | (a << 4), 5 | (b << 4), 0xb | (c << 4), 0xd | (d << 4), 0x1 | (e << 4));
						for (int x = 0 ; x < 6 ; ++x)
//							if (!isalnum(str[x]))
//							if (!isdigit(str[x]) && !islower(str[x]))
							if (!islower(str[x]))
							{
								ok = 0;
								break ;
							}
						if (ok)
							printf("%s\n", str);
						ok = 1;
					}
	return (0);
}
