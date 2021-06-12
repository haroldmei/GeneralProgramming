/*
ff3hr is a forensic tool to recover deleted history records from Firefox 3. 
This browser uses  SQLite databases to store the history, 
and this tool can search and recover records from moz_places, moz_historyvisits moz_downloads and moz_formhistory tables in a whole disk image.
murilo.mtp at dpf.gov.br
*/
#include "stdafx.h"

#include <time.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <Fcntl.h>
#include <SYS\Stat.h>
#include <Share.h>
#include <math.h>


#define FILE_BUFFER 4000

int  visit_count, hidden, typed, favicon, frecency, url, title ,rev_host, header ; /* var for moz_places table */
int bufferPointer, totalRecFound=0;
int fp; /* global pointer to input file */
FILE *fpoutMozPlaces , /* global pointer to output file */
	 *fpoutMozHistoryVisit,
	 *fpoutMozFormHistory, 
	 *fpoutMozDownloads;

long long position=0;
char fileBuffer[FILE_BUFFER];

/* To avoid read and seek small values, we read a big buffer and keep track of the pointer */
int readFileBuffer()
{
	/* seek back to the next char to be processed */
	_lseeki64(fp, bufferPointer - FILE_BUFFER, SEEK_CUR);
	return _read(fp, fileBuffer, FILE_BUFFER);
}

/* 
readArrayOfSizes:  read the array of sizes and check the consistency
return 0 if it is not a history record.
return 1 if it is a history record*/
int readArrayOfSizes()
{
	int huffmanFlag, headerCount, headerFlag, pointer, i, id, calcRecSize, recordSize; 
	char buffer[18];

	/* fp is pointing to the next char after the http:// (or other protocol indication) 
	seek back the "http://" + 18 bytes to be read (maximum size of the array) , total of 25 bytes
	Maximum size of each field in array of types and sizes (normally the array has 10, 11 or 12 bytes only): 
	record size up to 2, id up to 3, hsize=2 (1 + byte 0x00), url up to 2, title up to 2, rev_host up to 2, visit_count = 1, hidden = 1, typed = 1, favicon = 1, frecency = 1 */

	bufferPointer -= 25;
	if(bufferPointer < 0) {/* we need to correct the file buffer */
		_lseeki64(fp, bufferPointer - FILE_BUFFER , SEEK_CUR);
		_read(fp, fileBuffer, FILE_BUFFER);
		bufferPointer = 0;
	}
	for(i=0;i<18;i++) 
			buffer[i] = fileBuffer[bufferPointer++];

	visit_count = buffer[13];
	hidden = buffer[14];
	typed = buffer[15];
	favicon = buffer[16];
	frecency = buffer[17];
    /* headerCount will keep count of header size  (field sizes) to compare to headerSize */
	headerCount = 5;	

	if (visit_count > 0 && visit_count < 4)
		if (hidden == 1 && typed == 1)
			if (favicon >= 0 && favicon < 4)
				if (frecency > 0 && frecency < 4) {
					/* up to here it is like a record. now, try to read the sizes of url, title and rev_host, remebering about the huffman code  */

					calcRecSize = visit_count + hidden + typed +  favicon + frecency; /* this variable will sum all field sizes */
					/* read rev_host */
					rev_host =  buffer[12];
					headerCount ++;
					huffmanFlag = buffer[11];  /* test for huffman coding (uses 2 bytes) */
					if(huffmanFlag < 0 ) huffmanFlag += 256; /*  correct the convertion from signed char to unsigned int */
					pointer = 11;
					if (huffmanFlag > 127 ) {
						/* it큦 a huffman code. Calculate value and position pointer to prepare to next field */
						rev_host = (huffmanFlag - 128) * 128 + rev_host;
						pointer--;
						headerCount ++;
					}
					calcRecSize += (rev_host-13)/2;

					/* read title */
					title = buffer[pointer--];
					headerCount ++;
					huffmanFlag = buffer[pointer];
					if(huffmanFlag < 0 ) huffmanFlag += 256;
					if (huffmanFlag > 127 ) {
						/* it큦 a huffman code. Calculate value and position pointer to prepare to next field */
						title = (huffmanFlag - 128) * 128 + title;
						pointer--;
						headerCount ++;
					}

					calcRecSize += (title-13)/2;

					/* read url */
					url = buffer[pointer--];
					headerCount ++;
					huffmanFlag = buffer[pointer];
					if(huffmanFlag < 0 ) huffmanFlag += 256;
					if (huffmanFlag > 127 ) {
						/* it큦 a huffman code. Calculate value and position pointer to prepare to next field */
						url = (huffmanFlag - 128) * 128 + url;
						pointer--;
						headerCount ++;
					}
					
					calcRecSize += (url-13)/2;

					headerFlag = buffer[pointer--];
					header = buffer[pointer--];				
					headerCount += 2;					
					
					if ((header == headerCount) && (headerFlag == 0) ) {
						/* header is consistent. It is definitely a history record */
						totalRecFound++;
						calcRecSize += header;

						/* read id*/
						id = buffer[pointer--];
												
						huffmanFlag = buffer[pointer];
						if(huffmanFlag < 0 ) 
							huffmanFlag += 256;
						if(huffmanFlag < 128 ) { /* id uses only one byte */
							recordSize = huffmanFlag;
							if (calcRecSize == recordSize) { /* consistent with 1 byte for id  and record size*/
								fprintf(fpoutMozPlaces,"%d \t", id); /* print id with 1 byte */
								return 1;
							}
							else {
								huffmanFlag = buffer[pointer-1];
								if(huffmanFlag < 0 ) 
									huffmanFlag += 256;
								recordSize = (huffmanFlag - 128) * 128 + recordSize;
								if (calcRecSize == recordSize) {/* consistent with 1 byte for id  and 2 bytes for record size*/
									fprintf(fpoutMozPlaces,"%d \t", id); /* print id with 1 byte */
									return 1;
								}
							}
						}
					
						id = (huffmanFlag - 128) * 128 + id;
						huffmanFlag = buffer[--pointer];
						if(huffmanFlag < 0 ) 
							huffmanFlag += 256;
						recordSize = huffmanFlag;
						if (calcRecSize == recordSize) { /* consistent with 2 bytes for id  and 1 byte record size*/
							fprintf(fpoutMozPlaces,"%d \t", id); /* print id */
							return 1;
						}
						else {
							huffmanFlag = buffer[pointer-1];
							if(huffmanFlag < 0 ) 
								huffmanFlag += 256;
							recordSize = (huffmanFlag - 128) * 128 + recordSize;
							if (calcRecSize == recordSize) {/* consistent with 2 byte for id  and 2 bytes for record size*/
								fprintf(fpoutMozPlaces,"%d \t", id); /* print id */
								return 1;
							}
						
						/* test id with 3 bytes */
						huffmanFlag = buffer[pointer--];
						if(huffmanFlag < 0 ) 
							huffmanFlag += 256;
						if (huffmanFlag > 127 )	{/* it큦 a huffman code. Calculate value of id with 3 bytes. */
							id = (huffmanFlag - 128) * 16384 + id;
							huffmanFlag = buffer[pointer];
							if(huffmanFlag < 0 ) 
								huffmanFlag += 256;
							recordSize = huffmanFlag;
							if (calcRecSize == recordSize) { /* consistent with 3 bytes for id  and 1 byte record size*/
								fprintf(fpoutMozPlaces,"%d \t", id); /* print id */								
								return 1;
							}
							else {
								huffmanFlag = buffer[pointer-1];
								if(huffmanFlag < 0 ) 
									huffmanFlag += 256;
								recordSize = (huffmanFlag - 128) * 128 + recordSize;
								if (calcRecSize == recordSize) {/* consistent with 3 byte for id  and 2 bytes for record size*/
									fprintf(fpoutMozPlaces,"%d \t", id); /* print id */
									/* fprintf(fpoutMozPlaces,"%d \t", recordSize); /* print record size */
									return 1;
								}
								else 
									fprintf(fpoutMozPlaces,"ID or Record size overwritten? \t \t"); 
							}
						}
						else 
							fprintf(fpoutMozPlaces,"ID or Record size overwritten? \t \t"); /* id with 3 byte is not possible. */ 
						}						
					}
					else  /* it is not a history record */
						return 0;
					
				}
	return 0;
} /*end of int readArrayOfSizes() */


