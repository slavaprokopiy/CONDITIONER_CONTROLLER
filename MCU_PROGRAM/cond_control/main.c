/*
 * cond_control.c
 *
 * Created: 20.10.2015 20:54:36
 * Author : prokopiy
 */ 
#define F_CPU 8000000UL // 8 MHz
//#define F_CPU 14.7456E6
#include <util/delay.h>

#define ON	1
#define OFF	0
#define HIGH	1
#define LOW		0


#define SW1_PIN				PINB0
#define SW2_PIN				PINB2
#define RELAY_PIN			PINB1
#define LED_ANODE_PIN		PINB3
#define LED_CATHIODE_PIN	PINB4
#define LED_VOLTAGE_TRESHOLD 0.4
#define	RELAY_ON_TIME		150  // ms
#define	PAUSE_TIME			1000 // ms

#include <avr/io.h>


int SW1_STATUS, SW2_STATUS, LED_STATUS;

int Get_Pin_State(int);
/*int Get_SW1_State(void);
int Get_SW2_State(void);*/
int Get_LED_Status(void);
//void wait(int);
void Turn_Relay_ON(void);
void Turn_Relay_OFF(void);


int main(void)
{
    // Init GPIO
	DDRB = 0x00;
	PORTB = 0x00;
	
	// LED Input Pins
	DDRB &= ~(1<<LED_ANODE_PIN) | ~(1<<LED_CATHIODE_PIN); 
	// SW1 & SW2 input pins
	DDRB &= ~(1<<SW1_PIN) | ~(1<<SW2_PIN); 
	
	// Relay Output Pin
	DDRB |= (1<<RELAY_PIN);
	PORTB &= ~(1<<RELAY_PIN);
	
	//PB2 - Debug Output
	//DDRB |= (1<<PINB2);
	//PB0 - Debug Output
	//DDRB |= (1<<PINB0);
	
	// Init ADC
	//ADCSRA = (1<<ADEN);
	// VCC used as Voltage Reference
	ADMUX &= ~(1<<REFS1) | ~(1<<REFS0);
	
	//Debug
	/*while(1){
		Get_LED_Status();
		LED_STATUS = Get_LED_Status();		// ����� SW1
		if(LED_STATUS){Turn_Relay_ON();}
			else{Turn_Relay_OFF();}
	}*/
	//Debug wait for 1 sec
	/*while(1){
		Turn_Relay_ON();
		_delay_ms(1000);
		Turn_Relay_OFF();
		_delay_ms(1000);
	}*/
	
	while (1) 
    {
	 //1. ��� ���� �������, ������ �� ������ (�������� ������������, ���� �����)
	    _delay_ms(PAUSE_TIME);
		//2. (��������� ���) ���������� ��������� ��������� EN � ONOFF �� ������� �2, � ��������� ����������.
		SW1_STATUS = Get_Pin_State(SW1_PIN);		// ����� SW1
		SW2_STATUS = Get_Pin_State(SW2_PIN);		// ����� SW2
		LED_STATUS = Get_LED_Status();		// ����� ����������
		
										// ���� ����������� SW1 (EN) ���������, �� ������ �� ������. 
		if(SW1_STATUS == ON){			// � ��������� ������,		
			if(LED_STATUS == ON){		// ���� ��������� ����� 
				if(SW2_STATUS == OFF){	// � �������� SW2 (ONOFF) ����������, 
					Turn_Relay_ON();	//  �� ��������� ������ - �������� �������� 4 � 5 �� 150 �����������.	
					_delay_ms(RELAY_ON_TIME);			
					Turn_Relay_OFF();
				}
			}
			else{						//	��� ��������� �� �����, 
				if(SW2_STATUS == ON){	// � �������� SW2 (ONOFF) ��������,
					Turn_Relay_ON();	//  �� ��������� ������ - �������� �������� 4 � 5 �� 150 �����������.
					_delay_ms(RELAY_ON_TIME);
					Turn_Relay_OFF();	
				}
			}							
		}
	}
	//3. goto 1
}

