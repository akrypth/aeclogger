#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <malloc.h>
#include <zlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/klog.h>


#define FMT_RAW 0
#define FMT_HTML 1

// codes for AEConversion-protocol
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		#define GET_PARAMETER 1014
		#define GET_BETRIEBSDATEN 1005
		#define GET_LEISTUNGSREDUZIERUNG 1023
		#define GET_ERTRAGSDATEN 1021
		#define GET_STATUS 1008
		#define GET_LAENDER 1007
		
		#define SET_LEISTUNGSREDUZIERUNG 1022
		#define SET_SCHUECOID 1015
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


#define F_BETRIEB 0  
#define F_BITBETRIEB 1
#define F_BITSTOERUNG 2
#define F_BITFEHLER 3
#define F_SCHUECO 4
#define F_LAENDER 5
#define F_PARAM 6
#define F_STATISTIK 7

const char Version[] = "0.19";
const double sqr2 = 1.414213562373095;


// struct / variables for html.inc
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		typedef struct  {
		 long pos;
		 long len;
		} s_tidx;
		
		s_tidx * tindex=NULL;
		int maxtindex;
		long dstate;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


typedef void* (*tcphandler) ( char, char *);
#define MAXTCPHANDLER 30   
struct s_tcphd {
   int         fd;  
   tcphandler  handler;   // see typedef -> callbackfunction
   char*       param;
  } tcphd[MAXTCPHANDLER];  

int log_interval=10;
int req_interval=10;
int pow_interval=5;

int listenport=3038;
char * serialport;
char * serialidentifier;
char * program_path = NULL;
char default_path[] = "./";
char * param_path = NULL;
char * tmp_path;
char * log_path;
time_t startuptime;	
time_t tslastserial;
int serialid=1000;

int datalog_active=0;
int logit = 0;

long detailHourColor=0x000099;
long detailLiveColor=0x002299;

int globalgetsize;

char stimebuffer[30];

char gzfname[50];

char daylogname[19];

char * host;
char * sbuf;  //serial buffer reeadin

int AENumDevices;
int AEDeviceIndex;
int AELeistungsregelung = -1;

long ExternLiveValueTs=0;;
double ExternLiveValue;

int dontfork=0;
int serialactive = 0;

long logtime;

// function prototypes
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		int AE_Request (int, unsigned short type, int value);
		void AE_Request_Schueco (int);
		char * AE_Print_Schueco (int, int);
		char * AE_Print_Betriebsdaten (int, int);
		char *  AE_Print_Laender (int,int);
		char *  AE_Print_Parameter (int,int);
		char * AE_Print_Bit_Betrieb (int, int);
		char * AE_Print_Bit_Stoerung (int, int);
		char * AE_Print_Bit_Fehler (int, int);
		char * AE_Print_Statistikdaten (int);
		void LoadLogArchive (int, int);
		int get_offlinestate (int);
		void write_offlinepage (int,int);
		void write_overviewpage (int);
		void write_reduce(int,int, int);
		void saveDayLog (int, int);
		void saveLiveLog (int , int);
		void init_fdarrays();
    unsigned long millis (void);		
		void printQueue (void); 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



int * AEDevices;
int * SchuecoID;
char * *AEDevicesComment;

// Serial (USB) handler
int serialfd;                    									// handler for serial device

// Http communication
struct sockaddr_in eventserv_addr, eventcli_addr;  	// socket info for http
int eventfd, newevfd, eventclilen; 	                // sockethandlers for select
fd_set active_fd_set, read_fd_set;  

#define MEANINTERVAL 15

// logfile
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct s_logfile {
			char * fname;
			int deviceindex;
			int isintmp;
			time_t ts;
			int logday;
			int logmonth;
			int logyear;
			int logweek;
			float yield;
			long len;
			long max;
			struct sAEDayLog * aptr;
		} * loglist = NULL;
		
		int loglistentries = 0;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// Struct for devicedata (AEC)
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  struct sAEData{
		time_t AE_Lastrequest_ts;   //letzter erfolgreicher request   
    double AE_external_power;
    double AE_Hour_state[24];
    double AE_Hour_yield[24];    
    time_t AE_Hour_state_offset;    
    int AE_Offline;
    time_t AE_Offline_ts;
    time_t AE_Online_ts;
    time_t AE_Wakeup_ts;
    int AE_Offline_ct;
    // Ertragsdaten
      time_t AE_TS_Current;
  		double AE_current_power;
	  	double AE_today_yield;
    // Betriebsdaten
      time_t AE_TS_Operations;
			double AE_Ampl_AC_Spannung;
			double AE_Ampl_AC_Strom;
    	double AE_PV_DC_Strom;
    	double AE_PV_DC_Spannung;
    	double AE_PV_DC_Leistung;
    	double AE_PV_AC_Leistung;
    unsigned long AE_ChartColor;
    double AE_Temperatur;

		// AEC device-parameters
      time_t AE_TS_Parameter;
	    int    AE_ID;
	    char   AE_Typ[9]; 
	
	    int    AE_Schueco;
	    int    AE_LCode;     
			
			int    AE_CRC_Aktiv;
			int    AE_CRC_Backup;
			int    AE_Bootcode;
	    int    AE_LCode_Aenderung;
	    int    AE_LCode_Fehler;
	    int    AE_Anzahl_Boot; 
	    int    AE_Anzahl_Codefehler;		
	    char   AE_Version_SW[9]; 
	    char   AE_Version_HW[9]; 
	    double AE_Max_Leistung;
			long   AE_error;

    //AEC status-LWORDS 
	    time_t AE_TS_Status;
	    int    AE_Status_Betrieb;
	    int    AE_Status_Fehler;
	    int    AE_Status_Stoerung;
    
      double AE_Reduzierung_set;      
      double AE_Reduzierung;

    // AEC-country-settings
      time_t          AE_TS_Country;
		  double          AE_Nennspannung;
		  double          AE_Obere_Abschaltspannung;
		  unsigned short  AE_Abschaltzeit_Ueberspannung;
	    double          AE_Kritische_Ueberspannung;
	    unsigned short  AE_Unknown;
		  double          AE_Untere_Abschaltspannung;
		  unsigned short  AE_Abschaltzeit_Unterspannung;
		  double          AE_Nennfrequenz;
		  double          AE_Untere_Abschaltfrequenz;
		  unsigned short  AE_Abschaltzeit_Unterfrequenz;
		  double          AE_Obere_Abschaltfrequenz;
		  unsigned short  AE_Abschaltzeit_Ueberfrequenz;
		  double          AE_Max_DC_Anteil;
		  unsigned short  AE_Abschaltzeit_Ueber_MAX_DC; 
		  unsigned short  AE_Abschaltzeit_Inselerkennung;
		  unsigned short  AE_Wiedereinschalt_Fehler_lt3;
		  unsigned short  AE_Wiedereinschalt_Fehler_gt3;
		  unsigned short  AE_Unterspannung_ATT;
		  unsigned short  AE_Ueberspannung_ATT;
		  unsigned short  AE_Ueberfrequenz_ATT;
		  unsigned short  AE_Unterfrequenz_ATT;

    // KACO / Schueco	  
		  time_t KA_Timestamp;
		  int    KA_Id;
		  int    KA_Status;
		  double KA_DC_U;
		  double KA_DC_I;
		  double KA_DC_P;
		  double KA_AC_U;
		  double KA_AC_I;
		  double KA_AC_P;
		  double KA_Temperatur;
		  double KA_Ertrag;

  } * AEData;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


// LiveLog data (rolling buffer)
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct sAELiveLog {
			long ts;
			float reduce;
			float power;
			float value;
		} ** AELiveLog;
		
		long * AELiveLogPos;
		long * AELiveLogSize;
		int * AELiveLogCounter;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// DayLog !
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct sAEDayLog {
			long ts;
			float reduce;
			float yield;
			float power_DC;
			float power_AC;
			float voltage_DC;
			float current_DC;
			float voltage_AC;
			float current_AC;
			float temperature;
			float value;
			char  offline;
			int  opcode;
			int  discode;
			int  errcode;
		} ** AEDayLog;
		
		long * AEDayLogPos;
		long * AEDayLogSize;
		long * AEDayLogCounter;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


// helpers for data-conversion AEC
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		union clong {
			int  i;
			unsigned char b[4];
		 };
		
		union cshort {
			short  s;
			unsigned char b[2];
		 };
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// RS485-Queue
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		struct s_queue {
			
			int deviceindex;
			unsigned short type;
			int parameter;
		} * queue = NULL;
		
		int queueentries;
		int queuesize;
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



char * getTimeString(void)
{
	struct tm * timeinfo;
  long ts = time(NULL);	
	timeinfo = localtime(&ts);
	strftime (stimebuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);
	return stimebuffer;
}


//**********************************************************************


/************************************************\
    Kernel-Ringbuffer handling for USB
\************************************************/

int getUSBID ( char * identifier)
{
	char * rBuffer = NULL;
  int usbid;
	int len =  klogctl(10, NULL, 0);
	
	if (serialport != NULL)
		 free(serialport);
	serialport = malloc (20);	 
	
	printf("size %d\r\n",len);
	rBuffer = malloc(len);
	int rlen = klogctl(3,rBuffer,len);
	
	
  char delimiter[] = "\n";
  char *ptr;

  ptr = strtok(rBuffer, delimiter);

  while(ptr != NULL) {
	 
	  if (strstr(ptr,identifier)) {
	     char *p = strstr(ptr,"ttyUSB");
	     usbid = atoi(p+6);

	     }
 	  ptr = strtok(NULL, delimiter);
  }
	free(rBuffer);
	
	sprintf(serialport,"/dev/ttyUSB%d",usbid);
	if (serialid != usbid) {
		serialid = usbid;
		return 1;
	}
	return 0;
}



//**********************************************************************

/************************************************\
    RS485-queries  queue handling
\************************************************/

void printQueue (void) 
{
	int a;
	printf("Queuesize:%d %d\r\n",queuesize,queueentries);
	for (a=0; a<queuesize; a++){
	  printf("\t%d = %d [%d]\r\n",a,queue[a].deviceindex,queue[a].type);
	}
	fflush(stdout);
}


void createQueue (void)
{
	queuesize =  AENumDevices*10; //should be enough
	queue = calloc(sizeof(struct s_queue), queuesize); 
	queueentries = 0;
	int a; 
	for (a=0; a < queuesize; a++)
	  queue[a].deviceindex = -1;
}

void pushQueueEntry (int deviceindex, unsigned short type, int parameter)
{
	int a;
	for (a=0; a<queueentries; a++) 
	  if (queue[a].deviceindex == deviceindex && queue[a].type==type) 
	  	  return;
	
	queue[queueentries].deviceindex = deviceindex;
	queue[queueentries].type = type;
	queue[queueentries].parameter = parameter;
	queueentries++;
}

void pullQueueEntry (void)
{
	int a;
	for (a=1; a<queuesize; a++) {
		memcpy (queue+a-1, queue+a, sizeof(struct s_queue));
	}
	queueentries--;
	queue[queuesize-1].deviceindex=-1;
	queue[queuesize-1].type=0;
}

void CompactQueue (void)
{
	int a,b,found;
	for (a=0; a<queuesize-1; a++) {
	  if (queue[a].deviceindex == -1) {
	  	 found = 0; 
	  	 for (b=a+1; b<queuesize; b++) {
	  	   if (queue[b].deviceindex !=-1) {
	  	   	 found = 1;
	  	   	 break;
	  	     }
	  	   }
	  	 if (found) {
	  	 	  memcpy (queue+a, queue+b, sizeof(struct s_queue));
	  	 	  queue[b].deviceindex = -1;
	  	 	  queue[b].type=0;
	  	   }
	  	 else 
	  	 	 break;	
	     }
	  }
}

void DeleteDeviceEntriesFromQueue (int deviceindex)
{
	int a;
	for (a=0; a<queuesize; a++) {
		if (queue[a].deviceindex == deviceindex) {
		    queue[a].deviceindex=-1;
		    queue[a].type=0;
		    queueentries--;
		  }
	}
	//printQueue();
	CompactQueue();
}

void testQueue(void)
{
	printf("START\r\n");
	printQueue();
	DeleteDeviceEntriesFromQueue(1);
	printQueue();
}

/************************************************\
    GZIP - wrapper functions
\************************************************/

struct gzFile_s * open_gz (void)
{
	char tmp[50];
	snprintf(tmp,49,"%s%s",tmp_path, gzfname);
	struct gzFile_s * gfd = gzopen(tmp,"w9");
	return gfd;
}

void close_gz (struct gzFile_s * gzfd)
{
	gzclose(gzfd);
}

void send_gz (int fd)
{
	submit_file (fd, gzfname,2,NULL);
	unlink(gzfname);
}

/************************************************\
    controlled shutdown (save logs before)
\************************************************/

void ShutDown(void) 
{
	int a;
	for (a=0; a<AENumDevices; a++) {
   saveDayLog(a,0);
   saveDayLog(a,1);
   saveLiveLog(a,1);
   exit(0);
  }
}

/************************************************\
   calc yield per hour - for hourwise barchart
\************************************************/

void getHourYield(int idx)
{
 struct tm * timeinfo;
 int a;
 for (a=0; a<24; a++) {
   AEData[idx].AE_Hour_state[a]=0; 
   AEData[idx].AE_Hour_yield[a]=0;
  }
  
 int hr; 
 for (a=0; a<AEDayLogPos[idx];a++) {
	timeinfo = localtime(&AEDayLog[idx][a].ts);
 	hr = timeinfo->tm_hour;

 	if (AEData[idx].AE_Hour_state_offset == 0) 
       AEData[idx].AE_Hour_state_offset = (AEDayLog[idx][a].ts/3600)*3600-hr*3600;
 	if (AEData[idx].AE_Hour_state[hr] == 0) 
 		AEData[idx].AE_Hour_state[hr] = (float)AEDayLog[idx][a].yield;
 		AEData[idx].AE_Hour_yield[hr] = AEData[idx].AE_Hour_state[hr]-AEData[idx].AE_Hour_state[hr-1];
  }
  AEData[idx].AE_Hour_yield[hr+1] = AEDayLog[idx][AEDayLogPos[idx]-1].yield-AEData[idx].AE_Hour_state[hr];
}

/************************************************\
      write/send JSON yield per hour-data 
\************************************************/

void writeJSONHourstate(int fd, char * devicefield)
{
	struct gzFile_s * tmp = open_gz();
	int firstdevice;
	int device;
	int idx;
	
	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);

  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
  	   if (devicefield[device]=='1') {
            getHourYield(device);

  	   	     if (!firstdevice) 
  	   	     	 gzprintf(tmp,"},");
             gzprintf(tmp,"{\"id\": %d,\"data\":[",device);
  	   	     for (idx=0; idx<=timeinfo->tm_hour+1; idx++) {
  	   	     	  if (idx > 0) 
  	   	     	  	 gzprintf(tmp,",");
                gzprintf(tmp,"{\"ts\": %d,\"val\": %1.3f",AEData[device].AE_Hour_state_offset+idx*3600,AEData[device].AE_Hour_yield[idx]);    
   	     	   	  gzprintf(tmp,"}");  	     	      	  	   	     	      	
  	   	       }
  	   	 	     gzprintf(tmp,"]");
  	   	       firstdevice = 0;
  	   }
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}

/************************************************\
  write/send JSON device-data - init JS-main-doc 
\************************************************/

void writeJSONDeviceinfo(int fd)
{
	struct gzFile_s * tmp = open_gz();
	int firstdevice;
	int device;
	int idx;
	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);
	double sumpower = 0;
	double setsumpower = 0;
  for (device=0; device < AENumDevices; device++) {
  	sumpower += AEData[device].AE_Max_Leistung;
  	setsumpower += AEData[device].AE_Reduzierung;
   }


  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"sumpower\": %1.0f,\"setsumpower\": %1.0f, \"livecolor\": \"#%06X\" , \"hourcolor\": \"#%06X\" ,\"devices\":[",Version,
                                                                                             tbuffer,sumpower,setsumpower,detailLiveColor,detailHourColor);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
       if (!firstdevice) 
 	     	 gzprintf(tmp,"},");
       gzprintf(tmp,"{\"id\": %d,\"name\": \"%s\",\"maxpower\": %1.0f, \"setpower\": %1.0f, \"displaycolor\": \"#%06X\" ",device,AEDevicesComment[device],
                                                                                             AEData[device].AE_Max_Leistung,
                                                                                             AEData[device].AE_Reduzierung,
                                                                                             AEData[device].AE_ChartColor);
 	     firstdevice = 0;
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}

//************************************************