/* this function takes an array of char, a pointer to the position in this array and the 
size (how many bytes/char) to translate to big endian.
Integers in SQLite data segment are in big endian format */
long long readBigEndian(char *ch, int pointer, int size)
{
	int i, c;
	long long l, p;

	l = 0;
	for (i=0; i<size; i++) {
		c = ch[pointer+i];
		if(c < 0) 
			c += 256;/*  correct the convertion from signed char to unsigned int */
		p = c * pow((float)256,size-i-1);
		l += p;
	}
	return l;
}


int readUpTo2BytesBack(int &pointer)
{
	int huffmanFlag, value;

	value = fileBuffer[pointer--];
	if(value < 0 ) 
		value += 256;
	huffmanFlag = fileBuffer[pointer];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if(huffmanFlag < 128 ) 	/* value uses only one byte */
		return value;
	value = (huffmanFlag - 128) * 128 + value;
	pointer--;
	return value;
}
int readUpTo2BytesFront(int &pointer)
{
	int huffmanFlag, value;

	huffmanFlag = fileBuffer[pointer++];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if(huffmanFlag < 128 ) 	/* value uses only one byte */
		return huffmanFlag;
	value = fileBuffer[pointer++];
	if(value < 0 ) 
		value += 256;
	value = (huffmanFlag - 128) * 128 + value;
	return value;
}
int readUpTo3BytesBack(int &pointer)
{
	int huffmanFlag, value;

	value = fileBuffer[pointer--];
	if(value < 0 ) 
		value += 256;
	huffmanFlag = fileBuffer[pointer];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if(huffmanFlag < 128 ) 	/* value uses only one byte */
		return value;

	value = (huffmanFlag - 128) * 128 + value; /* value with 2 bytes */
	pointer--;

	/* test value with 3 bytes */
	huffmanFlag = fileBuffer[pointer];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if (huffmanFlag > 127 )	{/* it큦 a huffman code. Calculate value of id with 3 bytes. */
		value = (huffmanFlag - 128) * 16384 + value;
		pointer--;
		}
	return value;
}


// this function prints a string of size VALUE from fileBuffer. Checks should be done on calling function
void printFileBufferChar(int value, FILE * f, char separator)
{
	int i;
	char buffer[2000];

	for(i=0;i<value;i++) {
		buffer[i] = fileBuffer[bufferPointer++];
		if(buffer[i]<0) 
			buffer[i] +=256;	
	}
	buffer[i]='\x0';
	fprintf(f,"%s%c", buffer, separator);
}

