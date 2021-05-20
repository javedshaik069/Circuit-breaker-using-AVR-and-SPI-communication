# define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>


#define ctrl PORTC
#define en PC2
#define rw PC1
#define rs PC0
int Load1=0;
int Load2=0;

void LCD_init(void);
void LCD_clear(void);
void LCD_cmd(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_string();
void SPI_master_init(void);
void SPI_transmit(unsigned char data);
void keypad1(void);
void keypad2(void);
void keypad3(void);
void keypad4(void);
void keypad5(void);
void passkey1(void);
void passkey2(void);
char keyscan(void);
char keycheck(void);

int main(void)
{
	DDRA = 0XFF;
	DDRC = 0X07;
	DDRD = 0XF0;
	SPI_master_init();
	LCD_init();
	LCD_string("WelcomeTo");
	LCD_cmd(0XC0);
	LCD_string("AMC Project");
	_delay_ms(10);
	while(1){
		LCD_clear();
		LCD_string("1)KnowTheStatus");
		LCD_cmd(0XC0);
		LCD_string("2)ChangeStatus");
		keypad1();
	}
	return 0;
}

void keypad1(void)
{
	char key1;
	key1=keyscan();
	if(key1=='1'){
		LCD_clear();
		LCD_string("Status is");
		LCD_clear();
		if (Load1==0 && Load2==0){
			LCD_string("Load1IsOFF");
			LCD_cmd(0XC0);
			LCD_string("Load2IsOFF");
		}
		if (Load1==1 && Load2==0){
			LCD_string("Load1IsON");
			LCD_cmd(0XC0);
			LCD_string("Load2IsOFF");
		}
		if(Load1==0 && Load2==1){
			LCD_string("Load1IsOFF");
			LCD_cmd(0XC0);
			LCD_string("Load2IsON");
		}
		if(Load1==1 && Load2==1){
			LCD_string("Load1IsON");
			LCD_cmd(0XC0);
			LCD_string("Load2IsON");
		}
		_delay_ms(100);
	}
	if(key1=='2'){
		keypad2();
	}
	return;
}

void keypad2(void)
{
	LCD_init();
	LCD_string("1)ChangeLoad1");
	LCD_cmd(0XC0);
	LCD_string("2)ChangeLoad2");
	keypad3();
	return;
}

void keypad3(void)
{
	char key2;
	key2=keyscan();
	if(key2=='1'){
		LCD_clear();
		LCD_string("EnterThePasskey1");
		LCD_cmd(0XC0);
		passkey1();
	}
	if(key2=='2'){
		LCD_clear();
		LCD_string("EnterThePasskey2");
		LCD_cmd(0XC0);
		passkey2();
	}
	return;
}

void passkey1(void)
{
	int i;
	int count1;
	char arr1[4];
	char original1[4]={'1','2','3','4'};
	for(i=0; i<4; i++){
		arr1[i]=keyscan();
		LCD_data(arr1[i]);
		LCD_cmd(0X06);
		_delay_ms(10);
	}
	for(i=0; i<4; i++){
		if(arr1[i]==original1[i]){
			count1 += 1;
		}
	}
	if(count1 == 4){
		LCD_clear();
		LCD_string("PasskeyRight");
		_delay_ms(100);
		LCD_clear();
		LCD_string("SendingSignal");
		_delay_ms(10);
		if(Load1==0){
			if(Load2==0){
				SPI_transmit(0X01);
				Load1=1;
			}
			else{
				SPI_transmit(0X03);
				Load1=1;
			}
		}
		else{
			if(Load2==0){
				SPI_transmit(0X00);
				Load1=0;
			}
			else{
				SPI_transmit(0X02);
				Load1=0;
			}
		}
		
	}
	else{
		LCD_clear();
		LCD_string("PasskeyWrong");
		_delay_ms(100);
		LCD_clear();
		LCD_string("TryAgainLater");
		_delay_ms(100);
	}
	return;
}

void passkey2(void)
{
	int i,count2;
	char arr2[4];
	char original2[4]={'8','5','2','0'};
	for(i=0; i<4; i++){
		arr2[i]=keyscan();
		LCD_data(arr2[i]);
		LCD_cmd(0X06);
	}
	for(i=0; i<4; i++){
		if(arr2[i]==original2[i]){
			count2 += 1;
		}
	}
	if(count2 == 4){
		LCD_clear();
		LCD_string("PasskeyRight");
		_delay_ms(50);
		LCD_clear();
		LCD_string("SendingSignal");
		if(Load2==0){
			if(Load1==0){
				SPI_transmit(0X02);
				Load2=1;
			}
			else{
				SPI_transmit(0X03);
				Load2=1;
			}
		}
		else{
			if(Load1==0){
				SPI_transmit(0X00);
				Load2=0;
			}
			else{
				SPI_transmit(0X01);
				Load2=0;
			}
		}
	}
	else{
		LCD_clear();
		LCD_string("PasskeyWrong");
		_delay_ms(100);
		LCD_clear();
		LCD_string("TryAgainLater");
		_delay_ms(100);
	}
	return;
}

char keyscan(void)
{
	char key = 'a';
	while(key=='a')
	{
		key = keycheck();
	}
	return key;
}

char keycheck(void)
{
	PORTD=0b11101111;	//ground row 0 (A-key row)
	_delay_ms(3);
	if((PIND&0b00000001)==0)
	{
		return '7';
	}
	if((PIND&0b00000010)==0)
	{
		return '4';
	}
	if((PIND&0b00000100)==0)
	{
		return '1';
	}
	PORTD=0b11011111;	//ground row 1 (B-key row)
	_delay_ms(3);
	if((PIND&0b00000001)==0)
	{
		return '8';
	}
	if((PIND&0b00000010)==0)
	{
		return '5';
	}
	if((PIND&0b00000100)==0)
	{
		return '2';
	}
	if((PIND&0b00001000)==0)
	{
		return '0';
	}
	PORTD=0b10111111;	//ground row 3 (C-key row)
	_delay_ms(3);
	if((PIND&0b00000001)==0)
	{
		return '9';
	}
	if((PIND&0b00000010)==0)
	{
		return '6';
	}
	if((PIND&0b00000100)==0)
	{
		return '3';
	}
	PORTD=0b01111111;	//ground row 4 (D-key row)
	_delay_ms(3);
	if((PIND&0b00000001)==0)
	{
		return '/';
	}
	if((PIND&0b00000010)==0)
	{
		return '*';
	}
	if((PIND&0b00000100)==0)
	{
		return '-';
	}
	if((PIND&0b00001000)==0)
	{
		return '+';
	}
	//if no key pressed
	return 'a';
}


void LCD_init(void)
{
	LCD_cmd(0x38);
	_delay_ms(1);
	LCD_cmd(0x38);
	_delay_ms(1);
	LCD_cmd(0x38);
	_delay_ms(1);
	LCD_cmd(0x06);
	_delay_ms(1);
	LCD_cmd(0x0c);
	_delay_ms(1);
	LCD_cmd(0x01);
	_delay_ms(1);
	LCD_cmd(0x02);
	_delay_ms(1);
	return ;
}

void LCD_clear(void)
{
	LCD_cmd(0X01);
	_delay_ms(1);
	LCD_cmd(0X80);
	_delay_ms(10);
}

void LCD_cmd(unsigned char cmd)
{
	PORTA=cmd;
	ctrl=(0<<rs)|(0<<rw)|(1<<en);
	_delay_ms(1);
	ctrl=(0<<rs)|(0<<rw)|(0<<en);
	_delay_ms(2);
	return;
}

void LCD_data(unsigned char data)
{
	PORTA=data;
	ctrl=(1<<rs)|(0<<rw)|(1<<en);
	_delay_ms(1);
	ctrl=(1<<rs)|(0<<rw)|(0<<en);
	_delay_ms(2);
	return;
}

void LCD_string(unsigned char* str)
{
	int i=0;
	while(str[i]!='\0')
	{
		LCD_data(str[i]);
		i++;
	}
	return;
}

void SPI_master_init(void)
{
	DDRB = ((1<<PB5)|(1<<PB7));
	PORTB |= (1<<PB4);
	SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR1));
}

void SPI_transmit(unsigned char data){
	SPDR=data;
	while(!(SPSR & (1<<SPIF)));
	return;
}



