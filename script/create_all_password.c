#include <stdio.h>

int		main(void)
{
	char *phase_1[] = { "Publicspeakingisveryeasy." };
	char *phase_2[] = { "12624120720" };
	char *phase_3[] = { "1b214", "2b755", "7b524" };
	char *phase_4[] = { "9" };
	char *phase_5[] = { "opekma", "opekmq", "opukma", "opukmq" };
	char *phase_6[] = { "426135" };

	for (int i = 0 ; i < 3 ; ++i)
		for (int j = 0 ; j < 4 ; ++j)
			printf("%s%s%s%s%s%s\n", phase_1[0], phase_2[0], phase_3[i], phase_4[0], phase_5[j], phase_6[0]);
	return (0);
}