/* read a moz_places record */
int readMozPlaces()
{
	char buffer[2048];
	int pointer, int_fields, c, i;

	
	/* read the array of types and sizes and put in global variables */
	if (readArrayOfSizes() == 1) { /* field sizes are consistent. read record  */
		/* correct url, title and rev_host value ( (N-13)/2) */
		url = (url-13)/2;
		title = (title-13)/2;
		rev_host = (rev_host-13)/2;
		
		/* check if these fields will overflow the buffer */
		if (url+title+rev_host+bufferPointer > FILE_BUFFER-20) {
			readFileBuffer();
			bufferPointer = 0;
		}
				
		for(i=0;i<url;i++) 
			buffer[i] = fileBuffer[bufferPointer++];
		buffer[url] = '\x0';
		fprintf(fpoutMozPlaces,"%s\t", buffer);
		position += url-7; /* the pointer was pointing to the char after http:// */

		for(i=0;i<title;i++) 
			buffer[i] = fileBuffer[bufferPointer++];			
		buffer[title] = '\x0';
		fprintf(fpoutMozPlaces,"%s\t", buffer);
		position += title;

		for(i=0;i<rev_host;i++) 
			buffer[i] = fileBuffer[bufferPointer++];
		buffer[rev_host] = '\x0';
		fprintf(fpoutMozPlaces,"%s\t", buffer);
		position += rev_host;

		/* read the total bytes for the other fields */
		int_fields = visit_count+typed+hidden+favicon+frecency;
		position += int_fields;
		for(i=0;i<int_fields;i++) 
			buffer[i] = fileBuffer[bufferPointer++];
		pointer = 0;

		/*visit_count */
		switch (visit_count)  { 
			case 1: fprintf(fpoutMozPlaces,"%d\t", readBigEndian(buffer,pointer++,1)); 
				break;
			case 2: fprintf(fpoutMozPlaces,"%ld\t", readBigEndian(buffer,pointer,2));  
				pointer +=2; 
				break;
			case 3: fprintf(fpoutMozPlaces,"%ld\t", readBigEndian(buffer,pointer,3)); 
				pointer +=3; 
				break;
		}

		/* hidden. must be one byte  choose not to show this. just skip*/
		pointer++;

		/* typed. must be one byte */
		c = buffer[pointer++];			
		if (c == 1) fprintf(fpoutMozPlaces,"yes\t");
		else 
			if (c == 0) fprintf(fpoutMozPlaces,"no\t");
			else
				fprintf(fpoutMozPlaces,"ALERT: Overwritten record.\t");

		/* favicon:  choose not to show. just skip*/
		pointer += favicon;

		/* frecency */
		switch (frecency)  { 
			case 1:  
				c = buffer[pointer++];	/*	frecency can be negative. no conversion is needed for one byte */
				fprintf(fpoutMozPlaces,"%d\n", c);
				break;
			case 2: 
				fprintf(fpoutMozPlaces,"%ld\n", readBigEndian(buffer,pointer,2));  
				pointer +=2; 
				break;
			case 3: 
				fprintf(fpoutMozPlaces,"%ld\n", readBigEndian(buffer,pointer,3)); 
				pointer +=3; 
				break;
		}
		return 1;
	}
	else { /* http:// found is not a record. Seek to the next char */
		bufferPointer += 7; 
		return 0;
	}
}

