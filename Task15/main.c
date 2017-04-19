#include <stdio.h>

int main(int argc, char *argv[])
{
	int id_h = 0x682c;
        int id_l = 0x83e55b77;
        long error;

        error = syscall(333, id_h, id_l);
        if (error) {
                printf("syscall error=%d\n", error);
                return 1;
        }

        id_l += 1;
        error = syscall(333, id_h, id_l);
        if (error) {
                printf("syscall error=%d\n", error);
                return 1;
        }

	return 0;
}