void writeJSONLogstates (int fd, int dev, int year, int value, int gran)
{
	struct gzFile_s * tmp = open_gz();
	int firstdevice;
  int firstentry;
	int device;
	int entry;
	int idx;
	
	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);

  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
  	 if (!(dev == -1 || dev == device)) continue;
  	   
             if (!firstdevice) gzprintf(tmp,"},");
  	   	     firstdevice = 0;
             firstentry = 1;
             gzprintf(tmp,"{\"id\": %d,\"data\": [",device);
            
  	   	     
  	   	     for (entry=0; entry < loglistentries; entry++) {
  	   	     	  //printf("entry %d : %d/%d %d %d\r\n",entry,loglist[entry].logyear,year,loglist[entry].logweek,loglist[entry].logmonth);
  	   	     	  if (loglist[entry].deviceindex==device && loglist[entry].logyear == year && (( gran == 1 && loglist[entry].logweek == value) || ( gran == 2 && loglist[entry].logmonth == value))) {
  	   	     	  	 if (!firstentry) gzprintf(tmp,"},");
  	   	     	  	 firstentry = 0;
  	   	     	  	 gzprintf(tmp,"{\"year\": %d,\"day\": %d,\"month\": %d,\"week\": %d,\"ts\": %d,\"yield\": %1.1f ",
  	   	     	  	          loglist[entry].logyear,
  	   	     	  	          loglist[entry].logday,
  	   	     	  	          loglist[entry].logmonth,
  	   	     	  	          loglist[entry].logweek,
  	   	     	  	          loglist[entry].ts,
  	   	     	  	          loglist[entry].yield);
  	   	     	    }
  	   	     	 
  	   	         
  	   	     }
  	   	     if (!firstentry) gzprintf(tmp,"}");
  	   	     	 gzprintf(tmp,"]");
  	   
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}
	
	


//************************************************

void writeJSONOnlinestate(int fd)
{
	struct gzFile_s * tmp = open_gz();
	int firstdevice;
	int device;
	int idx;
	
	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);

  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
  	   
             if (!firstdevice) 
  	   	     	 gzprintf(tmp,"},");
             gzprintf(tmp,"{\"id\": %d,\"online\": %d",device,AEData[device].AE_Offline == 1 ? 0 : 1);
  	   	 	 
  	   	     firstdevice = 0;
  	   
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}

//************************************************ 

void writeCSVArchiveLogData(int fd, char * devicefield, long ts)
{
 	struct tm * timeinfo;
	char tbuffer[50];
	char fname[50];
	struct sAEDayLog * logptr;
	int a, loglistindex;
  timeinfo=localtime (&ts);
  int m_day,m_month, m_year;
  m_day = timeinfo->tm_mday;
  m_month = timeinfo->tm_mon+1;
  m_year = timeinfo->tm_year+1900;

  int device;
  long logidx;
  long lts;
  struct gzFile_s * tmp = open_gz();
  int firstentry;
  int firstdevice;
  int step=1;
  
  snprintf(fname,49,"%02d.%02d.%04d (%ld).csv",m_day,m_month,m_year,time(NULL)%86400);
  gzprintf(tmp,"sep=\t\r\n");
  gzprintf(tmp,"Geraet\tDatum\tUhrzeit\tReduzierung\tErtrag\tPDC\tPAC\tVDC\tCDC\tVAC (eff.)\tCAC\tTemperatur\tExtern\tStatus\tStoerung\tFehler\r\n");
  for (device=0; device < AENumDevices; device++) {
  	   if (devicefield[device]=='1') {
  	   	     loglistindex = -1; 
  	   	     for (a=0; a<loglistentries; a++) {
  	   	     	  if (loglist[a].deviceindex==device && loglist[a].logday == m_day && loglist[a].logmonth== m_month && loglist[a].logyear== m_year) {
                    loglistindex=a;
                    LoadLogArchive (a,1);
                    logptr = loglist[a].aptr;
                    break;  	   	     	  	
  	   	     	     }
  	   	       }
  	   	     if (loglistindex == -1) continue;  
  	   	     for (logidx=0; logidx<loglist[loglistindex].len; logidx++) {
		  	   	     	gzprintf(tmp,"%d\t",device);
		  	   	     	timeinfo = localtime(&logptr[logidx].ts);
			            strftime (tbuffer,49,"%d.%B.%Y",timeinfo);
		  	   	     	gzprintf(tmp,"%s\t",tbuffer);
		  	   	     	strftime (tbuffer,49,"%H:%M:%S",timeinfo);
		  	   	     	gzprintf(tmp,"%s\t",tbuffer);
		   	     	    gzprintf(tmp,"%5.0f\t",logptr[logidx].reduce);
		   	     	    gzprintf(tmp,"%0.3f\t",logptr[logidx].yield);
		   	     	    gzprintf(tmp,"%5.3f\t",logptr[logidx].power_DC);
		   	     	    gzprintf(tmp,"%5.3f\t",logptr[logidx].power_AC);
		   	     	    gzprintf(tmp,"%5.2f\t",logptr[logidx].voltage_DC);
		   	     	    gzprintf(tmp,"%5.2f\t",logptr[logidx].current_DC);
		   	     	    gzprintf(tmp,"%5.2f\t",logptr[logidx].voltage_AC/sqr2);
		   	     	    gzprintf(tmp,"%5.2f\t",logptr[logidx].current_AC);
		   	     	    gzprintf(tmp,"%5.2f\t",logptr[logidx].temperature);
		   	     	    gzprintf(tmp,"%5.3f\t",logptr[logidx].value); 
		   	     	    gzprintf(tmp,"%08X\t",logptr[logidx].opcode); 
		   	     	    gzprintf(tmp,"%08X\t",logptr[logidx].discode); 
		   	     	    gzprintf(tmp,"%08X\t",logptr[logidx].errcode);  	 
		  	   	     	gzprintf(tmp,"\r\n");  	  
  	   	       }
  	   	       free(loglist[loglistindex].aptr);
  	   	       loglist[loglistindex].aptr = NULL;
  	   }
      }
	close_gz(tmp);
	submit_file (fd, gzfname,2,fname);
	unlink(gzfname);
}


//************************************************

void resetLogs (void)
{
	int idx;
  
	struct tm * timeinfo;
	char tbuffer[20];
	char fname[100];
	long ts = time(NULL);
  
  timeinfo = localtime(&ts);
	strftime (daylogname,20,"%d.%m.%Y.ael",timeinfo);

	
	for (idx=0; idx < AENumDevices; idx++) {
				snprintf(fname,99,"%s%02d_%s",tmp_path,idx,daylogname);
				AELiveLogPos[idx]=0;
				AELiveLogCounter[idx]=0;
		   	AEDayLogPos[idx]=0;
				AEDayLogCounter[idx]=0;
				AEDayLogSize[idx] = 1000;
			  free(AEDayLog[idx]);
			  AEDayLog[idx]= (struct sAEDayLog *)calloc(sizeof(struct sAEDayLog),AEDayLogSize[idx]);
			  AEData[idx].AE_today_yield = 0;
			  saveDayLog(idx,1);
	  }
}

//************************************************

void initLiveLogs (void)
{
	int a;
		
	AELiveLogPos = calloc(sizeof(long),AENumDevices);
	AELiveLogSize = calloc(sizeof(long),AENumDevices);
	AELiveLogCounter = calloc(sizeof(int),AENumDevices);

	AELiveLog = calloc(sizeof(struct sAELiveLog *),AENumDevices);
	
	for (a=0; a<AENumDevices; a++) {
	 AELiveLogSize[a] = 50;
	 AELiveLogPos[a]=0;	
	 AELiveLog[a]= (struct sAELiveLog *)calloc(sizeof(struct sAELiveLog),AELiveLogSize[a]);
	 AELiveLogCounter[a] = 0;
	}
}

//************************************************

void initDayLogs (void)
{
	int a;
	
	AEDayLogPos = calloc(sizeof(long),AENumDevices);
	AEDayLogSize = calloc(sizeof(long),AENumDevices);
	AEDayLogCounter = calloc(sizeof(int),AENumDevices);

	AEDayLog = calloc(sizeof(struct sAEDayLog *),AENumDevices);
	
	for (a=0; a<AENumDevices; a++) {
	 
	 AEDayLogSize[a] = 1000;
	 AEDayLogPos[a]=0;	
	 AEDayLog[a]= (struct sAEDayLog *)calloc(sizeof(struct sAEDayLog),AEDayLogSize[a]);
	 AEDayLogCounter[a] = 0;
	 }
}

//************************************************

void saveLiveLog (int idx, int tmp)
{
	if (logit & 1) printf("saveLiveLog"); fflush(stdout);   
	
	char fname[100];
  
	snprintf(fname,99,"%s%02d_AECLIVELOG",tmp ? tmp_path : log_path,idx);
	FILE * fp = fopen (fname,"wb");
	if (fp) {
		 fwrite(&AEData[idx].AE_Online_ts,sizeof(long),1,fp);
		 fwrite(&(AELiveLogSize[idx]),sizeof(long),1,fp);
		 fwrite(&(AELiveLogPos[idx]),sizeof(long),1,fp);
		 fwrite(AELiveLog[idx],sizeof(struct sAELiveLog),AELiveLogSize[idx],fp);
		 fclose(fp);
	  }
	else 
		printf("cannot open LOG-File %s\r\n",fname);
	if (logit & 1) printf("...DONE\r\n"); fflush(stdout);   
}

//************************************************

void deleteDayLogTmp (int idx)
{
 	char fname[100];
	snprintf(fname,99,"%s%02d_%s",tmp_path,idx,daylogname);
	unlink (fname);
}	

//************************************************

void cleanTmp (void)
 {
	 char * pos;
	 DIR *pdir = NULL;
	 char fname[100];
	 pdir = opendir (tmp_path);
	 if (pdir) { 
		 struct dirent *pent = NULL;
		 while (pent = readdir (pdir)) {
		 	 if (strstr(pent->d_name,"aes.")) {
		 	 	  snprintf(fname,99,"%s%s",tmp_path,pent->d_name);
		 	 	  unlink(fname);
		 	 }
     } 
	 }
  closedir(pdir);  
}

//************************************************

void saveDayLog (int idx, int tmp)
{
	if (logit & 1) printf("saveDayLog"); fflush(stdout);   

	struct tm * timeinfo;
	char fname[100];
	char magic[]="AECLOG";
	char version = 0x02;
  float yield;
	long ts = time(NULL);	

	snprintf(fname,99,"%s%02d_%s",tmp ? tmp_path : log_path,idx,daylogname);
	FILE * fp = fopen (fname,"wb");
	if (fp) {
     fwrite(magic,strlen(magic),1,fp); 
     fwrite(&version,1,1,fp);
     yield = (float) AEData[idx].AE_today_yield;
     fwrite(&yield,sizeof(float),1,fp);
		 fwrite(&AEData[idx].AE_Online_ts,sizeof(long),1,fp);
		 fwrite(&(AEDayLogSize[idx]),sizeof(long),1,fp);
		 fwrite(&(AEDayLogPos[idx]),sizeof(long),1,fp);
		 fwrite(AEDayLog[idx],sizeof(struct sAEDayLog),AEDayLogSize[idx],fp);
		 fclose(fp);
	  }
	else 
		printf("cannot open LOG-File %s\r\n",fname);
	if (logit & 1) printf("...DONE\r\n"); fflush(stdout);   
}


//************************************************

void loadLiveLog (int idx, int tmp)
{
	char fname[100];
	int r;
  time_t ts;
	snprintf(fname,99,"%s%02d_AECLIVELOG",tmp ? tmp_path : log_path,idx);
	FILE * fp = fopen (fname,"rb");
	if (fp) {
		 fread(&ts,sizeof(long),1,fp);
		 fread(&(AELiveLogSize[idx]),sizeof(long),1,fp);
		 fread(&(AELiveLogPos[idx]),sizeof(long),1,fp);
		 fread(AELiveLog[idx],sizeof(struct sAELiveLog),AELiveLogSize[idx],fp);
		 fclose(fp);
		 printf("LiveLogEntries %s -> %ld/%ld\r\n",fname,  AELiveLogPos[idx],AELiveLogSize[idx]);
	  }
	else {
		printf("LOG-File %s nicht gefunden\r\n",fname);
	  
	  }
	
}

//************************************************

void loadDayLog (int idx, int tmp)
{
	char fname[100];
	char magic[10];
	char version;
	float yield;
	time_t ts;

	snprintf(fname,99,"%s%02d_%s",tmp ? tmp_path : log_path,idx,daylogname);
	FILE * fp = fopen (fname,"rb");
	if (fp) {
		 fread(magic,6,1,fp); 
     fread(&version,1,1,fp);
     fread(&yield,sizeof(float),1,fp);
     AEData[idx].AE_today_yield = (double) yield;
		 fread(&ts,sizeof(long),1,fp);
		 fread(&(AEDayLogSize[idx]),sizeof(long),1,fp);
		 fread(&(AEDayLogPos[idx]),sizeof(long),1,fp);
		
		 AEDayLog[idx] = realloc(AEDayLog[idx],(size_t)AEDayLogSize[idx]*sizeof(struct sAEDayLog));
     
     if (version == 1) {
     	int a; 
     	for (a=0; a<AEDayLogSize[idx];a++) 
     	  fread(AEDayLog[idx]+a,sizeof(struct sAEDayLog)-3*sizeof(int),1,fp);
      }
     else 
		   fread(AEDayLog[idx],sizeof(struct sAEDayLog),AEDayLogSize[idx],fp);
		 fclose(fp);
		 printf("DayLogEntries %s -> %ld/%ld\r\n",fname,  AEDayLogPos[idx],AEDayLogSize[idx]);
     getHourYield(idx);
	  }
	else {
		printf("LOG-File %s nicht gefunden\r\n",fname);
	  
	  }
	
}

//************************************************

void pushLiveLog (int idx)
{
	int a; 
	
			if (AELiveLogPos[idx] == AELiveLogSize[idx]-1) { 
				memmove (AELiveLog[idx], &(AELiveLog[idx][a+1]), (AELiveLogSize[idx]-1)*sizeof(struct sAELiveLog)) ;
//				for (a=0; a<AELiveLogSize[idx]-1; a++)
//				 AELiveLog[idx][a] =  AELiveLog[idx][a+1];
			  }
			
			AELiveLog[idx][AELiveLogPos[idx]].ts = AEData[idx].AE_TS_Current;
			AELiveLog[idx][AELiveLogPos[idx]].reduce = (float) AEData[idx].AE_Reduzierung;
			AELiveLog[idx][AELiveLogPos[idx]].power = (float) AEData[idx].AE_current_power;
			AELiveLog[idx][AELiveLogPos[idx]].value = (float) AEData[idx].AE_PV_DC_Spannung;
		
		  if (AEData[AEDeviceIndex].AE_Offline) {
		  	 AELiveLog[idx][AELiveLogPos[idx]].reduce=
		  	   AELiveLog[idx][AELiveLogPos[idx]].power=
		  	     AELiveLog[idx][AELiveLogPos[idx]].value=0;
		  	   }
			 
		  if (AELiveLogPos[idx] < AELiveLogSize[idx]-1) AELiveLogPos[idx]++;
		 
		  AELiveLogCounter[idx]++; 	
		  if (AELiveLogCounter[idx] >= 10) {
		  	saveLiveLog(idx,1);
		  	AELiveLogCounter[idx]=0;
		  }
}

//************************************************

void pushDayLog (void)
{
	int idx;
	
  for (idx=0; idx<AENumDevices; idx++) {
			if (AEDayLogPos[idx]==AEDayLogSize[idx]-1) {  //if necessary realloc logstorage
			   AEDayLogSize[idx] += 1000;
		   	 AEDayLog[idx] = realloc(AEDayLog[idx], sizeof (struct sAEDayLog)*AEDayLogSize[idx]);
			 }
						
			AEDayLog[idx][AEDayLogPos[idx]].ts = logtime;
			AEDayLog[idx][AEDayLogPos[idx]].reduce = AEData[idx].AE_Reduzierung;
			AEDayLog[idx][AEDayLogPos[idx]].yield = AEData[idx].AE_today_yield;
			AEDayLog[idx][AEDayLogPos[idx]].power_DC = AEData[idx].AE_PV_DC_Leistung;
			AEDayLog[idx][AEDayLogPos[idx]].power_AC = AEData[idx].AE_PV_AC_Leistung;
			AEDayLog[idx][AEDayLogPos[idx]].voltage_DC = AEData[idx].AE_PV_DC_Spannung;
			AEDayLog[idx][AEDayLogPos[idx]].current_DC = AEData[idx].AE_PV_DC_Strom;
			AEDayLog[idx][AEDayLogPos[idx]].voltage_AC = AEData[idx].AE_Ampl_AC_Spannung;
			AEDayLog[idx][AEDayLogPos[idx]].current_AC = AEData[idx].AE_Ampl_AC_Strom;
			AEDayLog[idx][AEDayLogPos[idx]].temperature = AEData[idx].AE_Temperatur;
			AEDayLog[idx][AEDayLogPos[idx]].offline = AEData[idx].AE_Offline;
			AEDayLog[idx][AEDayLogPos[idx]].value =  AEData[idx].AE_external_power;
		  AEDayLog[idx][AEDayLogPos[idx]].opcode =  AEData[idx].AE_Status_Betrieb;
		  AEDayLog[idx][AEDayLogPos[idx]].discode =  AEData[idx].AE_Status_Stoerung;
		  AEDayLog[idx][AEDayLogPos[idx]].errcode =  AEData[idx].AE_Status_Fehler;
		  AEDayLogPos[idx]++;
		  AEDayLogCounter[idx]++; 	
		 
		  if (AEDayLogCounter[idx] >= 10) {
		  	saveDayLog(idx,1);
		  	AEDayLogCounter[idx]=0;
		  }
  }
}