int readMozHistoryVisit()
{
	char buffer[34];
	char *timeBuffer;
	int pointer,huffmanFlag,i, 
		session, 
		visit_type,
		visit_date,
		place_id,
		from_visit,
		zero,
		headerSize, id, recordSize;
	time_t time;
	
	timeBuffer = (char *) malloc(30);

		/*
		The record from moz_history_visits is composed by:
		Record size: 1 byte in huffman coding
		ID: 1 to 3 bytes in huffman coding
		Header: 7 bytes  
			Header size: 1 byte (always 7)
			byte 0x00
			from_visit: 1 byte - value between 1 to 3 (indicates how many bytes the from_visit field data occupies
			place_id: 1 byte - value between 1 to 3 (indicates how many bytes the field data occupies
			visit_date: 1 byte - value 6, what indicates 8 bytes of data
			visit_type: 1 byte - value 1 (field data occupies 1 byte
			session: 1 byte - value between 1 to 3 or 6 
		Data segment: sizes as indicated by header. 
			Minimum data segment size is: 1+1+8+1+1 = 12 bytes
			Max size: 3+3+8+1+8 = 23 bytes

		The record size does not include record size itself and ID. Includes: the sum of header (always 7) + data segment
		MIN record size: 7+12= 19 bytes
		MAX record size: 7+23= 30 bytes
		*/


	/* the file pointer is pointing to the next byte after the header.
	   We will seek back 11 bytes: the header (7 bytes) + ID (MAX 3 bytes) + record size (1 byte)
	   and read 34 bytes (34 bytes: ID(3)+record size(1)+header(7)+data(23) */
	bufferPointer -= 11; 
	if(bufferPointer < 0) {/* we need to correct the file buffer */
		_lseeki64(fp, bufferPointer - FILE_BUFFER , SEEK_CUR);
		_read(fp, fileBuffer, FILE_BUFFER);
		bufferPointer = 0;
	}
	for(i=0;i<34;i++) 
			buffer[i] = fileBuffer[bufferPointer++];
	
	pointer = 10;
	session = buffer[pointer];
	visit_type = buffer[pointer-1];
	visit_date = buffer[pointer-2];
	place_id = buffer[pointer-3];
	from_visit = buffer[pointer-4];
	zero = buffer[pointer-5];
	headerSize = buffer[pointer-6];
	pointer -=7;

	
	/* read id*/
	id = buffer[pointer--];

	huffmanFlag = buffer[pointer--];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if(huffmanFlag < 128 ) 	/* id uses only one byte */
		recordSize = huffmanFlag;
	else {
		id = (huffmanFlag - 128) * 128 + id;
		huffmanFlag = buffer[pointer--];
		if(huffmanFlag < 0 ) 
			huffmanFlag += 256;
		if(huffmanFlag < 128 )  /* id uses two byte */
			recordSize = huffmanFlag;
		else {					/* id uses 3 bytes */
			id = (huffmanFlag - 128) * 16384 + id;
			recordSize = buffer[pointer];
			if(recordSize < 0 ) 
				recordSize += 256;
		}
	}

	if (recordSize>=19 && recordSize<=30) { /* found record*/
		totalRecFound++;
		fprintf(fpoutMozHistoryVisit,"%ld\t", id);
		pointer=11;
		switch (from_visit)  { /* read from_visit */
			case 1: fprintf(fpoutMozHistoryVisit,"%d\t", readBigEndian(buffer,pointer++,1)); 
				break;
			case 2: fprintf(fpoutMozHistoryVisit,"%ld\t", readBigEndian(buffer,pointer,2)); 
				pointer +=2; 
				break;
			case 3: fprintf(fpoutMozHistoryVisit,"%ld\t", readBigEndian(buffer,pointer,3)); 
				pointer +=3; 
				break;
		}

		switch (place_id)  { /* read place_id*/				
			case 1: fprintf(fpoutMozHistoryVisit,"%d\t", readBigEndian(buffer,pointer++,1)); 
				break;
			case 2: fprintf(fpoutMozHistoryVisit,"%ld\t", readBigEndian(buffer,pointer,2)); 
				pointer +=2; 
				break;
			case 3: fprintf(fpoutMozHistoryVisit,"%ld\t", readBigEndian(buffer,pointer,3)); 
				pointer +=3; 
			break;
		}

		/*read visit_date */
		time = readBigEndian(buffer,pointer,8);
		fprintf(fpoutMozHistoryVisit,"%lld\t", time);	
		time = (long long) time/1000000;
		timeBuffer = ctime(&time);
		timeBuffer[24] = '\x0';
		fprintf(fpoutMozHistoryVisit,"%s\t", timeBuffer);		
		pointer += 8;

		/* read visit_type - always 1 byte*/ 
		fprintf(fpoutMozHistoryVisit,"%d\t", readBigEndian(buffer,pointer++,1)); 

		switch (session)  { /* read session*/
			case 1: fprintf(fpoutMozHistoryVisit,"%d\n", readBigEndian(buffer,pointer++,1)); 
				break;
			case 2: fprintf(fpoutMozHistoryVisit,"%ld\n", readBigEndian(buffer,pointer,2)); 
				pointer +=2; 
				break;
			case 3: fprintf(fpoutMozHistoryVisit,"%ld\n", readBigEndian(buffer,pointer,3)); 
				pointer +=3; 
				break;
			case 6:
				fprintf(fpoutMozHistoryVisit,"%lld\n", readBigEndian(buffer,pointer,8));				
				pointer += 8;
				break;
		}
		position += pointer-11;
		if(pointer!=34)
			bufferPointer += pointer - 34; /* may be, the data did not occupy all the 34 bytes, we must seek back to the next char after the record  */
		return 1;
	} 
	else  {/* record not found */
		bufferPointer -= 19; /* seek to the char after the false header  */
		return 0;
	}
}


/*
this function checks the integrity of a moz_formhistory record.
It reads back the ID and record size, and verify if record size is consistent.

headerSize consists of: 
  2 (1byte for header size+1 byte for 0x00) + size of fieldname + size of value  
*/
int checkForm(int fieldName, int value, int headerSize, int & id, int& recordSize)
{

	int huffmanFlag, pointer, calcRecSize;
	
	calcRecSize = headerSize + fieldName + value;	/* record size that we calculate */
	pointer = bufferPointer - 3; /* pointer is now pointing to ID */

	/* read id*/
	id = fileBuffer[pointer--];
	huffmanFlag = fileBuffer[pointer];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	if(huffmanFlag < 128 ) { /* id uses only one byte */
		recordSize = huffmanFlag;
		if (calcRecSize == recordSize) { /* consistent with 1 byte for id  and record size*/
			//fprintf(fpoutMozFormHistory,"%d \t", id); /* print id with 1 byte */
			return 1;
		}
		else {
			huffmanFlag = fileBuffer[pointer-1];
			if(huffmanFlag < 0 ) 
				huffmanFlag += 256;
			recordSize = (huffmanFlag - 128) * 128 + recordSize;
			if (calcRecSize == recordSize) {/* consistent with 1 byte for id  and 2 bytes for record size*/
				//fprintf(fpoutMozFormHistory,"%d \t", id); /* print id with 1 byte */
				return 1;
			}
			else return 0;
		}
	}

	id = (huffmanFlag - 128) * 128 + id;
	huffmanFlag = fileBuffer[--pointer];
	if(huffmanFlag < 0 ) 
		huffmanFlag += 256;
	recordSize = huffmanFlag;
	if (calcRecSize == recordSize) { /* consistent with 2 bytes for id  and 1 byte record size*/
		//fprintf(fpoutMozFormHistory,"%d \t", id); /* print id */
		return 1;
	}
	else {
		huffmanFlag = fileBuffer[pointer-1];
		if(huffmanFlag < 0 ) 
			huffmanFlag += 256;
		recordSize = (huffmanFlag - 128) * 128 + recordSize;
		if (calcRecSize == recordSize) {/* consistent with 2 byte for id  and 2 bytes for record size*/
			//fprintf(fpoutMozFormHistory,"%d \t", id); /* print id */
			return 1;
		}

		/* test id with 3 bytes */
		huffmanFlag = fileBuffer[pointer--];
		if(huffmanFlag < 0 ) 
			huffmanFlag += 256;
		if (huffmanFlag > 127 )	{/* it큦 a huffman code. Calculate value of id with 3 bytes. */
			id = (huffmanFlag - 128) * 16384 + id;
			huffmanFlag = fileBuffer[pointer];
			if(huffmanFlag < 0 ) 
				huffmanFlag += 256;
			recordSize = huffmanFlag;
			if (calcRecSize == recordSize) { /* consistent with 3 bytes for id  and 1 byte record size*/
			//	fprintf(fpoutMozFormHistory,"%d \t", id); /* print id */								
				return 1;
			}
			else {
				huffmanFlag = fileBuffer[pointer-1];
				if(huffmanFlag < 0 ) 
					huffmanFlag += 256;
				recordSize = (huffmanFlag - 128) * 128 + recordSize;
				if (calcRecSize == recordSize) {/* consistent with 3 byte for id  and 2 bytes for record size*/
					//fprintf(fpoutMozFormHistory,"%d \t", id); /* print id */
					/* fprintf(fpoutMozFormHistory,"%d \t", recordSize); /* print record size */
					return 1;
				}
			}
		}

	}
	return 0;
}

