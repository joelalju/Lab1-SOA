#include <libc.h>

char buff[24];

int pid;

long inner(long n) {
	int i;
	long suma;
	suma = 0;
	for (i = 0; i<n; i++) suma = suma + i;
	return suma;	
}
long outer (long n) {
	int i;
	long acum;
	acum = 0;
	for (i=0;i<n;i++) acum = acum + inner(i);
	return acum;
}

int add (int par1, int par2) {
	int par = par1+par2;
	return par;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	int i = 0;
	while(1) {
		if (i%10000000 == 0){
			//char text[] = "probando, probando, 1, 2 ,3, probando";
			//write(1, text, sizeof(text));
			/*char timeBuffer[512];
			int time = gettime();
			itoa(time, timeBuffer);
			write(1, timeBuffer, strlen(timeBuffer));
			perror();
			write (1, "\n", 1);
			perror();
			write(56,"Soy un error",1);
			perror();*/
			char textBuffer[] = "\nProcess pid: ";
			write(1, textBuffer, strlen(textBuffer));

			int pid = getpid();
			char pidBuffer[512];
			itoa(pid, pidBuffer);
			write(1, pidBuffer, strlen(pidBuffer));

		}
		i++;
	}
	return 0;
}