//************************************************

void changeLogDay(void) 
{
	
	long ts = time(NULL);	
	char tbuffer[20];
	struct tm * timeinfo;
  timeinfo = localtime(&ts);
	strftime (tbuffer,20,"%d.%m.%Y.ael",timeinfo);

  if (strcmp(tbuffer,daylogname) != 0) {
  	  int idx;
  	  for (idx=0; idx <AENumDevices; idx++) {
        saveDayLog(idx,0);
        deleteDayLogTmp(idx);
       }
      cleanTmp();	  
      resetLogs();  
    }
}

//*****************************************

void write_st (int fd, char *msg, ...)
{
 int r = 0;
 int z;
 unsigned long m = millis();
 if (fd <= 0) return; 
 va_list ap;
 char * buf;
 va_start(ap, msg);
 int l = vsnprintf(NULL,0, msg, ap);
 va_end(ap); 
 buf = malloc(l+1);
 va_start(ap, msg);
 vsprintf(buf, msg, ap);
 va_end(ap); 
 do {
    z = write(fd, buf+r, strlen(buf)-r );
    if (z>0) r+=z;
  } while (r < strlen(buf) || millis()- m > 3500); 
 free(buf);  
}
	
//************************************************

void send_http_header (int fd, int code)
{
 write_st(fd,"HTTP/1.1 ");
 switch (code) {
  case 200:  write_st(fd,"200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Cache-Control: no-cache\r\n");
      
             break; 
  case 304:  write_st(fd,"304 Not Modified\r\n");
             break;              
  case 404:  write_st(fd,"404 Not Found\r\n");
             break; 
  case 401:  write_st(fd,"401 Unauthorized\r\nServer: AECLogger\r\nWWW-Authenticate: Basic realm=\"AECLogger\"\r\n"
                           "Accept-Ranges: bytes");
             break; 
 }
 write_st(fd,"Connection: Close\r\n\r\n");
}  

//************************************************



/* submit an image file from disk (logo ...
--------------------------------------------*/
int submit_file (int fd, char * file, int where, char* uploadfilename) //1->programpath, 2->tmp_path
{
  char  *buf = malloc(1025);
  
  int cnt,f;
  FILE * sf;
  struct  stat fstatus; 
  if (where == 1)
     sprintf(buf,"%s%s",program_path,file);
  if (where == 2)
     sprintf(buf,"%s%s",tmp_path,file);
  
  
  if (stat(buf, &fstatus) == 0) 
    if (fstatus.st_size > 0) { 
      f = open (buf,O_RDONLY);  
      
      if (f)  {
          write_st(fd,"HTTP/1.1 200 OK\r\n");
           write_st(fd,"Server: AECLogger %s \r\n",Version);
            write_st(fd,"Connection: close\r\n"); 

           if (strstr(file,".js	")!=NULL) {
           	   write_st(fd,"Content-Type: text/javascript\r\n");    
               }

           	   
           if (strstr(file,".png")!=NULL || strstr(file,".PNG")!=NULL)
              write_st(fd,"Content-Type: image/png\r\n");  
            if (strstr(file,".ico")!=NULL || strstr(file,".ICO")!=NULL)
              write_st(fd,"Content-Type: image/x-icon\r\n");     
           if (strstr(file,".log")!=NULL || strstr(file,".LOG")!=NULL ) {
              write_st(fd,"Content-Type: application/octet-stream\r\n"); 
              
              
              }
           if (uploadfilename != NULL) write_st(fd,"Content-Disposition: attachment; filename=\"%s\"\r\n",uploadfilename); 
              
            if (strstr(file,".gz")!=NULL){
            	  write_st(fd,"Cache-Control: no-cache\r\n");  
            	  //if (logit & 4) printf("GZS(%ld)",fstatus.st_size);fflush(stdout);
            	  write_st(fd,"Content-Encoding: gzip\r\n");
               }

            if (strstr(file,".b64")!=NULL || strstr(file,".LOG")!=NULL){
              write_st(fd,"Content-Transfer-Encoding: base64\r\n");    
              write_st(fd,"Content-Type: text/plain\r\n");    
              }

           write_st(fd,"Access-Control-Allow-Origin: *\r\n");
           
           write_st(fd,"Content-Length: %ld\r\n\r\n",fstatus.st_size);
         
          int snd;
          int done;
          while((cnt=read(f,buf,1024)) > 0) {
             snd = 0;
             do {            
             done = send(fd,buf+snd,cnt-snd,0);
             if (done > 0) snd+=done;
             }while (snd < cnt); 
           
            }
          close(f); 
          
         }
      else printf("FILE NOT FOUND %s\r\n",buf);   
     }
 free(buf);
 return 1;
}


/******************************************************************************\
  Build the index for html.inc 
\******************************************************************************/

void buildTextIndex (void)
{
 if (tindex != NULL) free (tindex);	
	
 tindex = malloc(sizeof(s_tidx)*300);
 maxtindex = 299; 
 
 int a; 

 char *fname = malloc (strlen(program_path)+20); 
 sprintf(fname,"%shtml.inc",program_path);
 struct stat fattr;
 stat (fname,&fattr);
 dstate = fattr.st_mtime; 
 FILE * f = fopen(fname,"rb");
 free (fname);
 char * buf = malloc(500);
 int ctr=0;
 int aidx=-1;
 if (f != NULL) { 
   while (1) {
       if (fgets (buf,249,f)) {
         if (buf[0] == '#' && buf[1] != '#') { //Ende
            if (aidx >= 0) {
               tindex[aidx].len = ctr;
               ctr = 0;
               
               }   
            break;
           } 
         if (strlen(buf) > 2)
            if (buf[0] == '#' && buf[1] == '#'){
                if (aidx >= 0) {
                   tindex[aidx].len = ctr;
                   ctr = 0;
                   
                  }                 
                aidx = atoi(buf+2);
                if (aidx >= 0 && aidx < 500) {
                  if (aidx > maxtindex) {
                    tindex = realloc (tindex, sizeof(s_tidx)*(aidx+1));
                    maxtindex = aidx;
                    }
                  }
                else aidx = -1;
                if (aidx >=0) {
                  tindex[aidx].pos = ftell(f);
                  }
                continue;  
               }
            ctr+=strlen(buf);
         }
       else break;  
   }
   fclose(f);     
  }
  else 
  	printf("Error reading html-file\r\n");
  free(buf);
 
}   

/******************************************************************************\
  Read text-fragments from html.inc 
  ini-part ##1  html/javascript-parts > ##1 end with #
\******************************************************************************/

char * getText (int idx)
{
 //if (logit & 4) printf("G(%d)",idx),fflush(stdout);
 FILE * f;
 char * buf = NULL; 
 int ctr;
 int a;
 int spos=0;
 char space =0;
 char * line = malloc(500);
 char *fname = malloc (strlen(program_path)+20); 
 sprintf(fname,"%shtml.inc",program_path);
 
 struct stat fattr;
 stat (fname,&fattr);
 if (dstate != fattr.st_mtime) {
		dstate = fattr.st_mtime; 
		buildTextIndex ();
	}
 
 while ((f = fopen(fname,"rb")) == NULL);
 free (fname);
 if (f != NULL) {
    fseek(f,tindex[idx].pos,SEEK_SET); 
    ctr = tindex[idx].len;
    buf = malloc(ctr+1);
    while (ctr > 0) {
      fgets(line,499,f);
      int linelen = strlen(line);
      ctr -= linelen;
      if (line[0] == '|' || (line[0]=='/' && line[1]=='/') || (line[0]=='\r' && line[1]=='\n')) continue;	
      
      int empty = 1;
      for (a=0; a < linelen; a++) {
      	 if (idx > 1) {
	      	 if  (line[a] == ' ' && empty) continue; 
	         if  (line[a] == '\t') continue;
	         
	         if  (line[a] == ' ' && space) continue; 
	         if  (line[a] == ' ') space = 1;
	         else space = 0;  
	        }
         buf[spos++]=line[a];
         empty = 0;
                     
        }
      }
      buf[spos]=0;
 } 

 fclose(f);
 free (line);
 //if (logit & 4) printf("D(%d)",idx);fflush(stdout);
 return buf; 
}  

 
//************************************************
 
void getValue (char * ptr, char * target, char tr, int max) 
{
 char * qpos;
    
  	qpos = strchr(ptr,tr);
  	if (qpos == NULL) { 
  		strncpy (target,ptr,max-1);
  	  }  
  	else {
  		int len = qpos-ptr < max ? qpos-ptr : max-1;
  		memcpy (target,ptr,len);
  		target[len] = 0;
  	  }	
}	   

/******************************************************\
          open the http-listen socket
\******************************************************/

