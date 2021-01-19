#include <mega128.h>
#include <delay.h>
#include "lcd.h"
#define     Do      1908
#define     Re      1700
#define     Mi       1515
#define     Pa       1432
#define     Sol      1275
#define     La      1136
#define     Si      1012

unsigned int cnt;
unsigned char sec, min, hour, day, month;
unsigned int year;
unsigned char number[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x30};  //LCD ���ں� �ƽ�Ű�ڵ�
unsigned char keydata;
unsigned char keynum;
unsigned char getstrTime[14];
unsigned char getAlarm[3][10];
unsigned char alarmhour[3], alarmmin[3], alarmsec[3];
bit sound_flag;
bit Select;
bit chatter;
bit alarmflag;
bit enter;
interrupt [EXT_INT0] void int0(void)
{
    sound_flag = 0;        //soundflag�� 0�Ǹ鼭 �˶��� ����
    LCD_Clear();
    LCD_Pos(0, 5);
    LCD_Char('-');
    LCD_Pos(0, 8);
    LCD_Char('-');
    LCD_Pos(1, 6);
    LCD_Char(':');
    LCD_Pos(1, 9);
    LCD_Char(':');
}

interrupt [TIM0_OVF] void timer0_out_comp(void)
{
    TCNT0 = -5;
    if(Select == 0) cnt++;
    if(cnt == 2881)
    {
        cnt = 0;
        sec++;
    }

}
void inturrupt_init(void)
{
    EIMSK = 0x01;
    EICRA = 0x03;
    DDRD = 0x00;
    DDRC = 0x0f;
    PORTC = 0x0f; 
}
void init_timer(void)
{
    TIMSK = (1 << TOIE0);
    TCCR0 |=  (1<<CS02)|(1<<CS01)|(1<<CS00);
    TCNT0 = -5;
    SREG |= 0x80;
}
void myDelay_us(unsigned int delay)     //���� �����ϱ����� delay�Լ� 
{
    int i;
    for(i=0; i<delay; i++)
    {
        delay_us(1);
    }
}

void SSound(int time)                  //���� ��� �Լ� 
{
    int i, tim;
    tim = 50000/time;
    for(i=0; i<tim; i++) // ���踶�� ���� �ð� ���� �︮���� time ���� ���
    {
        PORTG |= 1<<PORTG4; //����ON, PORTG�� 4�� �� ON(out 1)
        myDelay_us(time);
        PORTG &= ~(1<<PORTG4);  //����OFF, PORTG�� 4�� �� OFF(out 0)
        myDelay_us(time);
    }
}

unsigned char KeyScan(void)
{
    //����4��Ʈ�� ����Ī�ϸ鼭 ���
    unsigned int Key_Scan_Line_Sel = 0xf7;
    unsigned char Key_Scan_sel = 0, key_scan_num = 0;
    unsigned char Get_Key_Data = 0;

    for(Key_Scan_sel = 0; Key_Scan_sel < 4; Key_Scan_sel++)
    {
        PORTC = Key_Scan_Line_Sel; //Ű ��Ʈ������ ��ĵ ���� ������ ���� PORTC ��� ��
        delay_us(10);
        Get_Key_Data = (PINC & 0xf0);

        if(Get_Key_Data != 0x00)
        {
            switch(Get_Key_Data)
            {
                case 0x10:
                    key_scan_num = Key_Scan_sel * 4 + 1;
                    break;
                case 0x20:
                    key_scan_num = Key_Scan_sel * 4 + 2;
                    break;
                case 0x40:
                    key_scan_num = Key_Scan_sel * 4 + 3;
                    break;
                case 0x80:
                    key_scan_num = Key_Scan_sel * 4 + 4;
                    break;
                default :
                    key_scan_num = 17;
                    break;
            }

            return key_scan_num;
        }

        else;
        Key_Scan_Line_Sel = (Key_Scan_Line_Sel >> 1);
    }
    return key_scan_num;
}