int Get_LED_Status(void){
	float LED_CATHODE_VOLTAGE;
	float LED_ANODE_VOLTAGE;
	uint16_t TEMP1 = 0;
	uint16_t TEMP2 = 0;
	int status;
		
	// ��������� ���������� �� PB4 - LED_CATHODE
	ADCSRA |= (1<<ADEN); // Enable ADC
	ADMUX &= 0xF0;
	ADMUX &= ~(1<<MUX3) | ~(1<<MUX2) | ~(1<<MUX0);	// PB4 is ADC input ADMUX = 0010
	ADMUX|= (1<<MUX1);								// PB4 is ADC input
	// ������ ���������
	ADCSRA |= (1<<ADSC);
	// �������� ��������� ���������
	while(((ADCSRA >> ADIF) & 0x01) == 0){;}
	ADCSRA |= (1<<ADIF); // Clear ADC Interrupt flag
	// ��������� ���������� 
	TEMP1 = ADCL;	// ������� ������ ADCL
	TEMP2 = ADCH;	// ����� ADCH
	TEMP1 = TEMP1 | (TEMP2 << 8);
	ADCSRA &= ~(1<<ADEN);		// Disable ADC
	 
	LED_CATHODE_VOLTAGE = (TEMP1*3.3)/1023;

// Debug
	//if(LED_CATHODE_VOLTAGE < 0.1){Turn_Relay_ON();}
	//else{Turn_Relay_OFF();}
	
	// ��������� ���������� �� PB3 - LED_ANODE
	ADCSRA |= (1<<ADEN); // Enable ADC
	ADMUX &= 0xF0;	
	ADMUX &= ~(1<<MUX3) | ~(1<<MUX2) ;	//PB3 is ADC input ADMUX = 0011
	ADMUX|= (1<<MUX1) | (1<<MUX0);		//PB3 is ADC input	
	// ������ ���������
	ADCSRA |= (1<<ADSC);
	// �������� ��������� ���������
	while(((ADCSRA >> ADIF) & 0x01) == 0){;}
	ADCSRA |= (1<<ADIF); // Clear ADC Interrupt flag
	// ��������� ����������
	TEMP1 = ADCL;	// ������� ������ ADCL
	TEMP2 = ADCH;				// ����� ADCH
	TEMP1 = TEMP1 | (TEMP2 << 8);
	ADCSRA &= ~(1<<ADEN);		// Disable ADC
	
	
	LED_ANODE_VOLTAGE = (TEMP1*3.3)/1023;
// Debug			
	//if(LED_ANODE_VOLTAGE > 0.4){Turn_Relay_ON();}
	//else{Turn_Relay_OFF();}
	//if(LED_CATHODE_VOLTAGE == LED_ANODE_VOLTAGE){Turn_Relay_ON();}
	//else{Turn_Relay_OFF();}
	
	LED_ANODE_VOLTAGE -= LED_CATHODE_VOLTAGE;	// ��������� ������� ���������� �� ����������+��������:
	if(LED_ANODE_VOLTAGE > LED_VOLTAGE_TRESHOLD){status = ON;}		// ���� ������� ������ �������������� ������, ������ ��������� �������
	else{status = OFF;}												// ����� - ��������� ��������

//Debug	
	//if(status == ON){Turn_Relay_ON();}
	//else{Turn_Relay_OFF();}
	return(status);
}
int Get_Pin_State(int SW){
	int status = (PINB >> SW);
	status &= 0x01;
	if(status == 0x01){status = OFF;}
	else{status = ON;}
	return(status);
}
void Turn_Relay_ON(void){
	PORTB |= (1<<RELAY_PIN);
}
void Turn_Relay_OFF(void){
	PORTB &= ~(1<<RELAY_PIN);
}
/*void wait(int time){
	if(time < 1){return;}
	uint16_t i ;
	for( i = 0; i < 250; i++){
		unsigned long a = time;
		while(a > 0){a--;}
		}
}*/