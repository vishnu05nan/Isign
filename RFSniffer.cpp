#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <ctype.h>
#include "RCSwitch.h"

#define DB "server.db"  /*Database File Name*/

RCSwitch mySwitch;      /* RF Reciever Object*/

int h2d_rec(const char *hex, int d);

int h2d(const char *hex) {
    return h2d_rec(hex, 0);
}

int h2d_rec(const char *hex, int d) {
    char hexChar = *hex;
    if (0==hexChar) {
        return d;
    }
    int charNum;
    if (isalpha(hexChar)) {
        charNum = 10 + toupper(hexChar) - 'A';
    } else {
        charNum = hexChar - '0';
    }
    // Note d<<4 is the same as 16*d
    return h2d_rec(hex+1, (d<<4) + charNum);
}


int main()
{
	int fd;				/* file descriptor of Serial Communication */
	int i, j, k;
	int numchar;
	char car_id[] = "KL-15-BL-1231";
	int car_speed;			/* stores the speed of the vehicle */
	char elm[10], data[10];
	
	int PIN = 2;
	int severity = 0; 	/* Stores the severity of the speeding limits */
	sqlite3 *db; 		/* Database handle */
	char sqlstmt[900];		/* SQL statements */
	int prev_speed=0;
	int rxdata;
	int speed_limit;
	
	
	//char *time = NULL;	/* Stores the time */
	
     	if (wiringPiSetup() == -1)
       		return 0;
	
	fd = serialOpen("/dev/ttyUSB0",38400);
	
	if (fd < 0) {
		fprintf(stderr,"Unable to open serial device:%s\n",strerror(errno));
		return 1;
	}
	
	/* Open Database file */
	if (sqlite3_open(DB, &db) != SQLITE_OK) {
		fprintf(stderr, "[-] Unable to open the DB file\n");
		exit(EXIT_FAILURE);
	} else{
		fprintf(stderr, "Opened database successfully\n");
	}
	
	mySwitch = RCSwitch();
        mySwitch.enableReceive(PIN);
	
	while(1) {
		severity = 0;
		int flag=1;
	      
		for(i=0;i<10;i++) {
			data[i]='\0';
		}
		
		for(i=0;i<10;i++) {
			elm[i]='\0';
		}
		
		char temp[3]="\0";
		
		
		if (mySwitch.available()) {
		        int rxdata = mySwitch.getReceivedValue();
		        
		        
		        if (rxdata == 0) {
		          	printf("Unknown encoding");
		        } 
		        else {  
		          	printf("*********************************************\n");
		          	
			        if( rxdata == 96){
			        	flag=0;
			        	if(prev_speed != rxdata)
			        	system("echo \"Sign board ahead. Steep Turn\" | festival --tts");
			        	speed_limit = 0;
			        }
			      
			        else if( rxdata == 128 ){
			        	flag=0;
			        	if(prev_speed != rxdata)
			        	system("echo \"Sign board ahead. Railway Crossing\" | festival --tts");
			        	speed_limit = 0;
			        }
			        
			        else if( rxdata == 160){
			        	flag=0;
			        	if(prev_speed != rxdata)
			        	system("echo \"Sign board ahead. One Way\" | festival --tts");
			        	speed_limit = 0;
			        }
			        
			        else if( rxdata == 64){
			        	if(prev_speed != rxdata)
			        	system("echo \"Sign board ahead. School Ahead. Maintain speed of 30 Kilometer per Hour \" | festival --tts");
			        	speed_limit = 30;
			        }	
			     
			        else{
			        	speed_limit = rxdata;	
				        if(prev_speed != speed_limit){
				        	char info[100];
				        	sprintf(info, "echo \"Sign board ahead. Maintain speed of \"%d\" Kilometer per Hour \" | festival --tts",speed_limit);
						system(info);
			        	}
		        	}
		        	printf("Speed Limit=%i\n", speed_limit);
		        }
		        
		        
		          
			serialPrintf(fd,"ATI");
		        delay(800);
			numchar=serialDataAvail(fd);
				
			//printf("numchar=%d\n",numchar);
				
			if (numchar !=- 1) {
				for(j=0;j<numchar;j++) {
					elm[j]=serialGetchar(fd);
					//printf("ELM Version=%s\n",elm);
				}
			} else {
				printf("Read Error");
			}
				
				serialFlush(fd);
				
			serialPrintf(fd,"010D");
			delay(800);			
			numchar=serialDataAvail(fd);
			//printf("numchar=%d\n",numchar);
					
			if (numchar != -1) {
				for(k=0;k<numchar;k++)
					data[k]=serialGetchar(fd);
				//printf("Speed Data=%s\n",data);
			} else {
				printf("Read Error");
			}
			
			printf("Enter Speed Data:");
			scanf("%s",data);
				
			temp[0] = data[4];
			temp[1] = data[5];
			temp[3] = '\0';
			
			//printf("Temp Data=%s\n",temp);
				
			car_speed = h2d(temp);
				
			printf("Vehicle Speed=%d\n",car_speed);
			
			//int speed_limit=70;
			//car_speed=120;
			
			char tts[100];
			
			if(car_speed > speed_limit && prev_speed != rxdata && flag==1) {
				
			
				printf("Warning!!! Over Speed\n");
				
				sprintf(tts, "echo \"Woarnninngg. Overspeed. Reduce Speed to \"%d\" Kilometer per Hour\" | festival --tts",speed_limit);
				system(tts);
				
				severity = car_speed - speed_limit;
				printf("Severity=%d\n",severity);
	
		
				/* (id INTEGER PRIMARY KEY AUTOINCREMENT, car_id CHARACTER(12) NOT NULL, 
				time DATETIME NOT NULL, speedlimit INTEGER, vehicle_speed INTEGER, severity INTEGER) */
								
				time_t rawtime;
				struct tm *currentTime;
				time ( &rawtime );
				currentTime = localtime ( &rawtime );
	
				const int TIME_STRING_LENGTH = 20;
				char time[TIME_STRING_LENGTH];
	
	
				strftime(time, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", currentTime);
					
				
	
				/*int len = strlen(car_id) + strlen(time) + sizeof(speed_limit) + sizeof(car_speed) + sizeof(severity) + 108;			
				
				snprintf(sql, len, "INSERT INTO server (car_id, time, speedlimit, vehicle_speed, severity) "\
						"VALUES(\'%s\', \'%s\', \'%d\'. \'%d\'. \'%d\');", \
						car_id, time, speed_limit, car_speed, severity);*/
						
				sprintf(sqlstmt, "INSERT INTO server (car_id, time, roadspeed, vehicle_speed, severity) VALUES ('%s', '%s', %d , %d , %d );",
					car_id, time, speed_limit, car_speed, severity);
					
				printf("SQL: %s\n", sqlstmt);
				
				/* Execute SQL statement */
				if (sqlite3_exec(db, sqlstmt, 0, 0, 0) != SQLITE_OK) {
					fprintf(stderr, "SQL error\n");
					exit(EXIT_FAILURE);
				} else {
					fprintf(stdout, "Records created successfully\n");
				}
				printf("*********************************************\n");
				fflush(stdout);
			}
			
			prev_speed = rxdata;
        	}
                mySwitch.resetAvailable();
	}
	serialClose(fd);	
	sqlite3_close(db);
	return(0);
}

	