void Get_Time(void) // ó�� �ð��� ��¥�� �޴� �Լ� 
{
    unsigned char i = 0 ;
    sec = min = hour = day = month = year = 0;

    while(1)
    { 
        keydata = KeyScan();
        delay_ms(100);  
        
        if(i == 0)                 //�ʱ� ���� ���
        {
            LCD_Clear();
            delay_ms(4);
            LCD_Pos(0, 5);
            LCD_Char('-');
            LCD_Pos(0, 8);
            LCD_Char('-');
            LCD_Pos(1, 6);
            LCD_Char(':');
            LCD_Pos(1, 9);
            LCD_Char(':');
            LCD_Cursor_Home();
            LCD_Cursor_Shift(RIGHT);
        }
       
        if(keydata && (keydata != 15))
        {
            delay_ms(220);
           
              
            if(keydata % 4 != 0)
                keynum = (keydata / 4) * 3 + (keydata % 4);
            else
                keynum = (keydata / 4) + 9;
                
            getstrTime[i] = number[keynum];      //�ش� �ƽ�Ű�ڵ尪�� time�迭�� �ϳ��� �ִ´�. 
            
            //�Է� ���� �´� �ƽ�Ű �ڵ带 LCD�� ��� 
            if(i >= 0 && i < 4)
            {
                LCD_Pos(0, i + 1);
                LCD_Char(getstrTime[i]);
            }
            else if(i >= 4 && i < 6)
            {
                LCD_Pos(0, i + 2);
                LCD_Char(getstrTime[i]);
            }
            else if(i >= 6 && i < 8)
            {
                LCD_Pos(0, i + 3);
                LCD_Char(getstrTime[i]);
            }
            else if(i >= 8 && i < 10)
            {
                LCD_Pos(1, i - 4);
                LCD_Char(getstrTime[i]);
            }
            else if(i >= 10 && i < 12)
            {
                LCD_Pos(1, i - 3);
                LCD_Char(getstrTime[i]);
            }
            else if(i >= 12 && i < 14)
            {
                LCD_Pos(1, i - 2);
                LCD_Char(getstrTime[i]);
            }
            else;
            
            //�ƽ�Ű �ڵ尪�� �ٽ� ������ �ٲ��� ���������� ���� 
            year  = ((getstrTime[0] - 0x30) * 1000) + ((getstrTime[1] - 0x30) * 100) + ((getstrTime[2] - 0x30) * 10) + (getstrTime[3] - 0x30);
            month = ((getstrTime[4] - 0x30) * 10) + (getstrTime[5] - 0x30);
            day   = ((getstrTime[6] - 0x30) * 10) + (getstrTime[7] - 0x30);
            hour  = ((getstrTime[8] - 0x30) * 10) + (getstrTime[9] - 0x30);
            min   = ((getstrTime[10] - 0x30) * 10) + (getstrTime[11] - 0x30);
            sec   = ((getstrTime[12] - 0x30) * 10) + (getstrTime[13] - 0x30);
            if(i<14) i++;        
        }
        if(keydata == 15)           //�� �Է��� ���
        {
            enter = 1;
            if((enter == 1) && (i == 14))
            {
                enter = 0;
                Select = 0;
                LCD_Clear();
                delay_ms(4);
                LCD_Pos(0, 5);
                LCD_Char('-');
                LCD_Pos(0, 8);
                LCD_Char('-');
                LCD_Pos(1, 6);
                LCD_Char(':');
                LCD_Pos(1, 9);
                LCD_Char(':');
             
                break;
            }
                
        } 
    }   
}