int initEventListenSocket (void) 
{
  eventfd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(eventfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
  struct timeval tv;
  tv.tv_sec = 0;  
  tv.tv_usec = 100000; 
  setsockopt(eventfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
  setsockopt(eventfd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

  if (eventfd < 0)  error("ERRsock");
  bzero((char *) &eventserv_addr, sizeof(eventserv_addr));
  eventserv_addr.sin_family = AF_INET;
  eventserv_addr.sin_addr.s_addr = INADDR_ANY;
  eventserv_addr.sin_port = htons(listenport);
  
  int ctr = 5;  
  int res;
  do {
     res = bind(eventfd, (struct sockaddr *) &eventserv_addr, sizeof(eventserv_addr));
     ctr--;    
    } while (ctr && res <0);
   
  if (res < 0) {  
     printf("ERRBIND"); fflush(stdout);;
     exit(0);
    }
  listen(eventfd,20);

  eventclilen = sizeof(eventcli_addr);
  FD_SET (eventfd, &active_fd_set);
}

/******************************************************\
        open and init the serial port RS-485
\******************************************************/

void initSerialPort ()
{
	struct termios options;
  
  if (serialfd != 0) close(serialfd);
  printf("open serial %s\r\n",serialport);
	serialfd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
 
	if (serialfd == -1) {
		printf("COM-Port error\r\n");
	  exit(0);
	  }
 
	tcgetattr(serialfd, &options);
 
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);
 
	options.c_cflag |= CS8;
	options.c_iflag |= IGNBRK;
	options.c_iflag &= ~( BRKINT | ICRNL | IMAXBEL | IXON);
	options.c_oflag &= ~( OPOST | ONLCR );
	options.c_lflag &= ~( ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
	
	options.c_iflag = 0;
	options.c_lflag |= NOFLSH;
	options.c_cflag &= ~CRTSCTS;
	
	options.c_cc[VTIME]=0;
  options.c_cc[VMIN]=0;

	tcsetattr(serialfd, TCSANOW, &options);
	ReadSerial(); // fflush input buffer + wait 
  return;
}

/******************************************************\
     read programs initial settings (html.inc #1)
\******************************************************/

void getParameters(void)
{
	char * params;
	char pname[250];
	snprintf(pname,249,"%saeclogger.ini",param_path);
	
	
	struct  stat fstatus; 
  
  if (stat(pname, &fstatus) == 0) {
    params=malloc(fstatus.st_size+1) ;
    }
  else {
  	printf("Datei %s nicht gefunden !\r\n",pname);
  	exit(0);
    }  
	
	
  int fctr = 0;
  char * line;  
	FILE * fp = fopen(pname,"rb");
	if (fp) {
	      while (!feof(fp)) {
	      	line = fgets(params+fctr,300,fp);
	      	printf("%s\r\n",line);
	      	if (line)
	      	 	if (line[0]!='|')
	      		fctr+=strlen(line);
	      	
	      }
	      
	      
				//fread (params,fstatus.st_size,1,fp);
				params[fstatus.st_size] = 0;
				char * start, * end;
			 	
				if (start = strstr(params,"http_port="))
					listenport = atoi (start+10);
					
				if (start = strstr(params,"log_level="))
					logit = atoi (start+10);		
					
					
				if (start = strstr(params,"detail_livecolor="))
					detailLiveColor=strtol(start+17,NULL,16);	

				if (start = strstr(params,"detail_hourcolor="))
					detailHourColor=strtol(start+17,NULL,16);						
					
				if (start = strstr(params,"log_interval="))
					log_interval = atoi (start+13);	
				if (start = strstr(params,"req_interval="))
					req_interval = atoi (start+13);	
				if (start = strstr(params,"pow_interval="))
					pow_interval = atoi (start+13);	
				
				
				end = NULL;	
				if (start = strstr(params,"serial_port=")) {
					start+=12;
				  if (! (end = strstr(start,";"))) 
				  	if ( !(end = strstr(start,"\r\n")))
				  		if ( !(end = strstr(start,"\n")))
				  			end = strstr(start,"\r");
					if (end) {
						serialport=calloc(end-start+1,1);
						memcpy(serialport, start,end-start);
				    printf("%s\r\n",serialport);
						}
				}	
				end = NULL;	
				if (start = strstr(params,"serial_identifier=")) {
					start+=18;
				  if (! (end = strstr(start,";"))) 
				  	if ( !(end = strstr(start,"\r\n")))
				  		if ( !(end = strstr(start,"\n")))
				  			end = strstr(start,"\r");
					if (end) {
						serialidentifier=calloc(end-start+1,1);
						memcpy(serialidentifier, start,end-start);
				    printf("%s\r\n",serialidentifier);
						}
				}		
				end = NULL;	
				if (start = strstr(params,"tmp_path=")) {
					start+=9;
				 if (! (end = strstr(start,";"))) 
				  	if ( !(end = strstr(start,"\r\n")))
				  		if ( !(end = strstr(start,"\n")))
				  			end = strstr(start,"\r");
					if (end) {
						tmp_path=calloc(end-start+1,1);
						memcpy(tmp_path, start,end-start);
						}
				}	
				
				end = NULL;	
				if (start = strstr(params,"log_path=")) {
					start+=9;
				  if (! (end = strstr(start,";"))) 
				  	if ( !(end = strstr(start,"\r\n")))
				  		if ( !(end = strstr(start,"\n")))
				  			end = strstr(start,"\r");
					if (end) {
						log_path=calloc(end-start+1,1);
						memcpy(log_path, start,end-start);
						}
				}	
				int a;
				int ct = 0;
				char key[30];
				start=params;
				while (start = strstr(start,"inverterid_")) {
				   ct++;
				   start++;
				 }
			
				AEDevices =calloc(ct,sizeof(int));
				SchuecoID =calloc(ct,sizeof(int));
				AEDevicesComment =calloc(ct,sizeof(char *));
				AENumDevices=ct;
				AEData = calloc(AENumDevices * sizeof(struct sAEData),1);
			
				for (a=0; a<AENumDevices; a++) 
					  AEData[a].AE_Offline = 1;
				
				for (a=0; a<ct; a++){
					snprintf(key,29,"inverterid_%d=",a);
					if (start=strstr(params,key)) 
						AEDevices[a]=atoi(start+strlen(key));
				}
				
				for (a=0; a<ct; a++){
					snprintf(key,29,"inverterreduce_%d=",a);
					if (start=strstr(params,key)) {
						AEData[a].AE_Reduzierung_set=(double)atoi(start+strlen(key));
					}
				}
	
				for (a=0; a<ct; a++){
					snprintf(key,29,"invertercolor_%d=",a);
					if (start=strstr(params,key)) 
						AEData[a].AE_ChartColor=strtol(start+strlen(key),NULL,16);
				}	
				
					for (a=0; a<ct; a++){
					snprintf(key,29,"schuecoid_%d=",a);
					if (start=strstr(params,key)) {
						SchuecoID[a]=atoi(start+strlen(key));
					}
				}
			
				for (a=0; a<ct; a++){
					snprintf(key,29,"invertercomment_%d=",a);
					if (start=strstr(params,key)) {
						if (! (end = strstr(start,";"))) 
				  	if ( !(end = strstr(start,"\r\n")))
				  		if ( !(end = strstr(start,"\n")))
				  			end = strstr(start,"\r");
					  if (end) {
						AEDevicesComment[a]=calloc(end-start+1,1);
						memcpy(AEDevicesComment[a], start+strlen(key),end-start-strlen(key));
					 } 
					}
				}
				free (params);
			  }
   else {
  	 printf("Kann aeclogger.ini nicht lesen !\r\n");
  	 exit(0);
     }  			  
}
//**********************************************************


void init()
{
 
 struct tm * timeinfo;
 long ts = time(NULL);	
 timeinfo = localtime(&ts);
 strftime (daylogname,20,"%d.%m.%Y.ael",timeinfo);

 struct  stat fstatus; 
 if (stat("/tmp/relais", &fstatus) == 0) 
    datalog_active = 1;
  

 snprintf(gzfname,15,"aes.%d.gz",getpid());

 sbuf = malloc(100);     

 createQueue(); 
 
 FD_ZERO (&active_fd_set); 
  

 initEventListenSocket();

 
}

//************************************************


void write_errorpage (int fd, char * buffer)
{
	send_http_header (fd,200);
	write_st(fd,"Unbekannter Request:%s<br>",buffer);
}

//************************************************


void write_offlinepage (int fd, int idx)
{
	send_http_header (fd,200);
  char text[20];
  if (idx == -1) 
  	  snprintf(text,19,"Alle");
  else
      snprintf(text,19,"WR-%d",idx);	 
	char * txt = getText(100);
	write_st(fd,txt,text);
	free(txt);
}
//************************************************ 

int get_offlinestate (int idx)
{
	int a;
	if (idx < 0)
	  for (a=0; a<AENumDevices; a++) {
		  if (! AEData[a].AE_Offline) 
			   return 0;
	   }
	if (idx >=0)
		  return  AEData[idx].AE_Offline ; 
	
	return 1;
}
//************************************************ 

void set_wakeup(int device)
{

  float lastyield = 0;
  long logidx;
  for (logidx=0; logidx<AEDayLogPos[device]; logidx++) {
      if(lastyield<1 && AEDayLog[device][logidx].yield>=1) {
      	
      	AEData[device].AE_Wakeup_ts = AEDayLog[device][logidx].ts; 
      	//if (logit &4) printf("setwakeup %f %f %ld\r\n",lastyield,AEDayLog[device][logidx].yield,AEData[device].AE_Wakeup_ts);
      	break;
        }
   	  lastyield = AEDayLog[device][logidx].yield;
   	}
 
}


//************************************************ 

void meanArchive(int idx, float * target)
{
	struct sAEDayLog * logptr = loglist[idx].aptr;
	int a, loglistindex;
	int numblocks = 24*3600/MEANINTERVAL;
	int tmpidx,logidx;
	for (a=0; a< numblocks; a++) 
	  target[a]=0;
	for (logidx=0; logidx<loglist[idx].len; logidx++) {
		if (logptr[logidx].power_AC > 0) {
			tmpidx=(logptr[logidx].ts%86400) / MEANINTERVAL;
			if (target[tmpidx]==0)
				target[tmpidx] = logptr[logidx].power_AC;
			else {
				target[tmpidx] += logptr[logidx].power_AC;
				target[tmpidx] /= 2;
			  }	
		}
	}
}

//************************************************ 

void sumArchive(int idx, float * source, float * target)
{
	int a;
	int numblocks = 24*3600/MEANINTERVAL;
	for (a=0; a<numblocks; a++) 
		 target[a]+=source[a];
}

//************************************************ 

void writeJSONArchiveLogData(int fd, char * devicefield, long ts, char * infofields)
{
 	struct tm * timeinfo;
	char tbuffer[50];
	struct sAEDayLog * logptr;
	int a, b,loglistindex;
  timeinfo=localtime (&ts);
  int m_day,m_month, m_year;
  m_day = timeinfo->tm_mday;
  m_month = timeinfo->tm_mon+1;
  m_year = timeinfo->tm_year+1900;
  
	
  int device;
  long logidx;
  long lts;
  struct gzFile_s * tmp = open_gz();
  int firstentry;
  int firstdevice;
  int step=1;
  
  int single = 0;
  
  for (a=0,b=0; a<strlen(devicefield); a++)
    if (devicefield[a]=='1') b++;
  
  single = b>1 ? 0 : 1;  	
  
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	
	float *mnarchive = calloc(sizeof(float),24*3600/MEANINTERVAL);  //15 second blocks
	float *smarchive = calloc(sizeof(float),24*3600/MEANINTERVAL);  //summe 15 sekunden blöcke
	
	
	 if (!single) { // summenarray bilden
		  for (device=0; device < AENumDevices; device++) {
		  	   if (devicefield[device]=='1') {
		  	   	     loglistindex = -1; 
		  	   	     for (a=0; a<loglistentries; a++) {
		  	   	     	  if (loglist[a].deviceindex==device && loglist[a].logday == m_day && loglist[a].logmonth== m_month && loglist[a].logyear== m_year) {
		                    loglistindex=a;
		                    LoadLogArchive (a,1);
		                    logptr = loglist[a].aptr;
		                    meanArchive(a, mnarchive);
		                    sumArchive(a, mnarchive,smarchive);
		                    break;  	   	     	  	
		  	   	     	     }
		  	   	       }
		  	   	     }
		  	   	 }  
		  	   }
	
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);
  
  if (ts == 0) step=10;
  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
	  	   
   for (device = single ? 0 : -1; device < AENumDevices; device++) {
   	   loglistindex = -1; 
   	   if (device >= 0)
  	     if (devicefield[device]=='1') {  	
  	   	  
  	   	     for (a=0; a<loglistentries; a++) {
  	   	     	  if (loglist[a].deviceindex==device && loglist[a].logday == m_day && loglist[a].logmonth== m_month && loglist[a].logyear== m_year) {
                    loglistindex=a;
                    LoadLogArchive (a,1);
                    logptr = loglist[a].aptr;
                    break;  	   	     	  	
  	   	     	     }
  	   	       }
  	   	     }   	 
           
           if (device >= 0)  	   	           
  	   	     if (loglistindex == -1) continue;  
  	   	       	   	     
  	   	     if (!firstdevice) 
  	   	     	 gzprintf(tmp,"},");
             gzprintf(tmp,"{\"id\": %d,\"data\":[",device);
             firstentry = 1;
             if (device >= 0) {
		  	   	     for (logidx=0; logidx<loglist[loglistindex].len; logidx++) {
		   	     	  	  if (firstentry) 
		   	     	  	     firstentry = 0;
		   	     	      else	  
		   	     	  	    gzprintf(tmp,",");
		   	     	      gzprintf(tmp,"{\"ts\": %ld",logptr[logidx].ts);
		   	     	      if (infofields[0] == '1') gzprintf(tmp,",\"red\": %1.0f",logptr[logidx].reduce);
		   	     	      if (infofields[1] == '1') gzprintf(tmp,",\"yld\": %1.3f",logptr[logidx].yield);
		   	     	      if (infofields[2] == '1') gzprintf(tmp,",\"pdc\": %1.3f",logptr[logidx].power_DC);
		   	     	      if (infofields[3] == '1') gzprintf(tmp,",\"pac\": %1.3f",logptr[logidx].power_AC > 0 ? logptr[logidx].power_AC : 0);
		   	     	      if (infofields[4] == '1') gzprintf(tmp,",\"vdc\": %1.2f",logptr[logidx].voltage_DC);
		   	     	      if (infofields[5] == '1') gzprintf(tmp,",\"cdc\": %1.2f",logptr[logidx].current_DC);
		   	     	      if (infofields[6] == '1') gzprintf(tmp,",\"vac\": %1.2f",logptr[logidx].voltage_AC/sqr2);
		   	     	      if (infofields[7] == '1') gzprintf(tmp,",\"cac\": %1.2f",logptr[logidx].current_AC);
		   	     	      if (infofields[8] == '1') gzprintf(tmp,",\"tem\": %1.2f",logptr[logidx].temperature);
		   	     	      if (infofields[9] == '1') gzprintf(tmp,",\"val\": %1.3f",logptr[logidx].value);  	 
		   	     	   	  gzprintf(tmp,"}");  	     	      	  	   	     	      	
		  	   	     	    
		  	   	       }
		  	   	 	     gzprintf(tmp,"]");
		  	   	       firstdevice = 0;
		  	   	       free(loglist[loglistindex].aptr);
		  	   	       loglist[loglistindex].aptr = NULL;
		  	   	 }
		  	   	 else {
		  	   	 	for (a=0; a<24*3600/MEANINTERVAL; a++) {
 	   	     	    if (smarchive[a] > 0) {
	   	     	  	  if (firstentry) 
	   	     	  	     firstentry = 0;
	   	     	      else	  
	   	     	  	    gzprintf(tmp,",");
	                    
	   	     	      gzprintf(tmp,"{\"ts\": %ld",(a*MEANINTERVAL+(ts/86400)*86400) + 86400);
	   	     	      gzprintf(tmp,",\"pac\": %1.3f",smarchive[a]);
	                gzprintf(tmp,"}");  	     	      	  	   	     	      	
	  	   	     	    
	  	   	       }
  	   	       }
  	   	 	     gzprintf(tmp,"]");
  	   	       firstdevice = 0;
		  	   	 }
   }
      
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
	free(mnarchive);
	free(smarchive);

}

//************************************************ 

void write_json_LogData(int fd, char * devicefield, long ts, char * infofields)
{
  int device;
  long logidx;
  long lts;
  struct gzFile_s * tmp = open_gz();
  int firstentry;
  int firstdevice;
  int step=1;
  
 	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);
  
  if (ts == 0) step=10;
  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
  	   if (devicefield[device]=='1') {
  	   	     if (!firstdevice) 
  	   	     	 gzprintf(tmp,"},");
             gzprintf(tmp,"{\"id\": %d,\"data\":[",device);
             firstentry = 1;
  	   	     for (logidx=0; logidx<AEDayLogPos[device]; logidx+=1) {
  	   	     	  if (AEDayLog[device][logidx].yield < 0.01) continue;	 
  	   	     	  lts = AEDayLog[device][logidx].ts;
  	   	     	  if (lts > ts) {
  	   	     	  	  if (firstentry) 
  	   	     	  	     firstentry = 0;
  	   	     	      else	  
  	   	     	  	    gzprintf(tmp,",");
  	   	     	      gzprintf(tmp,"{\"ts\": %ld",AEDayLog[device][logidx].ts);
  	   	     	      if (infofields[0] == '1') gzprintf(tmp,",\"red\": %1.0f",AEDayLog[device][logidx].reduce);
  	   	     	      if (infofields[1] == '1') gzprintf(tmp,",\"yld\": %1.3f",AEDayLog[device][logidx].yield);
  	   	     	      if (infofields[2] == '1') gzprintf(tmp,",\"pdc\": %1.3f",AEDayLog[device][logidx].power_DC);
  	   	     	      if (infofields[3] == '1') gzprintf(tmp,",\"pac\": %1.3f",AEDayLog[device][logidx].power_AC);
  	   	     	      if (infofields[4] == '1') gzprintf(tmp,",\"vdc\": %1.2f",AEDayLog[device][logidx].voltage_DC);
  	   	     	      if (infofields[5] == '1') gzprintf(tmp,",\"cdc\": %1.2f",AEDayLog[device][logidx].current_DC);
  	   	     	      if (infofields[6] == '1') gzprintf(tmp,",\"vac\": %1.2f",AEDayLog[device][logidx].voltage_AC/sqr2);
  	   	     	      if (infofields[7] == '1') gzprintf(tmp,",\"cac\": %1.2f",AEDayLog[device][logidx].current_AC);
  	   	     	      if (infofields[8] == '1') gzprintf(tmp,",\"tem\": %1.2f",AEDayLog[device][logidx].temperature);
  	   	     	      if (infofields[9] == '1') gzprintf(tmp,",\"val\": %1.3f",AEDayLog[device][logidx].value);  	 
  	   	     	   	  gzprintf(tmp,"}");  	     	      	  	   	     	      	
  	   	     	    }
  	   	       }
  	   	 	     gzprintf(tmp,"]");
  	   	       firstdevice = 0;
  	   }
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}


//************************************************ 

void write_json_LiveData(int fd, char * devicefield, long ts, char * infofields)
{
  int device;
  long logidx;
  long lts;
  struct gzFile_s * tmp = open_gz();
  int firstentry;
  int firstdevice;
  int step=1;
  
 	struct tm * timeinfo;
	char tbuffer[50];
  long tst = time(NULL);	
	timeinfo = localtime(&tst);
	strftime (tbuffer,49,"%d.%B.%Y %H:%M:%S",timeinfo);
  
  if (ts == 0) step=10;
  gzprintf(tmp,"{\"AECLogger\":\"%s\",\"time\":\"%s\",\"devices\":[",Version,tbuffer);
  firstdevice = 1;
  for (device=0; device < AENumDevices; device++) {
  	   if (devicefield[device]=='1') {
  	   	     if (!firstdevice) 
  	   	     	 gzprintf(tmp,"},");
             gzprintf(tmp,"{\"id\": %d,\"data\":[",device);
             firstentry = 1;
  	   	     for (logidx=0; logidx<AELiveLogPos[device]; logidx+=step) {
  	   	     	  	 
  	   	     	  lts = AELiveLog[device][logidx].ts;
  	   	     	  if (lts > ts) {
  	   	     	  	  if (firstentry) 
  	   	     	  	     firstentry = 0;
  	   	     	      else	  
  	   	     	  	    gzprintf(tmp,",");
                    
  	   	     	      gzprintf(tmp,"{\"ts\": %ld",AELiveLog[device][logidx].ts);
  	   	     	      if (infofields[0] == '1') gzprintf(tmp,",\"red\": %3.1f",AELiveLog[device][logidx].reduce);
  	   	     	      if (infofields[1] == '1') gzprintf(tmp,",\"pow\": %3.1f",AELiveLog[device][logidx].power);
  	   	     	      if (infofields[9] == '1') gzprintf(tmp,",\"val\": %3.1f",AELiveLog[device][logidx].value);  	 
  	   	     	   	  gzprintf(tmp,"}");  	     	      	  	   	     	      	
  	   	     	    }
  	   	       }
  	   	 	     gzprintf(tmp,"]");
  	   	       firstdevice = 0;
  	   }
      }
  gzprintf(tmp,"}]}");
	close_gz(tmp);
	send_gz(fd);
}
//************************************************ 

void write_json_DayChart(int fd, int idx, long ts, int vtyp)
{
	
	struct gzFile_s * tmp = open_gz();
	
	int a;
	int r = 0;
  float value=0;
	gzprintf(tmp,"{\"datapoints\":[");
	if (idx != 999) { // WR einzeln 
	for (a=0; a<AEDayLogPos[idx]; a++) {
		 if (AEDayLog[idx][a].ts > ts && AEDayLog[idx][a].power_AC >= 0 && AEDayLog[idx][a].yield >0 && AEDayLog[idx][a].ts > (time(NULL)/86400)*86400) {
		   gzprintf(tmp,"{\"t\":%ld,\"p\":%1.2f,\"r\":%1.2f,\"y\":%1.2f,\"v\":%1.2f}%s\r\n",AEDayLog[idx][a].ts,AEDayLog[idx][a].power_AC,AEDayLog[idx][a].reduce,AEDayLog[idx][a].yield,value,a==AEDayLogPos[idx]-1 ? "" : ",");
		  }
	  }
	}
	gzprintf(tmp,"]}");
	close_gz(tmp);
	send_gz(fd);
}

//************************************************ 

void write_json_LiveChart(int fd, int idx, long ts)
{
	send_http_header (fd,200);
	int a;
	int r = 0;
	write_st(fd,"{\"datapoints\":[");
	for (a=0; a<AELiveLogPos[idx]; a++) {
		 if (AELiveLog[idx][a].ts > ts && AELiveLog[idx][a].power > 0 ) {
		   write_st(fd,"{\"x\":%ld,\"y\":%1.2f,\"v\":%1.3f}%s",AELiveLog[idx][a].ts,AELiveLog[idx][a].power,AELiveLog[idx][a].value,a==AELiveLogPos[idx]-1 ? "" : ",");
		  }
	  }
	write_st(fd,"]}");
}


//************************************************ 

void write_json_Status(int fd, int idx)
{
	send_http_header (fd,200);
	write_st(fd,"{\"data\":[");
  write_st(fd,"{\"offline\":%d,\"betrieb\":%d,\"fehler\":%d,\"stoer\":%d}",AEData[idx].AE_Offline,
                                                                             AEData[idx].AE_Status_Betrieb==0x00000A83?1:0,
																																						 AEData[idx].AE_Status_Fehler==0?1:0,
																																						 AEData[idx].AE_Status_Stoerung==0?1:0                                                                             	
                                                                            );
	write_st(fd,"]}");
}

//************************************************ 

void write_json_Current_Overview(int fd)
{
	int a;
	double sum;
	int ext = 0;
	send_http_header (fd,200);
	if (time(NULL)-ExternLiveValueTs < 60) {
		ext = 1;
		sum += ExternLiveValue;
	}
	for (a=0; a<AENumDevices; a++) {
		if (! AEData[a].AE_Offline) 
			  sum += AEData[a].AE_current_power;
	}
	
	write_st(fd,"{\"data\":[");
	for (a=0; a<AENumDevices; a++) {
     write_st(fd,"{\"ID\":%d,\"current\":%f,\"percent\":%f,\"reduce\":%f},",a,AEData[a].AE_current_power,AEData[a].AE_current_power/sum,AEData[a].AE_Reduzierung);
   }
  write_st(fd,"{\"ID\":1000,\"current\":%f,\"percent\":%f}", ext ? ExternLiveValue : -999, ext ? ExternLiveValue/sum : 0);
 
	write_st(fd,"]}");
}


//************************************************ 

void write_detailpage (int fd, int id)
{
	send_http_header (fd,200);
	char * txt = getText(40);
	write_st(fd,txt,id);
	free(txt);
}


//************************************************ 
void write_Fragment (int fd, int id, int what)
{
  
	char * t_ptr;
	switch (what){
		 case F_BETRIEB:    set_wakeup(id);
		 	                  t_ptr = AE_Print_Betriebsdaten(FMT_HTML, id);
		 	                  break; 
		 case F_BITBETRIEB: t_ptr = AE_Print_Bit_Betrieb(FMT_HTML, id);
		 	                  break;
		 case F_BITSTOERUNG: t_ptr = AE_Print_Bit_Stoerung(FMT_HTML, id);
		 	                  break;
 		 case F_BITFEHLER: t_ptr = AE_Print_Bit_Fehler(FMT_HTML, id);
		 	                  break;		 	                  
		 case F_SCHUECO:   	AE_Request_Schueco(id); 
		                    t_ptr = AE_Print_Schueco(FMT_HTML, id);
		                    break;
		 case F_LAENDER:    AE_Request(id, GET_LAENDER,0);
		 	                  t_ptr = AE_Print_Laender(FMT_HTML, id);
		 	                  break; 
		 case F_PARAM:      t_ptr = AE_Print_Parameter(FMT_HTML, id);
		 	                  break;
		 case F_STATISTIK:  t_ptr = AE_Print_Statistikdaten(FMT_HTML);
		 	                  break;
		 default:           return;
		 	                  break; 
     
	  }
	
	struct gzFile_s * tmp = open_gz();
	gzprintf(tmp,t_ptr);
	free(t_ptr);
	close_gz(tmp);
	send_gz(fd);
	
}


//************************************************ 


void write_currentpage (int fd, int id)
{
	send_http_header(fd,200);
	int r;
	struct tm * timeinfo;
	char tbuffer[50];
	r = id;
			timeinfo = localtime(&AEData[r].AE_Lastrequest_ts);
			strftime (tbuffer,50,"%d.%B.%Y %H:%M:%S",timeinfo);

  write_st(fd,"{\"data\":[{");
  write_st(fd,"\"t\":\"%s\",",tbuffer);
  write_st(fd,"\"p\":%4.3f,",AEData[id].AE_current_power);
  write_st(fd,"\"s\":%4.3f,",AEData[id].AE_today_yield);
  write_st(fd,"\"v\":%4.3f}]}",AEData[id].AE_external_power);
 
}
//************************************************

void write_overviewpage (int fd)
{
	send_http_header(fd,200);
  char * txt = getText(60);
	write_st(fd,txt);
	free(txt);
}


//************************************************


void LoadLogArchive (int idx, int keep)
{
	long ts; 
	struct tm * timeinfo;
	char tbuffer[20];
	char fname[100];
	char magic[10];
	char version = 0x01;
	long dummy;
	
  if (loglist[idx].aptr != NULL) free(loglist[idx].aptr);
  loglist[idx].len = 0; 	
	snprintf(fname,99,"%s%s",loglist[idx].isintmp ? tmp_path : log_path,loglist[idx].fname);

	FILE * fp = fopen (fname,"rb");
	if (fp) {
     fread(magic,6,1,fp); 
     fread(&version,1,1,fp);
     fread(&loglist[idx].yield,sizeof(float),1,fp);
		 fread(&dummy,sizeof(long),1,fp);
		 fread(&loglist[idx].max,sizeof(long),1,fp);
		 fread(&loglist[idx].len,sizeof(long),1,fp);
		 loglist[idx].aptr = malloc((size_t)loglist[idx].len*sizeof(struct sAEDayLog));
		 
		 if (version == 1) {
     	int a; 
     	for (a=0; a<loglist[idx].len;a++) 
     	  fread(loglist[idx].aptr+a,sizeof(struct sAEDayLog)-3*sizeof(int),1,fp);
      }
     else 
		   fread(loglist[idx].aptr,sizeof(struct sAEDayLog),loglist[idx].len,fp);
		 
		 
		 fclose(fp);
		 
		 float max = 0;
		 int a;
		 if (loglist[idx].yield == 0) {
		 	  for (a=0; a<loglist[idx].len; a++) 
		 	  	 if (loglist[idx].aptr[a].yield > max)
		 	  	 	 max = loglist[idx].aptr[a].yield;
		 	  	
		 	  loglist[idx].yield = max;  
		   }
		 
		 
		 if (!keep) {
		 	 free(loglist[idx].aptr);
		 	 loglist[idx].aptr=NULL;
		   }

		 //printf("ArchiveLogEntries %s -> %ld   %f (%d %d %d)\r\n",fname,  loglist[idx].len, loglist[idx].yield,loglist[idx].logday,loglist[idx].logweek,loglist[idx].logyear);

	  }
	else {
		printf("LOG-File %s nicht gefunden\r\n",fname);
	  
	  }
	
}


//************************************************
void set_logentry (char * path, char * fname)
{
 int a;
 int istmp = 0;	
 struct tm *tm;
 time_t inittime;
 long device,day,month,year;
 struct s_logfile * ptr = loglist +loglistentries;	  
 if (strstr(path, tmp_path)) istmp = 1;
 
 if (istmp==0) {
 	 for (a=0; a<loglistentries; a++)
 	   if (strcmp(fname,loglist[a].fname) == 0) return; // eintrag bereits in TMP
   }
 
 ptr->isintmp=istmp;	
 ptr->fname = malloc (strlen(fname)+1);
 strcpy (ptr->fname, fname);
 sscanf(ptr->fname,"%02ld_%02ld.%02ld.%04ld.ael",&device,&day,&month,&year);
 ptr->logday=day;
 ptr->logmonth=month;
 ptr->logyear=year;
 ptr->deviceindex=device;
 
 
 
 LoadLogArchive(loglistentries, 0);
 
 time ( &inittime );
 tm = localtime ( &inittime );
 
 tm->tm_year= year-1900;
 tm->tm_mon= month-1;
 tm->tm_mday=day;
 tm->tm_hour=0;
 tm->tm_min=0;
 tm->tm_sec=0;
 ptr->ts=mktime(tm);  // 00:00:00 in UTC
 char tbuffer[50];
 strftime (tbuffer,49,"%V",tm);
 ptr->logweek=atoi(tbuffer);
 
 loglistentries++;	
}

//************************************************


long getLogFiles (char *path, int countonly) {

   long ctr = 0;
	 char * pos;
	 DIR *pdir = NULL;
	 char buffer[150];
	 pdir = opendir (path);
	 if (pdir) { 
		 struct dirent *pent = NULL;
		 while (pent = readdir (pdir)) {
		 	 if (strstr(pent->d_name,".ael")) {
		 	 	  if (!countonly) {
		 	 	  	 set_logentry (path, pent->d_name);
		 	 	  }
          ctr++;
         }
		   }
		  }
  closedir(pdir);  	
	return ctr; 
}

//************************************************

void buildLogList (void)
{
  int numfiles = 0;
	int a;
	if (loglist != NULL) {
	  for (a=0; a<loglistentries; a++) {
	     if (loglist[a].fname != NULL)
	     	  free(loglist[a].fname);
       if (loglist[a].aptr != NULL)
	     	     free(loglist[a].aptr);
	     }
	  free(loglist);
	  loglist=NULL;
	  }
	numfiles += getLogFiles(tmp_path,1);
	numfiles += getLogFiles(log_path,1);

  if (loglist != NULL) free(loglist);
  	
  loglist = calloc(numfiles, sizeof(struct s_logfile));
  loglistentries=0;
  getLogFiles(tmp_path,0);
	getLogFiles(log_path,0);
  printf("Anzahl files %d\r\n",numfiles);
}

//************************************************

void write_loganalysis (int fd)
{
	send_http_header(fd,200);
  char * txt = getText(80);
  buildLogList();
	write_st(fd,txt);
	free(txt);
}
//************************************************

void AE_Request_Schueco (int gid)
{
	if (SchuecoID[gid] != 0) {
		if (AE_Request (gid,GET_PARAMETER,0)) {
			if (AEData[gid].AE_Schueco != SchuecoID[gid] && SchuecoID[gid] > 0 && SchuecoID[gid] <=32) {
				printf("SET SCHUECO !!!\r\n");fflush(stdout);
				if (AE_Request (gid, SET_SCHUECOID, SchuecoID[gid]))
					  AEData[gid].AE_Schueco = SchuecoID[gid];	
			}
		}
	}
	
	int id = AEData[gid].AE_Schueco;
	
	char req[10];
	snprintf(req,9,"#%02d0\r",id);
	SendSerial(req,strlen(req));	
	int r = ReadSerial();

  int a;
  unsigned char cs = 0; 
  for (a=1; a<57; a++) cs += (unsigned char)sbuf[a];
  if (cs==sbuf[57]) {
   	     
				 char lbuf[20];
				 if (sbuf[0] == 0x0A && sbuf[1]=='*') {
				 	  memcpy(lbuf,sbuf+2,2);
				 	  lbuf[2]=0;
				 	  AEData[gid].KA_Id = atoi(lbuf);
				 	  AEData[gid].KA_Status = atoi(sbuf+5);
				 	  AEData[gid].KA_DC_U = atof(sbuf+9);
				 	  AEData[gid].KA_DC_I =atof(sbuf+15);
				 	  AEData[gid].KA_DC_P =atof(sbuf+21);
				 	  AEData[gid].KA_AC_U =atof(sbuf+27);
				 	  AEData[gid].KA_AC_I =atof(sbuf+33);	
				 	  AEData[gid].KA_AC_P =atof(sbuf+39);
				 	  AEData[gid].KA_Temperatur = atof(sbuf+45); 
				 	  AEData[gid].KA_Ertrag = atof(sbuf+51);
				 	  AEData[gid].KA_Timestamp = time(NULL);
      }				 	  
	 }
}

//************************************************

char * AE_Print_Schueco (int fmt, int index)
{
	
	char * s = calloc(5000,1);
		 
	if (fmt == FMT_RAW) { snprintf(s,100,"not yet implemented");}
				 	
	if (fmt == FMT_HTML) {
		char * txt = getText(42);
		char format[10];
		snprintf(format,9,"%d",AEData[index].KA_Id);	sprintf(s+strlen(s),txt,"ID",format,"");
		snprintf(format,9,"%d",AEData[index].KA_Status);	sprintf(s+strlen(s),txt,"Status",format,"");
		snprintf(format,9,"%3.0f",AEData[index].KA_DC_U);	sprintf(s+strlen(s),txt,"DC-Spannung",format,"V=");
		snprintf(format,9,"%3.0f",AEData[index].KA_DC_I);	sprintf(s+strlen(s),txt,"DC-Strom",format,"A");
		snprintf(format,9,"%3.0f",AEData[index].KA_DC_P);	sprintf(s+strlen(s),txt,"Leistung",format,"W");
		snprintf(format,9,"%3.0f",AEData[index].KA_AC_U);	sprintf(s+strlen(s),txt,"AC-Spannung",format,"V~");
		snprintf(format,9,"%3.0f",AEData[index].KA_AC_I);	sprintf(s+strlen(s),txt,"AC-Strom",format,"A");
		snprintf(format,9,"%3.0f",AEData[index].KA_Temperatur);	sprintf(s+strlen(s),txt,"Temperatur",format,"°C");
		snprintf(format,9,"%3.0f",AEData[index].KA_Ertrag);	sprintf(s+strlen(s),txt,"Tagesleistung",format,"W");
		free(txt);
	}			 	
 return s;				 	
}

//************************************************

void processevent(int fd) 
{
	
  int bsize = 200;
	char * buffer = malloc (bsize);
	int bptr = 0;
	int n;
	int rep = 0;
	char * pos;
	char * url = NULL;
	char _cmd[20];
	char _val[20];
	char _fmt[20];
	char _dev[20];
	char _ts[20];
	char _cur[20];
	char _year[20];


	int done =0;
	_cmd[0]=_val[0]=_fmt[0]=_dev[0]=0;
	
	
   do {   
       n = read(fd,buffer+bptr,99);
       if (n == 0) {rep++; continue;}
       bptr+=n;
       if (bptr > bsize-200) {
       	  bsize += 500;
       	  buffer = realloc (buffer,bsize);
         }
       buffer[bptr]=0;
      } while (n > 0 && rep <5); 

   if ((pos = strstr(buffer,"Host: ")) != NULL) {
   	      pos+=6;
   	      char * end = strstr(pos,"\r\n");
   	      if (host != NULL) free(host);
   	      int len = end-pos+1; 	
   	      host = calloc(len,1);
   	      strncpy(host,pos,len-1);	
   	      //printf("-%s-\r\n",host);
          }

   
   if ((pos = strstr(buffer,"GET")) != NULL) {
   	 url = pos+3;
   	  if ((pos = strstr(buffer,"HTTP/")) != NULL) {
   	  	 pos[0]=0;
   	    } 
   	  else 
   	  	 url = NULL;  
    } 
    
   if (url == NULL) {
      write_errorpage(fd,buffer);
     }
   else {	  
   	     if (pos = strstr(url,"favicon")){
   	     	send_http_header(fd,404);
   	     	free(buffer);
   	     	return;
   	    }
   	     if (pos = strstr(url,"WATCHDOG")){
   	     	send(fd, "AESOLARD",8,MSG_NOSIGNAL);
   	     	free(buffer);
   	     	return;
   	    } 
   	    
   	   if (pos = strstr(url,"aeclogger")){
   	     	free(buffer);
          send_http_header(fd,200);
          char * txt;
          txt = getText(10);
         
          write_st(fd,txt,host,Version,host,host,host);
          free(txt); 
   	     	return;
   	    }  
        if (logit & 4) printf("Request-->%s ",url);			
		    if ((pos = strstr(url,"CMD=")) != NULL) 
  	       getValue(pos+4, _cmd, '&',20);
  	    if ((pos = strstr(url,"VAL=")) != NULL) 
  	       getValue(pos+4, _val, '&',20);
  	    if ((pos = strstr(url,"FMT=")) != NULL) 
  	       getValue(pos+4, _fmt, '&',20);
  	    if ((pos = strstr(url,"DEV=")) != NULL) 
  	       getValue(pos+4, _dev, '&',20);
  	    if ((pos = strstr(url,"TS=")) != NULL) 
  	       getValue(pos+3, _ts, '&',20);
  	    if ((pos = strstr(url,"CUR=")) != NULL) 
  	       getValue(pos+4, _cur, '&',20);
  	    if ((pos = strstr(url,"YEAR=")) != NULL) 
  	       getValue(pos+5, _year, '&',20);  	       
  	        

        if ((pos = strstr(_cmd,"ISALIVE")) != NULL) { 
           send_http_header (fd,200);
	         write_st(fd,"AECLOGGEROK");          
          }

        if ((pos = strstr(_cmd,"QLIST")) != NULL) { 
           printQueue();        
          }
        if ((pos = strstr(_cmd,"QTEST")) != NULL) { 
           testQueue();        
          }
        if ((pos = strstr(_cmd,"GETEXPORTFILE")) != NULL) { 
           writeCSVArchiveLogData(fd, _dev, atol(_ts));
           }

        if ((pos = strstr(_cmd,"OVERTAKE")) != NULL) { 
           send_http_header (fd,200);
	         write_st(fd,"OVERTAKEOK");          
	         ShutDown();
          }

        if ((pos = strstr(_cmd,"PVC")) != NULL) { 
              	 if (strlen(_val) > 0) {
              	  double v = atof(_val);
                  int a;
                  ExternLiveValue = v*1000;
                  ExternLiveValueTs = time(NULL);
                  for (a=0; a<AENumDevices; a++) { 
              	      AEDayLog[a][AEDayLogPos[a]].value = v;	
              	      AELiveLog[a][AELiveLogPos[a]].value = v;
              	      AEData[a].AE_external_power=v;
              	     }
              	}
          }

        if ((pos = strstr(_cmd,"GETLOGDATA")) != NULL) { 
        	 if (strlen(_dev) >= AENumDevices) {
              if (strlen(_ts) > 0) {
              	 if (strlen(_val) >= 10) {
              	    write_json_LogData(fd, _dev, atol(_ts), _val);
              	   }
               }
            }
          }

        if ((pos = strstr(_cmd,"GETARCHIVELOGDATA")) != NULL) { 
        	 if (strlen(_dev) >= AENumDevices) {
              if (strlen(_ts) > 0) {
              	 if (strlen(_val) >= 10) {
              	    writeJSONArchiveLogData(fd, _dev, atol(_ts), _val);
              	   }
               }
            }
          }


        if ((pos = strstr(_cmd,"GETLOGWEEK")) != NULL) { 
        	if (strlen(_dev) > 0 && strlen(_val) > 0 &strlen(_year) > 0)
        	     writeJSONLogstates(fd,atoi(_dev),atoi(_year),atoi(_val),1);
          }

        if ((pos = strstr(_cmd,"GETLOGMONTH")) != NULL) { 
        	    writeJSONLogstates(fd,atoi(_dev),atoi(_year),atoi(_val),2);
          }



        if ((pos = strstr(_cmd,"GETONLINESTATE")) != NULL) { 
        	    writeJSONOnlinestate(fd);
          }

        if ((pos = strstr(_cmd,"GETDEVICEINFO")) != NULL) { 
        	    writeJSONDeviceinfo(fd);
          }

        if ((pos = strstr(_cmd,"GETHOURDATA")) != NULL) { 
        	 if (strlen(_dev) > 0) {
         	    writeJSONHourstate(fd,_dev);
         	   }
          }
        
        if ((pos = strstr(_cmd,"LOGO")) != NULL) { 
           submit_file(fd,"logo.gif",1,NULL);
          }

        if ((pos = strstr(_cmd,"CANVASJS")) != NULL) { 
           submit_file(fd,"canvasjs.min.js.gz",1,NULL);
          }                      

        if ((pos = strstr(_cmd,"FRAG_BETRIEB")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_BETRIEB);
            }
          }
          
        if ((pos = strstr(_cmd,"FRAG_BITBETRIEB")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_BITBETRIEB);
            }
          }  
        if ((pos = strstr(_cmd,"FRAG_BITSTOERUNG")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_BITSTOERUNG);
            }
          }            
        if ((pos = strstr(_cmd,"FRAG_BITFEHLER")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_BITFEHLER);
            }
          }            
        if ((pos = strstr(_cmd,"FRAG_SCHUECO")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev, F_SCHUECO);
            }
          }  
        if ((pos = strstr(_cmd,"FRAG_LAENDER")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_LAENDER);
            }
          }  
          
        if ((pos = strstr(_cmd,"FRAG_PARAM")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_Fragment(fd,dev,F_PARAM);
            }
          }            

        if ((pos = strstr(_cmd,"FRAG_STATISTIK")) != NULL) { 
        	   write_Fragment(fd,-1, F_STATISTIK);
          }            


        if ((pos = strstr(_cmd,"SMALL_STATUS")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              int dev = atoi(_dev);
              write_json_Status(fd,dev);
            }
          }  


        if ((pos = strstr(_cmd,"DATALIVEDATA")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              if (strlen(_ts) > 0) {
              	 write_json_LiveChart(fd,atoi(_dev),atol(_ts));
               }
            }
          }

        if ((pos = strstr(_cmd,"DATADAYLOG")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              if (strlen(_ts) > 0) {
              	 if (strlen(_val) > 0) {
              	    write_json_DayChart(fd,atoi(_dev),atol(_ts),atoi(_val));
              	   }
               }
            }
          }

        if ((pos = strstr(_cmd,"DATALIVELOG")) != NULL) { 
        	 if (strlen(_dev) > 0) {
              if (strlen(_ts) > 0) {
              	 if (strlen(_val) > 0) {
              	    write_json_DayChart(fd,atoi(_dev),atol(_ts),atoi(_val));
              	   }
               }
            }
          }

        if ((pos = strstr(_cmd,"DEVICE_OVERVIEW")) != NULL) {
        	 if (get_offlinestate(-1))
        	 	write_offlinepage(fd,-1);
        	 else	  
           write_overviewpage (fd);
         }

        if ((pos = strstr(_cmd,"LOG_ANALYSIS")) != NULL) {  
           write_loganalysis (fd);
         }
          
        if ((pos = strstr(_cmd,"REDUCE")) != NULL) { 
          if (strlen(_dev) > 0) {
             	 if (strlen(_val) > 0) {
                  write_reduce (fd,atoi(_dev),atoi(_val));
                }
              }
          
        }
         
			 if ((pos = strstr(_cmd,"STATUS_DEVICE")) != NULL && strlen(_dev) > 0 ) {
			 	 if (strlen(_dev) > 0) {
			   	 if (get_offlinestate(atoi(_dev)))
        	   	write_offlinepage(fd,atoi(_dev));
        	 else	  
			    	  write_detailpage (fd, atoi(_dev));
			    	 }   
			    				             
           }			 	
			 if ((pos = strstr(_cmd,"CURRENT")) != NULL && strlen(_dev) > 0 ) {
			 	     if (strstr(_dev,"ALL"))
			 	         write_json_Current_Overview(fd);
			 	     else    
			   	       write_currentpage (fd, atoi(_dev));
           }	
		
      }		
 if (logit & 4) printf("..Done\r\n");			
	 
  free(buffer);
}

