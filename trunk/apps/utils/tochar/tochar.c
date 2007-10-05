/**********************************************************

	tochar - converts numeric value to ascii character
	jeremy hinds

	note: for arguments above 255, high-order bits are
	dropped, giving the automatic effect of mod 256 being
	performed on the arg.  

	example: arg 0x109 produces char 0x09 - tab

	same basically goes for args larger than max value of short.

	example: arg 0x1ffff produces char 0xff
		 arg 0x20009 produces char 0x09

**********************************************************/

#include <stdio.h>
#include <string.h>

void usage(char *);

int main(int argc, char *argv[]){
	short nascii = 0;
	if(argc != 2 || strncmp(argv[1],"-h",2) == 0 ){
		usage(argv[0]);
		return 1;
	}
	sscanf(argv[1],"%hi",&nascii);
	printf("%c",nascii);
	return 0;
}

void usage(char *exe){
	printf("tochar - turns numbers into characters\n");
	printf("usage: %s <number>\n",exe);
	printf("\twhere <number> is the numeric ascii value of the character you want.\n");
	printf("\taccepts decimal (NNN), hexidecimal (0xNN), or octal (0NNN)\n");
	return;
}