void AlarmSetMode(void) // �˶� ���ø�� 
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char alarmtxt[] = "DONE!!!";    

    while(1)
    {
        keydata = KeyScan();
        delay_ms(220);
        if(i==0)    // �ʱⰪ �� �� �� ���
        {
            LCD_Pos(0, 1);
            LCD_Char((year / 1000) + '0');
            LCD_Char(((year % 1000) - (year % 100)) / 100 + '0');
            LCD_Char(((year % 100) - (year % 10)) / 10 + '0');
            LCD_Char((year % 10) + '0');
            LCD_Char('-');
            LCD_Char((month / 10) + '0');
            LCD_Char((month % 10) + '0');
            LCD_Char('-');
            LCD_Char((day / 10) + '0');
            LCD_Char((day % 10) + '0');
            LCD_Pos(1, 6);
            LCD_Char(':');
            LCD_Pos(1, 9);
            LCD_Char(':');
            LCD_Pos(1, 4);
        }
        if(j==3)     // �� �Է� �Ǹ� �������� ���ư��� �˶��÷��װ� ��Ʈ �Ǹ鼭 �ش� �ð��� �Ǹ� �˶��� �︲ 
        {
            LCD_Clear();
            LCD_Str(alarmtxt);
            delay_ms(1000);
            LCD_Clear();
            delay_ms(4);
            LCD_Pos(0, 5);
            LCD_Char('-');
            LCD_Pos(0, 8);
            LCD_Char('-');
            LCD_Pos(1, 6);
            LCD_Char(':');
            LCD_Pos(1, 9);
            LCD_Char(':');
            LCD_Pos(1, 4);
            alarmflag = 1;
            break;
        }
        if(keydata == 8)   //M2�� �ѹ� �� ������ ���߿� �������� ���� 
        {
            alarmflag = 1;
            break;
        }

        if(keydata && (keydata != 15))
        {
            delay_ms(250);

            if(keydata % 4 != 0)
                keynum = (keydata / 4) * 3 + (keydata % 4);
            else
                keynum = (keydata / 4) + 9;
                
            getAlarm[j][i] = number[keynum];    
            
            if(i >= 0 && i < 2)
            {
                LCD_Pos(1, i + 4);
                LCD_Char(getAlarm[j][i]);
            }
            else if(i >= 2 && i < 4)
            {
                LCD_Pos(1, i + 5);
                LCD_Char(getAlarm[j][i]);
                delay_ms(200);
            }
            else if(i >= 4 && i < 6)
            {
                LCD_Pos(1, i + 6);
                LCD_Char(getAlarm[j][i]);
                delay_ms(200);
            }
            else;

            if(i != 6)
            {
                i++;
            }        
        }
        if(keydata == 15)           //�� �Է��� ���
        {
            enter = 1;
            if((enter == 1) && (i == 6))
            {
                alarmhour[j]  = ((getAlarm[j][0] - 0x30) * 10) + (getAlarm[j][1] - 0x30);
                alarmmin[j]   = ((getAlarm[j][2] - 0x30) * 10) + (getAlarm[j][3] - 0x30);
                alarmsec[j]   = ((getAlarm[j][4] - 0x30) * 10) + (getAlarm[j][5] - 0x30);
                j++;
                LCD_Clear();
                i = 0;
            }
        }
    }   
}