//**********************************************************************

int SendSerial (char *cmd, int len)
{
 serialactive = 1;	
 return( write(serialfd,cmd, len));	
}

//**********************************************************************

unsigned long millis (void)
{
	struct timeval tv;
  gettimeofday(&tv,0);    
  return  (unsigned long)(((unsigned long)tv.tv_sec-1380000000)*1000l + (((unsigned long)tv.tv_usec) / 1000l));
}
//**********************************************************************

int ReadSerial (void)
{
 int r = 0;	
 int rc=0;
 int esc = 0;

 unsigned long mstart = millis();
 do { 
 	   if (read (serialfd,sbuf+r,1) == 1) {
 	   	 
 	   	 if (r > 0) {
         if (sbuf[r] == 0x0d && !esc) {  //0x0d unescaped . end of transmission
           r++;
           break;
          }
	   	 	 if (esc) {
           sbuf[r-1] = sbuf[r];
           r--;
           esc = 0; 	   	 	 	
 	   	 	 }
 	   	 	else 
 	         if (sbuf[r] == 0x40 ) esc=1;
 	   	  }
 	   	  r++;
 	   }
 	   else {
 	   	   usleep(50000);
 	   	   }
   } while (millis() < mstart+2500); // timeout 1 sec
    if (logit & 0x02) {
				 int a;
				 if (r>0) {
				 printf("\r\n<- "); 
				 for (a=0; a<r;a++) {
				   printf("%02X ",(unsigned char)sbuf[a]); fflush(stdout);
				 }  
				 printf("\r\n");
				}
			}
 serialactive = 0;
 if (r > 0) tslastserial = time(NULL);
 return r;  
}


