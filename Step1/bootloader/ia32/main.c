/*
		This program is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program.  If not, see <http://www.gnu.org/licenses/>.

		Author: Olivier Gruber (olivier dot gruber at acm dot org)
 */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define COM1 ((uint16_t)0x3f8)
#define COM2 ((uint16_t)0x2f8)
#define MAXCMDHISTORY 10
#define SIZELINE 81

static __inline __attribute__((always_inline, no_instrument_function))
uint8_t inb(uint16_t port) {
	uint8_t data;
	__asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline __attribute__((always_inline, no_instrument_function))
void outb(uint16_t port, uint8_t data) {
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}


static void serial_init(uint16_t port) {

	outb(port+1,0x00);    // Disable all interrupts
	outb(port+3,0x80);    // Enable DLAB (set baud rate divisor)

	outb(port+0,0x01);  // Set divisor to 1 (lo byte) 115200 baud
	outb(port+1,0x00);  //                0 (hi byte)

	outb(port+3,0x03);    // 8 bits, no parity, one stop bit
	outb(port+2,/*0xC7*/ 0x00);    // Enable FIFO, clear them, with 14-byte threshold
	outb(port+4,/*0x0B*/ 0x08);    // IRQs enabled, RTS/DSR set

	// outb(port+1,0x0d); // enable all intrs but writes
}



static
void serial_write_char(uint16_t port, char c) {
	while ((inb(port + 5) & 0x20) == 0);
	outb(port,c);
}

static
void serial_write_string(uint16_t port, const unsigned char *s) {
	while(*s != '\0') {
		serial_write_char(port,*s);
		s++;
	}
}

char serial_read(uint16_t port) {
	while ((inb(port + 5) & 1) == 0);
	return inb(port);
}

char * itoa( int value, char * str, int base )
{
	char * rc;
	char * ptr;
	char * low;
	// Check for supported base.
	if ( base < 2 || base > 36 )
	{
		*str = '\0';
		return str;
	}
	rc = ptr = str;
	// Set '-' for negative decimals.
	if ( value < 0 && base == 10 )
	{
		*ptr++ = '-';
	}
	// Remember where the numbers start.
	low = ptr;
	// The actual conversion.
	do
	{
		// Modulo is negative for negative value. This trick makes abs() unnecessary.
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while ( value );
	// Terminating the string.
	*ptr-- = '\0';
	// Invert the numbers.
	while ( low < ptr )
	{
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

/*
 * See:
 *   http://wiki.osdev.org/Printing_To_Screen
 *   http://wiki.osdev.org/Text_UI
 *
 * Note this example will always write to the top
 * line of the screen.
 * Note it assumes a color screen 0xB8000.
 * It also assumes the screen is in text mode,
 * and page-0 is displayed.
 * Screen is 80x25 (80 characters, 25 lines).
 *
 * For more info:
 *   http://wiki.osdev.org/VGA_Resources
 */
void write_string( int colour, const char *string, char *x, char y)
{
	volatile char *video = (volatile char*)0xB8000;

	char *pointeur = video + 2*(*x) + 160*y;

	//	Caractère ASCII affichable
	while( *string != 0 )
	{
		*pointeur = *string++;
		pointeur++;
		*pointeur = colour;
		pointeur++;
		*x+=1;
	}

}


void onCharReceive(unsigned char c)
{

	static int nbcommande=0, commandeactuelle=0, currentline =0;
	static char commande[MAXCMDHISTORY][SIZELINE];
	static char x =0, y = 0, position = 0;
	char *str[2];
	char stk;
	str[0]=c;
	str[1]='\0';
	int index=0;
	int jindex=0;

	if (c>31 && c<127)
	{
		commande[commandeactuelle][x] = c;
		write_string(0x0F, str, &x,y);
		serial_write_char(COM1,c);
	}
	else
	{
		switch (c)
		{
		case 27 :
			stk = serial_read(COM1);
			if (stk =='[')
			{
				stk = serial_read(COM1);
				switch (stk)
				{
				//flèche du haut
				case 'A' :

					if (currentline > 0)
					{
						currentline--;
						serial_write_char(COM1,'\r');
						x = 0;
						write_string(0x0F, commande[currentline],&x,y);
						x = 0;
					}
					break;

					//flèche du bas
				case 'B' :

					if (currentline < nbcommande)
					{
						currentline++;
						serial_write_char(COM1,'\r');
						x = 0;
						write_string(0x0F, commande[currentline],&x,y);
						x = 0;
					}
					break;
					//flèche de droite
				case 'C' :
					x++;
					if (x >= 79)
						x=79;
					break;
					//flèche de gauche
				case 'D' :
					x--;
					if (x <= 0)
						x=0;
					break;
					//touche supr
				case '3' :
					stk = serial_read(COM1);
					if (stk == 126)
					{
						position = x;
						while (x < 80)
						{
							commande[commandeactuelle][x] = commande[commandeactuelle][x+1];
							x++;
						}

						commande[commandeactuelle][x] ='\0';
						x=0;
						write_string(0x0F, commande[commandeactuelle],&x,y);
						x = position;
						position = 0;
						break ;
					}
					else
					{
						write_string(0x0F, stk,&x,y);
					}
				}
			}
			break;

			//fin de ligne
		case '\r' :
		case '\n' :
			for (;x<SIZELINE-1;x++)
				commande[commandeactuelle][x]=' ';

			commande[commandeactuelle][x] = '\0';
			x = 0;
			if (nbcommande < MAXCMDHISTORY-1)
				nbcommande++;
			///on décale l'historique des commandes vers le haut
			else
			{

				while (index<(MAXCMDHISTORY))
				{
					while (jindex<SIZELINE-1)
					{
						commande[index][jindex]=commande[index+1][jindex];
						jindex++;
					}
					index++;
				}
			}

			currentline = commandeactuelle = nbcommande;
			y++;
			serial_write_char(COM1,'\r');
			serial_write_char(COM1,'\n');
			serial_write_char(COM1,'>');

			commande[commandeactuelle][x] = '>';
			write_string(0x0F,">",&x,y);
			break ;

			//supression d'un caractère
		case 127 :
			if (x!=0)
			{
				x--;

				while (x < SIZELINE-1)
				{
					commande[commandeactuelle][x] = commande[commandeactuelle][x+1];
					x++;
				}

				commande[commandeactuelle][x] ='\0';
				x=0;
				write_string(0x0F, commande[commandeactuelle],&x,y);
				x = position;
				position = 0;
			}
			break;
		}
	}
	if (x>SIZELINE-1)
		x=79;
}

void kputchar(int c, void *arg)
{
	serial_write_char(COM2,c);
}



void kmain(void)
{
	serial_init(COM1);
	serial_write_string(COM1,"\n\rHello!\n\r\nThis is a simple echo console... please type something.\n\r");
	onCharReceive('>');

	while(1)
	{
		onCharReceive(serial_read(COM1));
	}
}