int readMozFormHistory()
{
	int id, recordSize, huffmanFlag, pointer, fieldName, value, flag,i, sizeField=0, sizeValue=0;
	char fName[200], buffer[2000];

	/* 
	OBS: 
	1 byte in huffman coding allows for a value up to 127. 
	2 bytes in huffman coding allows for a value up to 16384. 

	The record from moz_formhistory is composed by:
		Record size: 1 or 2 bytes in huffman coding
		ID: 1 to 3 bytes in huffman coding
		Header: 7 bytes  
			Header size: 1 byte. MIN:4, if fieldname and value occupy 1 byte, each; MAX:6, if fieldname and value occupy 2bytes, each
			byte 0x00
			fieldName: 1 or 2 bytes in huffman coding
			value: 1 or 2 bytes in huffman coding
		Data segment: sizes as indicated by header. 
	
	the pattern found was: first byte 4|5|6, second byte 0, + 5 bytes not tested.
	We will seek back 5 bytes to read fieldName and value sizes. */
	
	bufferPointer -= 5;
	pointer = bufferPointer;

	fieldName = fileBuffer[pointer++];
	sizeField++;
	if(fieldName < 0 ) 
		fieldName += 256;
	if(fieldName >= 128 ){  /* fieldName uses more than one byte */
		sizeField++;
		huffmanFlag = fileBuffer[pointer++];
		if(huffmanFlag < 0 ) 
			huffmanFlag += 256;
		fieldName = (fieldName - 128) * 128 + huffmanFlag;
		}
	value = fileBuffer[pointer++];
	sizeValue++;
	if(value < 0 ) 
		value += 256;
	if(value >= 128 )  { /* value uses more than one byte */
		sizeValue++;
		huffmanFlag = fileBuffer[pointer++];
		if(huffmanFlag < 0 ) 
			huffmanFlag += 256;
		value = (value - 128) * 128 + huffmanFlag;
		}

		/* test if it is an odd number and  > 13 */
	if ( (fieldName < 13) || (value < 13 ) || (fieldName % 2 == 0) || (value % 2 ==0)) {
		bufferPointer += 5;		
		return 0;
	}
	/* calculate the real  value */
	fieldName = (fieldName - 13) / 2;
	value = (value - 13) / 2;

	/* here we impose some limits in fieldName and value lenght */
	if(fieldName > 100 || value > 500 || fieldName == 0 ){
		bufferPointer += 5;		
		return 0;
	}

	/* the checkForm function checks the integrity of a moz_formhistory record.
	        (int fieldName, int value, int headerSize, int & id, int& recordSize) */ 
	flag = checkForm(fieldName, value, sizeField+sizeValue+2,id, recordSize);
	if (flag==0){
		bufferPointer += 5;		
		return 0;
	}
	bufferPointer += sizeField+sizeValue;
	
	/* check if these fields will overflow the buffer */
	if (fieldName+value+bufferPointer >= FILE_BUFFER) {
		readFileBuffer();
		bufferPointer = 0;
	}
	for(i=0;i<fieldName;i++) {
			fName[i] = fileBuffer[bufferPointer++];
			if(fName[i] >= 0 && fName[i] < 32 ) {// non printable character - false positive
				bufferPointer += 5-sizeField-sizeValue-i+1; // bufferPointer will be the original value (+ 5 - what we have added)
				if(bufferPointer<0) bufferPointer = 0; // we have to correct position here
				return 0;			 
			}
	}	
	fName[fieldName] = '\x0';
	for(i=0;i<value;i++) {
		buffer[i] = fileBuffer[bufferPointer++];
		if(buffer[i] >= 0 && buffer[i] < 32 ) { // non printable character - false positive
				bufferPointer += 5-sizeField-sizeValue-fieldName-i+1; // bufferPointer will be the original value (+ 5 - what we have added)
				if(bufferPointer<0) bufferPointer = 0;// we have to correct position here
				return 0;			 
			}			
	}
	buffer[value] = '\x0';
	fprintf(fpoutMozFormHistory,"%ld \t", id); /* print id */
	fprintf(fpoutMozFormHistory,"%s\t", fName);
	fprintf(fpoutMozFormHistory,"%s\n", buffer);
	position += sizeField+sizeValue+fieldName+value-5;
	return 1;
}