//**********************************************************************

double AE_Get_FK16_Scheitel(char * buf)
{
	union clong cv;
  cv.b[0]=buf[3]; 	
	cv.b[1]=buf[2];
	cv.b[2]=buf[1];
	cv.b[3]=buf[0];
	return (((double)  ((long)((((double)(cv.i) / (double) 100000) * sqr2)*100)))/100);
}

//**********************************************************************

double AE_Get_FK16(char * buf)
{
	union clong cv;
  cv.b[0]=buf[3]; 	
	cv.b[1]=buf[2];
	cv.b[2]=buf[1];
	cv.b[3]=buf[0];
	return (((double)  ((long)(((double)(cv.i) / (double) 0xffff)*100))) /100);
}

//**********************************************************************

int AE_Get_INT(char * buf)
{
	union clong cv;
  cv.b[0]=buf[3]; 	
	cv.b[1]=buf[2];
	cv.b[2]=buf[1];
	cv.b[3]=buf[0];
	return cv.i;
}

//**********************************************************************

int AE_Get_SHORT(char * buf)
{
	union cshort cv;
  cv.b[0]=buf[1]; 	
	cv.b[1]=buf[0];
	return cv.s;
}

//**********************************************************************

void AE_Get_Betriebsdaten (char *buffer, int len)
{
		AEData[AEDeviceIndex].AE_Ampl_AC_Spannung = AE_Get_FK16 ((char *)buffer+3+(0*4));
		AEData[AEDeviceIndex].AE_Ampl_AC_Strom = AE_Get_FK16 ((char *)buffer+3+(1*4));
    AEData[AEDeviceIndex].AE_PV_DC_Strom = AE_Get_FK16 ((char *)buffer+3+(2*4));
    AEData[AEDeviceIndex].AE_PV_DC_Spannung = AE_Get_FK16 ((char *)buffer+3+(3*4));
    AEData[AEDeviceIndex].AE_PV_AC_Leistung = AE_Get_FK16 ((char *)buffer+3+(4*4));
    AEData[AEDeviceIndex].AE_PV_DC_Leistung = AE_Get_FK16 ((char *)buffer+3+(5*4));
    AEData[AEDeviceIndex].AE_Temperatur = AE_Get_FK16 ((char *)buffer+3+(6*4));
    AEData[AEDeviceIndex].AE_TS_Operations = time(NULL);
}

//************************************************

char * AE_Print_Betriebsdaten (int fmt, int index)
{
	char * s = calloc(5000,1);
	 
	if (fmt == FMT_RAW) { snprintf(s,100,"not yet implemented");}
				 	
	if (fmt == FMT_HTML) {
      char * txt = getText(43);
			sprintf(s+strlen(s),txt,"AC-Spannung (eff.)" ,AEData[index].AE_Ampl_AC_Spannung/sqr2,"V~");
			sprintf(s+strlen(s),txt,"AC-Strom  (eff.)",AEData[index].AE_Ampl_AC_Strom/sqr2,"A");
			sprintf(s+strlen(s),txt,"DC Strom",AEData[index].AE_PV_DC_Strom,"A");
			sprintf(s+strlen(s),txt,"DC Spannung",AEData[index].AE_PV_DC_Spannung,"V=");
			sprintf(s+strlen(s),txt,"AC Leistung",AEData[index].AE_PV_AC_Leistung,"W");
			sprintf(s+strlen(s),txt,"DC Leistung",AEData[index].AE_PV_DC_Leistung,"W");
			sprintf(s+strlen(s),txt,"Temperatur",AEData[index].AE_Temperatur,"°C");
			sprintf(s+strlen(s),txt,"Wirkungsgrad",AEData[index].AE_PV_AC_Leistung/AEData[index].AE_PV_DC_Leistung*100,"%");
      sprintf(s+strlen(s),txt,"Durchschnitt",AEData[index].AE_today_yield /((float)(time(NULL)-AEData[index].AE_Wakeup_ts)/3600.0),"W/h");
		  free(txt);

      txt = getText(44);
			sprintf(s+strlen(s),txt,"Reduziert auf",AEData[index].AE_Reduzierung,"W");
		  free(txt);
	}			 	
 return s;				 	
}

//************************************************

char * AE_Print_Statistikdaten (int fmt)
{
	
	char * s = calloc(5000,1);
	
	int index = 0;
	float syield = 0;
	float sawake=0;	
	
	 
	if (fmt == FMT_RAW) {
				    snprintf(s,100,"not yet implemented");
				 	}
				 	
				 	
	if (fmt == FMT_HTML) {
		
      int a;
      int ok,err,off;
      ok=err=off=0;
      char text[500];
      for (a=0; a<AENumDevices; a++){
      	// status
      	if (AEData[a].AE_Offline) off++;
        if (AEData[a].AE_Status_Betrieb!=0x00000A83 || AEData[a].AE_Status_Fehler!=0 || AEData[a].AE_Status_Stoerung!=0) 
        	err++;                                                                             	
        else
        	ok++;	
        // Summe / Durchschnitt	
        if (AEData[a].AE_Wakeup_ts < 1000) set_wakeup(a);	
        syield+=AEData[a].AE_today_yield;
        sawake += ((float)(time(NULL)-(AEData[a].AE_Wakeup_ts)));
        	
      }
      char * txt = getText(41);

			char soff[5];
      char serr[5];
      char sok[5];

      snprintf(sok,4,"%d",ok);
      snprintf(serr,4,"%d",err);
      snprintf(soff,4,"%d",off);
			
			sprintf(s+strlen(s),txt,"Wechselrichter-Status" ,off > 0 ? soff : "",err > 0 ? serr : "",ok > 0 ? sok : "");
			free(txt);

			txt = getText(46);
			snprintf(text,24,"%1.3f kWh",syield/1000);
			sprintf(s+strlen(s),txt,"Gesamtertrag", text);

			snprintf(text,24,"%1.1f Wh",syield/(sawake/3600));
			sprintf(s+strlen(s),txt,"Durchschnittsleistung", text);

			snprintf(text,24,"%d:%02d:%02d h",(int)(sawake/3600)/(err+ok),(int) (((int)((sawake)/(err+ok))) % 3600 / 60),(int) (((int)((sawake)/(err+ok))) % 60 ));
			sprintf(s+strlen(s),txt,"Durchschnittlich Online", text);

      int v = time(NULL)-startuptime;
      long hr = v/3600;
      long min = (v- (hr*3600)) /60;
      long sec = v - (hr*3600) - (min*60); 
   		snprintf(text,24,"%02ld:%02ld:%02ld",hr,min,sec);
			sprintf(s+strlen(s),txt,"AECLogger Uptime", text);
		  free(txt);
	}			 	
 return s;				 	
}

//**********************************************************************

void print_ev_data (void)
{
	char cmd[1000];
		
	sprintf(cmd,"/tmp/relais -csendevdata -p'?CMD=EVDATA&");
	sprintf(cmd+strlen(cmd),"P00=%d&", AEDeviceIndex);
	sprintf(cmd+strlen(cmd),"P01=%1.3f&", AEData[AEDeviceIndex].AE_current_power);
	sprintf(cmd+strlen(cmd),"P02=%1.3f&", AEData[AEDeviceIndex].AE_Reduzierung);
	sprintf(cmd+strlen(cmd),"P03=%1.3f'", AEData[AEDeviceIndex].AE_today_yield);
	system (cmd);
}

//**********************************************************************

void AE_Get_Ertragsdaten (char *buffer, int len)
{
	AEData[AEDeviceIndex].AE_current_power = AE_Get_FK16 ((char *)buffer+3);  // aktuelle Leistung
	if (datalog_active) print_ev_data ();
	double tmpyield = AE_Get_FK16 ((char *)buffer+7);
	if (tmpyield > AEData[AEDeviceIndex].AE_today_yield) 
		AEData[AEDeviceIndex].AE_today_yield = tmpyield;   // Summe heutige Leistung
	AEData[AEDeviceIndex].AE_TS_Current = time(NULL);	
	pushLiveLog(AEDeviceIndex);
	
}

//**********************************************************************

void AE_Get_Status (char *buffer, int len)
{
	AEData[AEDeviceIndex].AE_Status_Betrieb = AE_Get_INT ((char *)buffer+3+(0*4)); 
	AEData[AEDeviceIndex].AE_Status_Fehler = AE_Get_INT ((char *)buffer+3+(1*4));  
	AEData[AEDeviceIndex].AE_Status_Stoerung = AE_Get_INT ((char *)buffer+3+(2*4)); 
	AEData[AEDeviceIndex].AE_TS_Status = time(NULL);
}

//************************************************

char * AE_Print_Bit_Betrieb (int fmt, int index)
{
	char * s=calloc(3000,1);
	
  if (fmt == FMT_RAW) { snprintf(s,100,"not yet implemented");	}
				 	
	if (fmt == FMT_HTML) {
		char * txt = getText(45);
		
		sprintf(s+strlen(s),txt,"READY (AC)",(AEData[index].AE_Status_Betrieb & (1<<0)) > 0 ? "1-OK"   : "ERR");
		sprintf(s+strlen(s),txt,"ENOUGH_ENERGY:(DC)",(AEData[index].AE_Status_Betrieb & (1<<1)) > 0 ? "1-OK"   : "ERR");
		sprintf(s+strlen(s),txt,"MPP_INITIALIZED:",(AEData[index].AE_Status_Betrieb & (1<<2)) > 0 ? "1"      : "0");
		sprintf(s+strlen(s),txt,"POWER_LIMIT:",(AEData[index].AE_Status_Betrieb & (1<<3)) > 0 ? "RED." : "0-OK");
		sprintf(s+strlen(s),txt,"TRANSMIT:",(AEData[index].AE_Status_Betrieb & (1<<4)) > 0 ?  "1" : "0");
		sprintf(s+strlen(s),txt,"CALIBRATE:",(AEData[index].AE_Status_Betrieb & (1<<5)) > 0 ?  "1" : "0");
		sprintf(s+strlen(s),txt,"STORE_SETUP:",(AEData[index].AE_Status_Betrieb & (1<<6)) > 0 ?  "1" : "0");
		sprintf(s+strlen(s),txt,"ATTINY_OK:",(AEData[index].AE_Status_Betrieb & (1<<7)) > 0 ?  "1-OK" : "0-ERR");
		sprintf(s+strlen(s),txt,"REGLER_GESPERR:",(AEData[index].AE_Status_Betrieb & (1<<8)) > 0 ?  "1-ERR" : "0-OK");
		sprintf(s+strlen(s),txt,"ATTINY_PARAM_OK:",(AEData[index].AE_Status_Betrieb & (1<<9)) > 0 ?  "1-OK" : "0-ERR");
		sprintf(s+strlen(s),txt,"STROMMANGEL:",(AEData[index].AE_Status_Betrieb & (1<<10)) > 0 ? "1-ERR" : "0-OK");
		sprintf(s+strlen(s),txt,"SYNC (AC):",(AEData[index].AE_Status_Betrieb & (1<<11)) > 0 ? "1-OK" : "0-ERR");
		free(txt);
	}			 	
 return s;				 	
}


//************************************************

