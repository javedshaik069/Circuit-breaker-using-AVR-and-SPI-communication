#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void){
	DDRC = 0XFF;
	DDRD |= 0x03;
	SPCR = (1<<SPE);
	PORTD |= 0x00;
	while((SPSR & (1<<SPIF))==0);
	PORTC = 0X01;
	PORTD = SPDR;
}