void main(void)
{
    unsigned char evenodd = 0;
    unsigned char february = 0;
    unsigned char str[] = "2019-05-01";
    unsigned char str1[] = "AM 12:00:00";
    unsigned char strtimeset[] = "TIME SET MODE..!";
    unsigned char strAlarmset[] = "Alarm SET MODE!!";
    unsigned char AM[] = "AM";
    unsigned char PM[] = "PM";
    unsigned char keymod;
    unsigned char almtxt[]="!!!!Wake  up!!!!";
    bit M3;

    cnt = sec = min = Select = hour = keymod = 0;
    year = 2019;
    month = 5;
    day = 1;
    M3 = 0;

    init_timer();
    inturrupt_init();
    LCD_Init();
    DDRG |= (1<<4);

    LCD_Pos(0, 1);
    LCD_Str(str);
    LCD_Pos(1, 1);
    LCD_Str(str1);

    while(1)
    {
        keymod = KeyScan();

        if(keymod == 4)            //M1 ������ ��¥ �ð� ���� ���� ��
        {
            Select = 1;
            LCD_Clear();
            LCD_Str(strtimeset);
            delay_ms(2000);
            Get_Time();
        }

        if(keymod == 8)           //M2 ������ �˶� ���� ���� �� 
        {
            LCD_Clear();
            LCD_Str(strAlarmset);
            delay_ms(2000);
            LCD_Clear();
            LCD_Cursor_Home();
            LCD_Cursor_Shift(RIGHT);
            AlarmSetMode();
        }

        if(keymod == 0)
            chatter = 0;          //M3 ��ư ä�͸� ������� �Լ� 
        else
        {
            if(!chatter)
            {
                chatter = 1;
                if(keymod == 12)
                {
                    M3 = ~M3;
                    if(M3 == 1)
                    {
                        LCD_Clear();
                        LCD_Cursor_Shift(RIGHT);   
                    }
                }
            }
        }
                
        if(month >= 1 && month <= 7)
        {
            if(month == 2) february = 1;
                                                    //��¥ ��� �˰��� 
            evenodd = month % 2;
        }
        else if(month >= 8 && month <= 12)
            evenodd = (month + 1) % 2;
        else;
        if(sec >= 60)
        {
            min++; sec = 0;
        }

        if(min >= 60)
        {
            hour++;
            min = 0;
        }
        if(hour >= 24)
        {
            day++;
            hour = 0;
        }
        if((evenodd == 0) && (february == 1) && (day == 29))
        {
            month++;
            day = 1;
        }
        else if((evenodd == 0) && (day == 31))
        {
            month++;
            day = 1;
        }
        else if((evenodd == 1) && (day == 32))
        {
            month++;
            day = 1;
        }
        else if(day >=32)
        {
            day = 1;    
        }
        else;

        if(month >= 13 )
        {
            year++;
            month = 1;
        }
        
        LCD_Pos(0, 1);
        LCD_Char((year / 1000) + '0');
        LCD_Char(((year % 1000) - (year % 100)) / 100 + '0');
        LCD_Char(((year % 100) - (year % 10)) / 10 + '0');
        LCD_Char((year % 10) + '0');

        LCD_Pos(0, 6);
        LCD_Char(month / 10 + '0');
        LCD_Char(month % 10 + '0');

        LCD_Pos(0, 9);
        LCD_Char(day / 10 + '0');
        LCD_Char(day % 10 + '0');

        if(M3 == 0)
        {
            LCD_Pos(0, 5);
            LCD_Char('-');
            LCD_Pos(0, 8);
            LCD_Char('-');
            LCD_Pos(1, 6);
            LCD_Char(':');
            LCD_Pos(1, 9);
            LCD_Char(':');
            if(hour > 12)
            {
                LCD_Pos(1, 1);
                LCD_Str(PM);
                LCD_Pos(1, 4);
                LCD_Char(((hour - 12) / 10) + '0');
                LCD_Char(((hour - 12) % 10) + '0');
            }
            else
            {
                LCD_Pos(1, 1);
                LCD_Str(AM);
                LCD_Pos(1, 4);
                LCD_Char((hour / 10) + '0');
                LCD_Char((hour % 10) + '0');
            }
        }
        else if(M3 == 1)
        {

            LCD_Pos(1, 4);
            LCD_Char(((hour) / 10) + '0');
            LCD_Char(((hour) % 10) + '0');
            LCD_Pos(0, 5);
            LCD_Char('-');
            LCD_Pos(0, 8);                      //M3 ������ 24H��� 
            LCD_Char('-');
            LCD_Pos(1, 6);
            LCD_Char(':');
            LCD_Pos(1, 9);
            LCD_Char(':');
        }
        else;

        LCD_Pos(1, 7);
        LCD_Char((min / 10) + '0');
        LCD_Char((min % 10) + '0');
        LCD_Pos(1, 10);
        LCD_Char((sec / 10) + '0');
        LCD_Char((sec % 10) + '0');
        if(alarmflag)
        {
            if((hour == alarmhour[0]) && (min == alarmmin[0]) && (sec == alarmsec[0]))
            {
                sound_flag = 1;
                delay_ms(3);
                 alarmhour[0]=alarmmin[0]=alarmsec[0]=0;
                    
                while(sound_flag == 1)
                {
                    LCD_Pos(0,0);
                    LCD_Str(almtxt);
                    LCD_Pos(1,0);
                    LCD_Str(almtxt);
                    SSound(Re);
                    SSound(Pa);
                    SSound(La);
                }      
            }
            if((hour == alarmhour[1]) && (min == alarmmin[1]) && (sec == alarmsec[1]))
            {
                sound_flag = 1;
                alarmhour[1]=alarmmin[1]=alarmsec[1]=0;
                delay_ms(3);     
                while(sound_flag == 1)
                {
                    LCD_Pos(0,0);
                    LCD_Str(almtxt);
                    LCD_Pos(1,0);
                    LCD_Str(almtxt);
                    SSound(Do);
                    SSound(Mi);
                    SSound(Sol);
                }   
            }
            if((hour == alarmhour[2]) && (min == alarmmin[2]) && (sec == alarmsec[2]))
            {
                sound_flag = 1;
                alarmhour[2]=alarmmin[2]=alarmsec[2]=0;
                delay_ms(3);   
                while(sound_flag == 1)
                {
                    LCD_Pos(0,0);
                    LCD_Str(almtxt);
                    LCD_Pos(1,0);
                    LCD_Str(almtxt);
                    SSound(Do); 
                    SSound(Mi);
                    SSound(Sol);
                }   
            }
        }          
    }
}