char * AE_Print_Bit_Fehler (int fmt, int index)
{
	char * s=calloc(10000,1);
	 
	if (fmt == FMT_RAW) { snprintf(s,100,"not yet implemented");	}	 	
	
	if (fmt == FMT_HTML) {
	  char * txt = getText(45);
		sprintf(s+strlen(s),txt,"ERROR_TEMP_SENSOR",(AEData[index].AE_Status_Fehler & (1<<0)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_TEMP_HIGH",(AEData[index].AE_Status_Fehler & (1<<1)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_IAC_CRITICAL",(AEData[index].AE_Status_Fehler & (1<<2)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_UAC_HIGH",(AEData[index].AE_Status_Fehler & (1<<3)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_UAC_HIGH_CRITICAL",(AEData[index].AE_Status_Fehler & (1<<4)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_UAC_LOW_CRITICAL",(AEData[index].AE_Status_Fehler & (1<<6)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_FREQUENCY_LOW",(AEData[index].AE_Status_Fehler & (1<<7)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_FREQUENCY_HIGH",(AEData[index].AE_Status_Fehler & (1<<8)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_EEPROM_CRC",(AEData[index].AE_Status_Fehler & (1<<9)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_FREQ_LOW",(AEData[index].AE_Status_Fehler & (1<<10)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_FREQ_HIGH",(AEData[index].AE_Status_Fehler & (1<<11)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_UAC_LOW",(AEData[index].AE_Status_Fehler & (1<<12)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_UAC_HIGH",(AEData[index].AE_Status_Fehler & (1<<13)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_TIMEOUT",(AEData[index].AE_Status_Fehler & (1<<14)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_HOST_TIMEOUT",(AEData[index].AE_Status_Fehler & (1<<15)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_U_PV_HIGH",(AEData[index].AE_Status_Fehler & (1<<16)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_SOFORTABSCHALTUNG",(AEData[index].AE_Status_Fehler & (1<<17)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_INSELERKENNUNG",(AEData[index].AE_Status_Fehler & (1<<18)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_RELAIS_ATTINY",(AEData[index].AE_Status_Fehler & (1<<19)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_RELAIS_SAM7",(AEData[index].AE_Status_Fehler & (1<<20)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_RELAIS_ATTINY",(AEData[index].AE_Status_Fehler & (1<<21)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_AC_NOTAUS",(AEData[index].AE_Status_Fehler & (1<<22)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_ATTINY_UNPLAUSIBEL",(AEData[index].AE_Status_Fehler & (1<<23)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_DC_LEISTUNG",(AEData[index].AE_Status_Fehler & (1<<24)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_UAC_LOW",(AEData[index].AE_Status_Fehler & (1<<25)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_NEUSTART_VERBOTEN",(AEData[index].AE_Status_Fehler & (1<<26)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"ERROR_SURGETEST",(AEData[index].AE_Status_Fehler & (1<<27)) > 0 ? "1"   : "0");
		free(txt);
	}			 	
 return s;				 	
}

//************************************************

char * AE_Print_Bit_Stoerung (int fmt, int index)
{
	char * s=calloc(5000,1);
		 
	if (fmt == FMT_RAW) {snprintf(s,100,"not yet implemented");	}	 	
	
	if (fmt == FMT_HTML) {
		char * txt = getText(45);
		sprintf(s+strlen(s),txt,"DISTURB_PLL_LOW",(AEData[index].AE_Status_Stoerung & (1<<0)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_ADC_CONVERSION",(AEData[index].AE_Status_Stoerung & (1<<1)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_UDC_OVERFLOW",(AEData[index].AE_Status_Stoerung & (1<<2)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_UDC_LOW",(AEData[index].AE_Status_Stoerung & (1<<3)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_IAC_HIGH",(AEData[index].AE_Status_Stoerung & (1<<4)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_IAC_ZERO",(AEData[index].AE_Status_Stoerung & (1<<5)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_PI_HIGH",(AEData[index].AE_Status_Stoerung & (1<<6)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_PI_LOW",(AEData[index].AE_Status_Stoerung & (1<<7)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_ADC_LOW",(AEData[index].AE_Status_Stoerung & (1<<8)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_1",(AEData[index].AE_Status_Stoerung & (1<<9)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_2",(AEData[index].AE_Status_Stoerung & (1<<10)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_3",(AEData[index].AE_Status_Stoerung & (1<<11)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_4",(AEData[index].AE_Status_Stoerung & (1<<12)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_5",(AEData[index].AE_Status_Stoerung & (1<<13)) > 0 ? "1"   : "0");
		sprintf(s+strlen(s),txt,"DISTURB_FREQU_6",(AEData[index].AE_Status_Stoerung & (1<<14)) > 0 ? "1"   : "0");
		free(txt);
	}			 	
 return s;				 	
}


//**********************************************************************

void AE_Get_Laender (char *buffer, int len)
{
	AEData[AEDeviceIndex].AE_Nennspannung                = AE_Get_FK16 ((char *)buffer+3+(0*4)); 
	AEData[AEDeviceIndex].AE_Obere_Abschaltspannung      = AE_Get_FK16 ((char *)buffer+3+(4)); 
	AEData[AEDeviceIndex].AE_Abschaltzeit_Ueberspannung  = AE_Get_SHORT ((char *)buffer+3+(8)); 
	AEData[AEDeviceIndex].AE_Kritische_Ueberspannung     = AE_Get_FK16 ((char *)buffer+3+(10)); 
  AEData[AEDeviceIndex].AE_Unknown                     = AE_Get_SHORT ((char *)buffer+3+(14)); 
  AEData[AEDeviceIndex].AE_Untere_Abschaltspannung     = AE_Get_FK16 ((char *)buffer+3+(16)); 
  AEData[AEDeviceIndex].AE_Abschaltzeit_Unterspannung  = AE_Get_SHORT ((char *)buffer+3+(20)); 
  AEData[AEDeviceIndex].AE_Nennfrequenz                = AE_Get_FK16 ((char *)buffer+3+(22)); 
  AEData[AEDeviceIndex].AE_Untere_Abschaltfrequenz     = AE_Get_FK16 ((char *)buffer+3+(26)); 
  AEData[AEDeviceIndex].AE_Abschaltzeit_Unterfrequenz  = AE_Get_SHORT ((char *)buffer+3+(30)); 
  AEData[AEDeviceIndex].AE_Obere_Abschaltfrequenz      = AE_Get_FK16 ((char *)buffer+3+(32)); 
  AEData[AEDeviceIndex].AE_Abschaltzeit_Ueberfrequenz  = AE_Get_SHORT ((char *)buffer+3+(36)); 
  AEData[AEDeviceIndex].AE_Max_DC_Anteil               = AE_Get_FK16 ((char *)buffer+3+(38)); 
  AEData[AEDeviceIndex].AE_Abschaltzeit_Ueber_MAX_DC   = AE_Get_SHORT ((char *)buffer+3+(42)); 
  AEData[AEDeviceIndex].AE_Abschaltzeit_Inselerkennung = AE_Get_SHORT ((char *)buffer+3+(44)); 
  AEData[AEDeviceIndex].AE_Wiedereinschalt_Fehler_lt3  = AE_Get_SHORT ((char *)buffer+3+(46)); 
  AEData[AEDeviceIndex].AE_Wiedereinschalt_Fehler_gt3  = AE_Get_SHORT ((char *)buffer+3+(48)); 
  AEData[AEDeviceIndex].AE_Unterspannung_ATT           = AE_Get_SHORT ((char *)buffer+3+(50)); 
  AEData[AEDeviceIndex].AE_Ueberspannung_ATT           = AE_Get_SHORT ((char *)buffer+3+(52)); 
  AEData[AEDeviceIndex].AE_Ueberfrequenz_ATT           = AE_Get_SHORT ((char *)buffer+3+(54)); 
  AEData[AEDeviceIndex].AE_Unterfrequenz_ATT           = AE_Get_SHORT ((char *)buffer+3+(56)); 
  AEData[AEDeviceIndex].AE_TS_Country = time(NULL);
}

//**********************************************************************

void AE_Get_Fehlercode(char * buffer,int len )
{
	printf("Errorcode\r\n");
	printf("\tTimestamp: %d\r\n",AE_Get_INT ((char *)buffer+3+(0*4)));
}

//**********************************************************************

void AE_Get_Status_Leistungsreduzierung(char * buffer,int len )
{
 AEData[AEDeviceIndex].AE_Reduzierung = AE_Get_FK16 ((char *)buffer+3+(0*4));	
 
 if ((AEData[AEDeviceIndex].AE_Status_Betrieb & (1<<0)) <= 0  // NOT AC-READY
 	   || (AEData[AEDeviceIndex].AE_Status_Betrieb & (1<<11)) <= 0  ) // NO SYNC
 	   AEData[AEDeviceIndex].AE_Reduzierung = -1;
}

//**********************************************************************

char *  AE_Print_Laender (int fmt,int index)
{
	char * s=calloc(5000,1);

	if (fmt == FMT_RAW) { snprintf(s,100,"not yet implemented");	}

  if (fmt == FMT_HTML) {
		char * txt = getText(42);
		char format[30];
		snprintf(format,29,"%3.1f",AEData[AEDeviceIndex].AE_Nennspannung);	sprintf(s+strlen(s),txt,"Nennspannung",format," V~");
		snprintf(format,29,"%3.1f",AEData[AEDeviceIndex].AE_Obere_Abschaltspannung);	sprintf(s+strlen(s),txt,"Obere Abschaltspannung",format,"V~");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Abschaltzeit_Ueberspannung/50.0);	sprintf(s+strlen(s),txt,"Abschaltzeit Überspannung (Anzahl Perioden)",format,"s");
		snprintf(format,29,"%3.1f",AEData[AEDeviceIndex].AE_Untere_Abschaltspannung);	sprintf(s+strlen(s),txt,"Untere Abschaltspannung",format,"V~");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Abschaltzeit_Unterspannung/50.0);	sprintf(s+strlen(s),txt,"Abschaltzeit Unterspannung",format,"s");
		snprintf(format,29,"%3.2f",AEData[AEDeviceIndex].AE_Nennfrequenz);	sprintf(s+strlen(s),txt,"Nennfrequenz",format,"Hz");
		snprintf(format,29,"%3.2f",AEData[AEDeviceIndex].AE_Nennfrequenz-AEData[AEDeviceIndex].AE_Untere_Abschaltfrequenz);	sprintf(s+strlen(s),txt,"Untere Abschaltfrequenz",format,"Hz");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Abschaltzeit_Unterfrequenz/50.0);	sprintf(s+strlen(s),txt,"Abschaltzeit Unterfrequenz",format,"s");
		snprintf(format,29,"%3.2f",AEData[AEDeviceIndex].AE_Nennfrequenz+AEData[AEDeviceIndex].AE_Obere_Abschaltfrequenz);	sprintf(s+strlen(s),txt,"Obere Abschaltfrequenz",format,"Hz");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Abschaltzeit_Ueberfrequenz/50.0);	sprintf(s+strlen(s),txt,"Abschaltzeit Überfrequenz",format,"s");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Abschaltzeit_Inselerkennung/50.0);	sprintf(s+strlen(s),txt,"Abschaltzeit Inselerkennung",format,"s");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Wiedereinschalt_Fehler_lt3/50.0);	sprintf(s+strlen(s),txt,"Wiedereinschaltzeit bei Fehlern < 3s",format,"s");
		snprintf(format,29,"%3.2f",(float)AEData[AEDeviceIndex].AE_Wiedereinschalt_Fehler_gt3/50.0);	sprintf(s+strlen(s),txt,"Wiedereinschaltzeit bei Fehlern > 3s",format,"s");
		free(txt);
	}			 	
  return s;	
}
//**********************************************************************


void AE_Get_Geraeteparameter (char *buffer, int len)
{
	AEData[AEDeviceIndex].AE_ID = AE_Get_INT ((char *)buffer+3);  
  memcpy (AEData[AEDeviceIndex].AE_Typ,((char *)buffer+3+(1*4)),8); 
  AEData[AEDeviceIndex].AE_Typ[8]=0; 
  AEData[AEDeviceIndex].AE_Schueco = AE_Get_INT ((char *)buffer+3+(3*4));  
  AEData[AEDeviceIndex].AE_LCode = AE_Get_INT ((char *)buffer+3+(4*4));  
  AEData[AEDeviceIndex].AE_CRC_Aktiv = AE_Get_INT ((char *)buffer+3+(5*4));  
  AEData[AEDeviceIndex].AE_CRC_Backup = AE_Get_INT ((char *)buffer+3+(6*4));  
  AEData[AEDeviceIndex].AE_Bootcode = AE_Get_INT ((char *)buffer+3+(7*4));  
  AEData[AEDeviceIndex].AE_LCode_Aenderung = AE_Get_INT ((char *)buffer+3+(8*4));  
  AEData[AEDeviceIndex].AE_LCode_Fehler = AE_Get_INT ((char *)buffer+3+(9*4));  
  AEData[AEDeviceIndex].AE_Anzahl_Boot = AE_Get_INT ((char *)buffer+3+(10*4));  
  AEData[AEDeviceIndex].AE_Anzahl_Codefehler = AE_Get_INT ((char *)buffer+3+(11*4));  
  memcpy (AEData[AEDeviceIndex].AE_Version_SW,((char *)buffer+3+(12*4)),8); 
  AEData[AEDeviceIndex].AE_Version_SW[8]=0; 
  memcpy (AEData[AEDeviceIndex].AE_Version_HW,((char *)buffer+3+(14*4)),8); 
  AEData[AEDeviceIndex].AE_Version_HW[8]=0; 
  AEData[AEDeviceIndex].AE_Max_Leistung = AE_Get_FK16  ((char *)buffer+3+(16*4)); 
  AEData[AEDeviceIndex].AE_TS_Parameter = time(NULL);
}

//------------------

char *  AE_Print_Parameter (int fmt,int index)
{
	char * s=calloc(5000,1);

	if (fmt == FMT_RAW) {snprintf(s,100,"not yet implemented");	}
	
  if (fmt == FMT_HTML) {
		char * txt = getText(42);
		char format[30];
		snprintf(format,29,"%u",AEData[index].AE_ID);	sprintf(s+strlen(s),txt,"ID",format,"");
		                                        	sprintf(s+strlen(s),txt,"Typ",AEData[index].AE_Typ,"");
		snprintf(format,29,"%u",AEData[index].AE_Schueco);	sprintf(s+strlen(s),txt,"Schueco-Id",format,"");
		snprintf(format,29,"%d  (Ä:%d  F:%d)",AEData[index].AE_LCode,AEData[index].AE_LCode_Aenderung,AEData[index].AE_LCode_Fehler);	
		                                          sprintf(s+strlen(s),txt,"Laendercode",format,"");
		                                          sprintf(s+strlen(s),txt,"CRC",AEData[index].AE_CRC_Aktiv==AEData[index].AE_CRC_Backup ? "OK" : "ERROR","");
		snprintf(format,29,"%d",AEData[index].AE_Bootcode);	sprintf(s+strlen(s),txt,"Bootcode",format,"");

		snprintf(format,29,"%d",AEData[index].AE_Anzahl_Boot);	sprintf(s+strlen(s),txt,"Anzahl Boot",format,"");
		snprintf(format,29,"%d",AEData[index].AE_Anzahl_Codefehler);	sprintf(s+strlen(s),txt,"Anzahl Codefehler",format,"");
		snprintf(format,29,"S %s H %s",AEData[index].AE_Version_SW,AEData[index].AE_Version_HW);	sprintf(s+strlen(s),txt,"Version",format,"");
		snprintf(format,29,"%3.0f W",AEData[index].AE_Max_Leistung);	sprintf(s+strlen(s),txt,"Max. Leistung",format,"");
		free(txt);
	}			 	
  return s;	
}
//**********************************************************************

void Dump_Buffer (char* buffer, int len, int cs)
{
	int a;
	printf("\r\n------Dump---CS=%02X --LEN= %3d -----\r\n",cs,len);
	for (a=0; a<len; a++)  
	   printf("%3d : %02X %03d %c\r\n",a,buffer[a],buffer[a], buffer[a] > 32 & buffer[a]< 128 ? buffer[a] : ' ');
	printf("------------------------\r\n");
}

//**********************************************************************

int AE_CheckAnswer (unsigned char * buffer, int len) 
{
	int a;
	unsigned char cs = 0;
	for (a=1; a<len-2; a++){
		 if (buffer[a] == 0x40 && buffer[a+1] == 0x0d) continue;
	   if (buffer[a] == 0x40 && buffer[a+1] == 0x40) continue;
	   cs ^= buffer[a];
	  }
	   
	   
//Dump_Buffer(buffer,len,cs); 	   
	if (!(buffer[0] == 0x21 && buffer[len-1]== 0x0d && buffer[len-2] == (char)cs)) {
		 Dump_Buffer(buffer,len,cs);  
		 return 0;
		 }
	return 1;	 
		 

}  


//**********************************************************************

int AE_Analyze (char *buffer, int len)
{

 if (!AE_CheckAnswer(buffer,len)) return 0;
 
 switch ((int)buffer[1] * 256 + (int)buffer[2]) {
 	  case 10002 : // Betriebsdaten                 <---- 1005
 	  	           AE_Get_Betriebsdaten (buffer, len);
 	  	           break;
 	  case 10007 : // Ertragsdaten	                <---- 1021 
 	  	           AE_Get_Ertragsdaten (buffer, len);
 	  	           break;           
 	  case 10003 : // Fehler/Störstatus	            <---- 1008
 	  	           AE_Get_Status (buffer, len);
 	  	           break;           
 	  case 10005 : // Geräteparameter 	            <---- 1014
 	  	           AE_Get_Geraeteparameter (buffer, len);
 	  	           break;           
 	  case 10000 : // Acknowledge nach Änderungen
 	  	           return 10000;
 	  	           break;	           
 	  case 10001 : // Fehler nach Änderungen
 	  	           return 10001;
 	  	           break;	  
 	  case 1006  : // Laendereinstellungen          <---- 1007
 	  	           AE_Get_Laender (buffer,len);
 	  	           break;	
    case  10012 : // Fehlercode                    <-----1030 
   	             AE_Get_Fehlercode(buffer,len);     	  	                               
   	             break;
    case   1022 : // Status Leistungsreduzierung   <-----1023
                 AE_Get_Status_Leistungsreduzierung(buffer,len);
                 break;  
   }	
 
 
 return 1;	
 
}

//**********************************************************************

char AE_Checksum (char * buffer, int len) 
{
	int a, cs = 0;
	for (a=1; a<len; a++){
	   if (buffer[a] == 0x40 && buffer[a+1] == 0x0d) continue;
	   if (buffer[a] == 0x40 && buffer[a+1] == 0x40) continue;
	   cs ^= buffer[a];
	   }
	return cs;
}  


//**********************************************************************

int AE_Request (int idx, unsigned short type, int value)
{
	int r,ct;
	char req[200];
	unsigned short id ;

  AEDeviceIndex=idx;
  id = AEDevices[idx];
	
	r = ct = 0;
	req[ct++] = 0x21;
  req[ct++] = ((char*)(&id))[1];	
	req[ct++] = ((char*)(&id))[0];	
	req[ct++] = ((char*)(&type))[1];		
	req[ct++] = ((char*)(&type))[0];	

	
	if (type == 1022) { //Leistungsreduzierung
		union clong cv;
		cv.i = value * 65536;
		req[ct++] = cv.b[3];
		if (cv.b[3] == 0x40) req[ct++] = 0x40;
		if (cv.b[3] == 0x0D) {
			   req[ct-1] = 0x40;
			   req[ct++] = 0x0D;
		    }	
			
		req[ct++] = cv.b[2];
		if (cv.b[2] == 0x40) req[ct++] = 0x40;
			if (cv.b[2] == 0x0D) {
			   req[ct-1] = 0x40;
			   req[ct++] = 0x0D;
		    }	
		req[ct++] = cv.b[1];
		if (cv.b[1] == 0x40) req[ct++] = 0x40;
			if (cv.b[1] == 0x0D) {
			   req[ct-1] = 0x40;
			   req[ct++] = 0x0D;
		    }	
		req[ct++] = cv.b[0];
		if (cv.b[0] == 0x40) req[ct++] = 0x40;
			if (cv.b[0] == 0x0D) {
			   req[ct-1] = 0x40;
			   req[ct++] = 0x0D;
		    }	
	}
	
  if (type == SET_SCHUECOID) {
  	req[ct++] = (unsigned char) value;
  }

  unsigned char cs = AE_Checksum(req,ct);
  if (cs == 0x40) {
  	 req[ct++] = 0x40;
  	 req[ct++] = 0x40;
    }
  else {
  	   if (cs == 0x0D) {
		  	 req[ct++] = 0x40;
		  	 req[ct++] = 0x0D;
		    }
  	   else {
  	   	req[ct++] = cs;
  	    }
       }  
	//req[ct++] = AE_Checksum(req,ct);
	req[ct++] = 0x0D;


  if (logit & 0x02 && type == 1022) {
		  printf("\r\n. ");
		  for (r=0; r < ct; r++) {
		  	 printf("%02X ",req[r]); 
		  }
   }
   
  SendSerial(req,ct);	
  r = ReadSerial();	
  
  if (r>0)  r = AE_Analyze (sbuf, r);
  	
  		
  if (r==0) {  // Error - no Answer or wrong CS
  	// Wenn das erste mal offline ...
     if (AEData[idx].AE_Offline == 0) {
		  	   AEData[idx].AE_Offline_ts = time(NULL);
		 	     saveLiveLog (idx, 1);
		 	     saveDayLog (idx, 1);
		       }
		 else  
		  	 AEData[idx].AE_Offline_ct++;
		  	 
		 AEData[idx].AE_Offline = 1;
		 //printf ("%s offline[%d] (%d)\r\n",getTimeString(),idx,AEData[idx].AE_Offline_ct);fflush(stdout);
     }
    else {  // Success
  	  AEData[idx].AE_Lastrequest_ts = time(NULL); 
		  // prüfen ob wieder online -  Aufwachen, seit 10 Minuten stabile Antworten auf requests oder innerhalb einer Minute ein Aussetzter
		  if (AEData[idx].AE_Offline) 
		 	   if ((AEData[idx].AE_Lastrequest_ts > AEData[idx].AE_Offline_ts + 600) ||
		 		      (AEData[idx].AE_Lastrequest_ts < AEData[idx].AE_Offline_ts + 60 && AEData[idx].AE_Offline_ct <= 5)) {
		 		      	  //printf ("%s online[%d]\r\n",getTimeString(),idx);fflush(stdout);
		     		  	  AEData[idx].AE_Offline = 0;
				 		  	  AEData[idx].AE_Offline_ct = 0;
				 		  	  AEData[idx].AE_Online_ts = time(NULL);
		         }
  	  }

  
  return r;
}


//************************************************

void write_reduce(int fd,int dev, int power) 
{
 double value;	
 if (power < 0 || power > 450)	
 	 value = 0;
 else 
 	 value = (double)power;	 
 AEData[dev].AE_Reduzierung_set = value;	
 AE_Request (dev,SET_LEISTUNGSREDUZIERUNG,value);
 send_http_header(fd,200);
 AE_Request (dev,GET_LEISTUNGSREDUZIERUNG,0);
}

/* init our tcphandler-array - for async script and rpc
--------------------------------------------------------*/

void init_fdarrays()
{
 int a;
 for (a=0; a<MAXTCPHANDLER;a++) {
   tcphd[a].fd = -1;  
   }
 
}

/* find a free tcphandler
--------------------------------------------------------*/

struct s_tcphd * find_free_tcphandler ()
{
 int a;
 for (a=0;a<MAXTCPHANDLER;a++){
    if (tcphd[a].fd == -1){
      return ((struct s_tcphd *) (tcphd+a));   
      }
   }
 return NULL;  
}


//************************************************

char *sendgetrequest (char *msg, char * host, int port,char * head)
{
    int portno, n;
    struct s_tcphd * hd = NULL;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *buffer;
    unsigned long msize = 0;
    
    globalgetsize = 0;
    hd = find_free_tcphandler();
    if (hd == NULL) {
       return NULL;  
      } 
    
    hd->param = NULL;  
    server = gethostbyname(host);
    if (server == NULL) {
        server =  gethostbyname(host);
        if (server == NULL){
          return (NULL);
          }
      }
      
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
                     
    hd->fd = socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(hd->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    
    struct timeval timeout;
    timeout.tv_sec = 5;  // set timeout 
	  timeout.tv_usec = 0;
    setsockopt(hd->fd, SOL_SOCKET, SO_RCVTIMEO , &timeout, sizeof timeout);
    setsockopt(hd->fd, SOL_SOCKET, SO_SNDTIMEO , &timeout, sizeof timeout);
   
   
    int res;
    long arg;
    struct timeval tv;
    int valopt;
    fd_set myset;
    socklen_t lon;

    arg = fcntl(hd->fd, F_GETFL, NULL); 
    arg |= O_NONBLOCK; 
    fcntl(hd->fd, F_SETFL, arg); 
   
    res = connect(hd->fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    
    if (res < 0) { 
     if (errno == EINPROGRESS) { 
        tv.tv_sec = 5; 
        tv.tv_usec = 0; 
        FD_ZERO(&myset); 
        FD_SET(hd->fd, &myset); 
        if (select(hd->fd+1, NULL, &myset, NULL, &tv) > 0) { 
           lon = sizeof(int); 
           getsockopt(hd->fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
           if (valopt) {
     	     close(hd->fd);
           hd->fd = -1;
     	     return (NULL); 
     	    } 
        } 
        else {
     	     close(hd->fd);
           hd->fd = -1;
     	     return (NULL); 
     	    }
     } 
     else {
     	     close(hd->fd);
           hd->fd = -1;
     	     return (NULL); 
     	    }
    } 

    arg = fcntl(hd->fd, F_GETFL, NULL); 
    arg &= (~O_NONBLOCK); 
    fcntl(hd->fd, F_SETFL, arg); 
    
   msize = 150+strlen ((char *) msg);
   buffer = malloc (msize); 
   sprintf((char*) buffer, "GET /%s HTTP/1.1\r\n"
                           "Host: %s\r\n"           
                           "Content-Type: text/html\r\n%s"
                           "Connection: close\r\n\r\n", msg, host, head==NULL ? "" : head );
  

    n = write(hd->fd,buffer,strlen((char*)buffer));
    unsigned long csize = 0;
    buffer[0]=0;
    csize=0;  
    int sleepctr = 0;       
    do {
       n = read(hd->fd,buffer+csize,100);
       csize+=n;
       if (msize-csize < 100) {
          buffer = realloc(buffer,msize+1000);
          msize+=1000;         
         }
        if (n==0) { 
        	         usleep(100);
        	         sleepctr++;  
        	        }
       } while (n > 0 && sleepctr < 2);
       
    buffer[csize]=0;
    
    close (hd->fd);
    hd->fd = -1;
    globalgetsize = csize;
    return (buffer);
}

//************************************************

int main (int argc, char *argv[])
{
  int selectanswer;
  struct timeval timeout;
  time_t current_time;
  time_t reqticker=0;
  time_t logticker=LONG_MAX;
  time_t usbticker=0;
  time_t powticker=0;
  startuptime=time(NULL);
  int c;
  int overtake = 0;
  int fdc;
 
  while ((c = getopt (argc, argv, "p:c:do")) != -1){
	   switch (c) {
	     case 'd':
	       dontfork = 1;
	       break; 
	     case 'o':
	       overtake = 1;
	       break; 
	     case 'p':
	       program_path = malloc (strlen(optarg)+1);
	       strcpy (program_path,optarg);
	       break;
	     case 'c':
	       param_path = malloc (strlen(optarg)+1);
	       strcpy (param_path,optarg);
	       break;  
	     default:
	     	 printf("unbekannter Parameter %c  - Abbruch",c);
	       exit(0);
	     }
  }
  
  if (program_path==NULL) 
  	 program_path = default_path; 
  	 
  if (param_path==NULL) 
  	 param_path = default_path; 
  
   getParameters ();
   init_fdarrays();
   if (overtake) {
	   char * answer = sendgetrequest ("?CMD=OVERTAKE","127.0.0.1", listenport,NULL);
	   if (globalgetsize > 0)
	  	  if (strstr(answer,"OVERTAKEOK")) {
	  	  	printf("OK - new process\r\n");fflush(stdout);
	  	  	free(answer);
	  	  }
     } 
   else { 
	   char * answer = sendgetrequest ("?CMD=ISALIVE","127.0.0.1", listenport,NULL);
	   if (globalgetsize > 0)
	  	  if (strstr(answer,"AECLOGGEROK")) {
	  	  	printf("OK - is alive\r\n");fflush(stdout);
	  	  	exit(0);
	  	  }
  	}

   if (!dontfork) { // for debugging
	  pid_t pid = fork();
	  if (pid < 0) { _exit(0);}      
	  if (pid > 0) { _exit(0);} // exit this process we forked successful
	  setsid(); 
  }
 
  printf("Starte AECLogger mit \r\n\r\n\t Programmpfad='%s'\r\n"
  	                                  "\t Config='%saeclogger.ini'\r\n"
                                      "\t Debug=%d\r\n"
                                      "\t HTTP-Port=%d\r\n"
                                      "\t HS485-Port='%s'\r\n"
                                      "\t tmp Verzeichnis='%s'\r\n"
                                      "\t Log Verzeichnis='%s'\r\n"
                                      "\t Log Intervall= %d Sek.\r\n"
                                      "\t Logrequest Intervall= %d Sek.\r\n"
                                      "\t Powerrequest Intervall= %d Sek.\r\n"
                                      "\r\nEingetragene Inverter:\r\n\r\n",
          program_path,param_path, dontfork, listenport, serialport, tmp_path, log_path, log_interval,req_interval,pow_interval);
  init(); 
   
  if (getUSBID(serialidentifier)) initSerialPort(); 
  
  
  buildLogList();
  
  for (c=0; c<AENumDevices; c++) {   
  	if (AEData[c].AE_ChartColor == 0) 
  		switch(c) {
  			case 0: AEData[c].AE_ChartColor=0x117711;break;
				case 1: AEData[c].AE_ChartColor=0x22AA22;break;
				case 2: AEData[c].AE_ChartColor=0x33FF33;break;
				case 3: AEData[c].AE_ChartColor=0xDDAA00;break;
				case 4: AEData[c].AE_ChartColor=0xDD7700;break;
     
  		}
    printf("\t Inverterid %d : %d (%s) Reduzierung default:%1.0f Color:%06X\r\n",c,AEDevices[c], AEDevicesComment[c],AEData[c].AE_Reduzierung_set,AEData[c].AE_ChartColor);
  }
  printf("...\r\n");  	    
  
  timeout.tv_sec  = 0; 
	timeout.tv_usec = 250000; 

  initDayLogs();
  initLiveLogs();

  for (c=0; c<AENumDevices; c++) {
      loadDayLog(c,1);
      loadLiveLog(c,1);
     }

  while (1) {
		  read_fd_set = active_fd_set;  // save the select-set
		  timeout.tv_usec = queueentries != 0 ? 10000 : 500000;
		  selectanswer = select (FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);	 
		
		  if ( selectanswer != 0 ) {
				 for (fdc = 0; fdc < FD_SETSIZE; ++fdc)
					 if (FD_ISSET (fdc, &read_fd_set)) {
				      //  Request über Eventport
				 	  	if (fdc == eventfd) 	{
					       if (newevfd = accept(eventfd,(struct sockaddr *) &eventcli_addr, &eventclilen)) {
				            processevent(newevfd);
				            close (newevfd);
				           } 
				         break;
					      }
				    }
		  }
		  else { // cyclic non-select
		  	
		  	
		  	
		  	 current_time = time(NULL);
		  	
		  	 if (usbticker <= current_time) {
		  	 	 usbticker = current_time +300;
		  	   if (current_time - tslastserial > 20)
		  	       if (getUSBID(serialidentifier)) 
		  	       	  initSerialPort(); 
		  	   	
		  	   }
		  	
		  	 if (logticker <= current_time) {  // LOG
             //if min. one inv active -> push logs        
		  	 	   logticker = current_time + log_interval;
		         logtime = current_time;
             int i,off;
             for (i=0, off=0; i<AENumDevices; i++) { 
           	      if (AEData[i].AE_Offline) off++;
                }
             changeLogDay(); // check & set logdate
             //if min. one inv active -> push logs        
             if (off < AENumDevices) 
           	      pushDayLog();
		  	 }
		  	
		     if (queueentries != 0) {  // PULL REQUESTS
		     	     if (queue[0].type == GET_STATUS) {
		       	    if (AE_Request (queue[0].deviceindex, GET_STATUS,0)== 0) { 
   		   	   	     AEData[queue[0].deviceindex].AE_error++; 
   		   	   	     pullQueueEntry();
                   DeleteDeviceEntriesFromQueue	(queue[0].deviceindex);
                   continue;	       	       
		       	    }
		       	     pullQueueEntry();
		       	     continue;
		       	  }  
		       	  AE_Request(queue[0].deviceindex, queue[0].type,queue[0].parameter); 
		       	  pullQueueEntry();
		       	  if (queueentries == 0 && logticker == LONG_MAX)  // first complete run 
		       	  	  logticker = 0;
		       	  continue;
		      }
		     
		      
		      	
		    
		    	if (reqticker <= current_time) { // PUSH POWERREQUESTS
 		        int i;
		        reqticker = current_time + req_interval;
		        for (i=0; i<AENumDevices; i++) {
				       	pushQueueEntry (i, GET_STATUS, 0);
				       	if (current_time - AEData[i].AE_TS_Parameter > 300)
				       		  pushQueueEntry (i, GET_PARAMETER, 0);
				       	if (current_time - AEData[i].AE_TS_Country > 300)
				       		  pushQueueEntry (i, GET_LAENDER, 0);
						    if (AEData[i].AE_Reduzierung > 10 && AEData[i].AE_Reduzierung_set > 10 && AEData[i].AE_Reduzierung_set != AEData[i].AE_Reduzierung) 
				          pushQueueEntry (i, SET_LEISTUNGSREDUZIERUNG, AEData[i].AE_Reduzierung_set);

						    pushQueueEntry (i, GET_BETRIEBSDATEN, 0);
						    pushQueueEntry (i, GET_LEISTUNGSREDUZIERUNG, 0); 
			         }
		        } 
		      
		       
		      if (powticker <= current_time) { // PUSH POWERREQUESTS
 		        int i;
		        powticker = current_time + pow_interval;
		        for (i=0; i<AENumDevices; i++) {
		        	if (!AEData[i].AE_Offline)
			        pushQueueEntry (i, GET_ERTRAGSDATEN, 0);
		        } 
		      } 
		      
		      
		   }
  } // select loop
   
	
 }
	
	