/* try to read a moz_downloads record */
int readMozDownloads()
{
	char http[8];
	int i, pointer, calcRecSize, zero, headerSize, id, recordSize, tempPath, target, source, name, idSize, 
		referrer,entityID,currBytes,maxBytes,mimeType,preferredApplication,	preferredAction,autoResume;
	time_t time;
	char *timeBuffer;

	timeBuffer = (char *) malloc(30);

	/* The record from moz_downloads is composed by:
	Record size: 1 or 2 bytes in huffman coding
		ID: 1 to 3 bytes in huffman coding
		Header: ???? bytes  
			Header size: 1 byte: VALUE between MIN: 1+1+1+1+1+1+1+1+0+1+1+1+1+1+1+1 = 15 MAX: 1+1+2+2+2+2+1+1+1+2+2+1+1+1+2+1+1=24
			byte 0x00
			name TEXT: file name - 1 or 2 bytes
			source TEXT: url - 1 or 2 bytes
			target TEXT: full path - 1 or 2 bytes
			tempPath TEXT: - 1 or 2 bytes
			startTime INTEGER: 1 byte - value 6, what indicates 8 bytes of data (prtime format)
			endTime INTEGER: 1 byte - value 6, what indicates 8 bytes of data (prtime format)
			state INTEGER: 1 byte - value 1,  what indicates a 1 byte integer
			referrer TEXT: url - 1 or 2 bytes
			entityID TEXT: ???1 or 2 bytes
			currBytes INTEGER: 1 bytes - value 1 to 5,  what indicates a integer up to 6 bytes (max size 281474976710656)
			maxBytes: 1 bytes - value 1 to 5, 
			mimeType TEXT: 1 byte 
			preferredApplication TEXT:  1 or 2 bytes
			preferredAction INTEGER NOT NULL DEFAULT 0: 1 byte - value always 0 ???
			autoResume INTEGER NOT NULL DEFAULT 0: 1 byte - value always 0 ???
		Data segment: sizes as indicated by header. 
	
	the pattern found was: 661 (starttime/endtime/state) + 4 bytes not tested.
	We will seek back 7 bytes of pattern to read tempPath to record size. */
	
	if(bufferPointer < 20) {/* we need to correct the file buffer, because we will read backwards */
		_lseeki64(fp, -20+bufferPointer - FILE_BUFFER , SEEK_CUR);
		_read(fp, fileBuffer, FILE_BUFFER);
		bufferPointer = 20;
	}
	pointer = bufferPointer - 8;

	calcRecSize = 3+pointer; // 3 for the 661 + an initial value

	// read values and correct text values 
	tempPath = (readUpTo2BytesBack(pointer)-13)/2;
	target = (readUpTo2BytesBack(pointer)-13)/2;
	source = (readUpTo2BytesBack(pointer)-13)/2;
	name = (readUpTo2BytesBack(pointer)-13)/2;
	zero = fileBuffer[pointer--];
	headerSize = fileBuffer[pointer--];
	if(!(zero==0 && headerSize >= 15 && headerSize <= 24)){
		return 0; // byte zero or header size is not consistent
	}
	calcRecSize -= pointer; // remove the initial value. it doesnot include the record size itself 
	idSize = pointer;
	id = readUpTo3BytesBack(pointer);
	idSize -= pointer;	
	recordSize = readUpTo2BytesBack(pointer);	
	pointer = bufferPointer - 4; // return pointer to after 661
	calcRecSize -= pointer;
	referrer = readUpTo2BytesFront(pointer); //referrer can be null
	if(referrer>0) referrer = (referrer-13)/2;
	entityID  = (readUpTo2BytesFront(pointer)-13)/2;
	currBytes = fileBuffer[pointer++];
	maxBytes = fileBuffer[pointer++];
	mimeType  = fileBuffer[pointer++]; 
	if(mimeType<0) mimeType += 256;
	mimeType = (mimeType-13)/2;
	preferredApplication = (readUpTo2BytesFront(pointer)-13)/2;
	preferredAction = fileBuffer[pointer++];
	autoResume = fileBuffer[pointer++];
	calcRecSize += pointer; // now calcRecSize should be the header size + id
	if(calcRecSize != headerSize)
		return 0;
	if(!(currBytes>=1 && currBytes <= 5 && maxBytes >= 1 && maxBytes <= 5)){
		return 0; // currBytes or maxBytes is not consistent
	}
	if(source < 7 || target < 7 || name < 1 )
		return 0; // not consistent
	if(currBytes==5) // a value of 5 indicates a 6 byte long integer
		currBytes = 6;
	if(maxBytes==5) // a value of 5 indicates a 6 byte long integer
		maxBytes = 6;
	// sum the data fields to calcRecSize and verify consistency
	calcRecSize += tempPath + target + source +	name + referrer +entityID +	currBytes+maxBytes+	mimeType+preferredApplication+preferredAction+autoResume+8+8+1; //the 8+8+1 is for startTime, endTime and state 
	if(calcRecSize != recordSize)
		return 0;
	
	if(pointer+name+6 >= FILE_BUFFER)  
		return 0; // if name is very long (~200bytes) AND is located in fileBuffer limit, it will not be recovered. This is a very unusual situation. It may happen because we need to test the 6 bytes of source following the name field
	// test if the first bytes of source field is http:// or something like this
	for(i=0;i<6;i++) 
		http[i] = fileBuffer[pointer+i+name];
	http[6] = '\x0';
	if (! ((strcmp(http, "http:/") == 0 ) ||
		   (strcmp(http, "https:") == 0 ) ||
		   (strcmp(http, "file:/") == 0 ) ||
		   (strcmp(http, "ftp://") == 0 ) ) ) {
			   return 0;// source doesnot begins with http: => false positive
	}
	if(calcRecSize>FILE_BUFFER){
		printf("\n Warning: FILE_BUFFER is too small. moz_download record not recovered \n");
		return 0; // the record (header + data) is bigger than FILE_BUFFER! This should not happen with the default value of FILE_BUFFER=4000
	}

	// if we get here it is a valid record
	position += pointer-bufferPointer; // increment position by what was read
	bufferPointer = pointer;

	/* check if data fields will overflow the buffer */
	if (bufferPointer+calcRecSize> FILE_BUFFER) {
		readFileBuffer();
		bufferPointer = 0;
	}
	
	fprintf(fpoutMozDownloads,"%ld\t", id); /* print id */
	printFileBufferChar(name, fpoutMozDownloads,'\t');
	printFileBufferChar(source, fpoutMozDownloads,'\t');
	printFileBufferChar(target, fpoutMozDownloads,'\t');
	printFileBufferChar(tempPath, fpoutMozDownloads,'\t');
	/*read startTime */
	time = readBigEndian(fileBuffer,bufferPointer,8);
	fprintf(fpoutMozDownloads,"%lld\t", time);	
	time = (long long) time/1000000;
	timeBuffer = ctime(&time);
	timeBuffer[24] = '\x0';
	fprintf(fpoutMozDownloads,"%s\t", timeBuffer);		
	bufferPointer += 8;
	/*read endTime */
	time = readBigEndian(fileBuffer,bufferPointer,8);
	fprintf(fpoutMozDownloads,"%lld\t", time);	
	time = (long long) time/1000000;
	timeBuffer = ctime(&time);
	timeBuffer[24] = '\x0';
	fprintf(fpoutMozDownloads,"%s\t", timeBuffer);		
	bufferPointer += 8;

	fprintf(fpoutMozDownloads,"%d\t", readBigEndian(fileBuffer,bufferPointer++,1)); // state -header value always 1 (1byte integer)
	printFileBufferChar(referrer, fpoutMozDownloads,'\t');
	printFileBufferChar(entityID, fpoutMozDownloads,'\t');
	fprintf(fpoutMozDownloads,"%lld\t", readBigEndian(fileBuffer,bufferPointer,currBytes)); 
		bufferPointer += currBytes;
	fprintf(fpoutMozDownloads,"%lld\t", readBigEndian(fileBuffer,bufferPointer,maxBytes)); 
		bufferPointer += maxBytes;
	printFileBufferChar(mimeType, fpoutMozDownloads,'\t');
	printFileBufferChar(preferredApplication, fpoutMozDownloads,'\t');
	fprintf(fpoutMozDownloads,"%d\t", readBigEndian(fileBuffer,bufferPointer++,preferredAction)); 
	fprintf(fpoutMozDownloads,"%d\n", readBigEndian(fileBuffer,bufferPointer++,autoResume)); 
	return 1;
}
/* this function reads the file and makes the first tests */
void processInput()
{
	int seek, flag,i, totalGB=0,
		session, 
		visit_type,
		visit_date,
		place_id,
		from_visit,
		zero,
		headerSize;
	char buffer[8], p_ftp[8];
	long long fileSize;

	fileSize = _lseeki64(fp, 0, SEEK_END); /* position in the end of file to get its size */
	position = _lseeki64(fp, 0, SEEK_SET); /* back to the begin of file*/
	bufferPointer = FILE_BUFFER;
	do { 	
		if (bufferPointer > FILE_BUFFER-200) {
			readFileBuffer();
			bufferPointer = 0;
		}
		for(i=0;i<7;i++) 
			buffer[i] = fileBuffer[bufferPointer++];

		position += 7; /* keep file position to status message */
		buffer[7] = '\x0';
		strcpy_s(p_ftp, buffer);
		p_ftp[6] = '\x0';
				
		/* the following variables are the fields from moz_historyvisits */
		session = buffer[6];
		visit_type = buffer[5];
		visit_date = buffer[4];
		place_id = buffer[3];
		from_visit = buffer[2];
		zero = buffer[1];
		headerSize = buffer[0];
		flag = 0;
		if( (strcmp(buffer, "http://") == 0 ) ||
			(strcmp(buffer, "https:/") == 0 ) ||
			(strcmp(buffer, "file://") == 0 ) ||
			(strcmp(p_ftp, "ftp://") == 0 ) )  /* found  http://   or https:/  or ftp */
			flag = readMozPlaces(); /* reads a moz_places record. returns 0 if it is not a record, otherwise returns 1 */
		else
			if( (headerSize == 7)&& (zero == 0) && (from_visit >= 1) &&(from_visit <= 3) && (place_id >= 1) &&
			(place_id <= 3) && (visit_date == 6) && (visit_type == 1) && (session >= 1 ) && (session <= 6) ) 
				flag = readMozHistoryVisit(); /* reads a moz_historyvisits record. returns 0 if it is not a record, otherwise returns 1 */
			else 
				if( zero  == 0 && (headerSize >= 4)&& (headerSize <= 6))
					flag = readMozFormHistory();
				else 
					if((headerSize == 6)&& (zero == 6) && (from_visit == 1) )
						flag = readMozDownloads();
		
		if(flag==0) { /* no record was read. We need to seek back */
			
			/* this code was used when only moz_places and moz_historyvisits were processed, 
			because their pattern was 7 bytes long and we could test to seek less than 6.
			Now, as moz_formhistory uses a 2 byte pattern, we have to go byte a byte.

			seek = 0;
			switch (buffer[6])  {
				//* seek depending on http://   or https:/  or ftp:// or file ://*
				case 'h': seek = -1; break;
				case 'f': seek = -1;break;
				case 'i': seek = -2; break;
				case 'l': seek = -3; break;
				case 'e': seek = -4; break;
				case 't': seek = -3; break;
				case 'p': seek = -4; break;
				case ':': seek = -6; break;
				case 's': seek = -5; break;
				case '/': seek = -6; break;
			}
			test = buffer[6];
			switch(test){
				//* seek depending on MozHistoryVisit fields *
				case 7: seek = -1; break;
				case 0: seek = -2; break;
				case 2: seek = -4; break;
				case 3: seek = -4; break;
				case 6: seek = -5; break;
				case 1: seek = -6; break;
			} */

			seek = -6;
			position += seek;	
			bufferPointer += seek; 
			if(bufferPointer < 0) {/* we need to correct the file buffer */
				_lseeki64(fp, bufferPointer , SEEK_CUR);
				_read(fp, fileBuffer, FILE_BUFFER);
				bufferPointer = 0;
			}
		}

		if( (int)(position/1000000) > totalGB){
			totalGB = (int)(position/1000000);
			printf("Read: %dMB --- Recovered %d records\r",totalGB, totalRecFound);
		}
	} while(position<fileSize-7);/* end of do */
}

void usage()
{
	printf("Firefox 3 History Recovery\n by Murilo Tito Pereira (murilo.mtp at dpf.gov.br)");
	printf("\n\nff3hr is a forensic tool to recover deleted history records from Firefox 3. \n");
	printf("This browser uses various SQLite databases to store the history, \n");
	printf("and this tool can search and recover records from moz_places, moz_historyvisits, moz_formhistory and moz_downloads tables in a whole disk image.\n");
	printf("\n\nUsage: ff3hr -i <input file>\n");
	printf("Searches for Firefox 3 history record in input file.\n");
	printf("Prints results in following output files: ff3hr-mozplaces.txt, ff3hr-mozhistoryvisits.txt, ff3hr-mozformhistory.txt and ff3hr-mozdownloads.\n");
	
}

void main(int argc, char *argv[])
{
	int flag;
	
	if(argc == 1 ) { /* no arguments */
		usage();
		exit(0);
	}

	/* one argument: help */
		if( (argc == 2) && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"/?") || !strcmp(argv[1],"--help") )) {
			usage();
			exit(0);
		}

	/* two arguments: define input */
	if(argc == 3) {
		if ( !strcmp(argv[1],"-i") ) {
			flag = _sopen_s(&fp, argv[2], _O_RDONLY | _O_BINARY | _O_SEQUENTIAL, _SH_DENYWR, _S_IREAD);
			if(flag==-1) { /* file doenst exist */
				printf("\n\nError opening input file");
				exit(0);
			}
			fpoutMozPlaces = fopen("ff3hr-mozplaces.txt","w");
			if(fpoutMozPlaces==NULL) {
				printf("\n\nError opening output file ff3hr-mozplaces.txt");
				exit(0);
			}			
			fpoutMozHistoryVisit = fopen("ff3hr-mozhistoryvisits.txt" ,"w");
			if(fpoutMozHistoryVisit==NULL) {
				printf("\n\nError opening output file ff3hr-mozhistoryvisits.txt");
				exit(0);
			}
			fpoutMozFormHistory = fopen("ff3hr-mozformhistory.txt" ,"w");
			if(fpoutMozFormHistory==NULL) {
				printf("\n\nError opening output file ff3hr-mozformhistory.txt");
				exit(0);
			}
			fpoutMozDownloads = fopen("ff3hr-mozdownloads.txt" ,"w");
			if(fpoutMozDownloads==NULL) {
				printf("\n\nError opening output file ff3hr-mozformhistory.txt");
				exit(0);
			}
		}
		else {
			usage();
			exit(0);
		}
	}
	else {
		usage();
		exit(0);
		}

	fprintf(fpoutMozHistoryVisit,"ID\tfrom_visit\tplace_id\tRaw visit_date\tConverted visit date\tVisit type\tsession\n");
	fprintf(fpoutMozPlaces,"ID\tURL\tTitle\tRev_host\tVisit Count\tTyped?\tFrecency\n");
	fprintf(fpoutMozFormHistory,"ID\tField Name\tValue\n");
	fprintf(fpoutMozDownloads,"ID\tname\tsource\ttarget\ttempPath\tstartTime RAW\tstartTime\t");
	fprintf(fpoutMozDownloads,"endTime RAW\tendTime\tstate\treferrer\tentityID\tcurrBytes\tmaxBytes\tmimeType\tpreferredApplication\tpreferredAction\tautoResume\n");
	processInput();
	_close(fp);
	fclose(fpoutMozPlaces);
	fclose(fpoutMozHistoryVisit);
	fclose(fpoutMozFormHistory);
	fclose(fpoutMozDownloads);
}