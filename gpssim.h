#ifndef GPSSIM_H
#define GPSSIM_H

//#define FLOAT_CARR_PHASE // For RKT simulation. Higher computational load, but smoother carrier phase.

// Navigation message frame constants
#define N_DWRD_SBF 10 // Number of data words per subframe
#define N_SBF 5      // Number of subframes in a GPS L1 navigation message

#define TRUE	(1)
#define FALSE	(0)

/*! \brief Maximum length of a line in a text file (RINEX, motion) */
#define MAX_CHAR (100)

/*! \brief Maximum number of satellites in RINEX file */
#define MAX_SAT (32)

/*! \brief Maximum number of channels we simulate */
#define MAX_CHAN (16)

/*! \brief Maximum number of user motion points */
#ifndef USER_MOTION_SIZE
#define USER_MOTION_SIZE (3000) // max duration at 10Hz
#endif

/*! \brief Maximum duration for static mode*/
#define STATIC_MAX_DURATION (86400) // second

// L1C parameters 
#define L1C_CARRIER_FREQ    1575.42e6     /* L1C carrier frequency */
#define L1C_CODE_LENGTH     10230         /* L1C code length in chips */
#define L1C_CODE_RATE      1.023e6       /* L1C code rate [chips/s] */
#define L1C_SYMB_RATE      100.0         /* L1C symbol rate [symbols/s] */
#define L1C_DATA_RATE      50.0          /* L1C navigation data rate [bits/s] */
#define L1C_SYMB_LEN       10            /* L1C symbol length [ms] */

#define SECONDS_IN_WEEK 604800.0
#define SECONDS_IN_HALF_WEEK 302400.0
#define SECONDS_IN_DAY 86400.0
#define SECONDS_IN_HOUR 3600.0
#define SECONDS_IN_MINUTE 60.0

#define POW2_M5  0.03125
#define POW2_M19 1.907348632812500e-6
#define POW2_M29 1.862645149230957e-9
#define POW2_M31 4.656612873077393e-10
#define POW2_M33 1.164153218269348e-10
#define POW2_M43 1.136868377216160e-13
#define POW2_M55 2.775557561562891e-17

#define POW2_M50 8.881784197001252e-016
#define POW2_M30 9.313225746154785e-010
#define POW2_M27 7.450580596923828e-009
#define POW2_M24 5.960464477539063e-008

// Conventional values employed in GPS ephemeris model (ICD-GPS-200)
#define GM_EARTH 3.986005e14
#define OMEGA_EARTH 7.2921151467e-5
#define PI 3.1415926535898

#define WGS84_RADIUS	6378137.0
#define WGS84_ECCENTRICITY 0.0818191908426

#define R2D 57.2957795131

#define SPEED_OF_LIGHT 2.99792458e8
#define LAMBDA_L1 0.190293672798365

/*! \brief GPS L1 Carrier frequency */
#define CARR_FREQ (1575.42e6)
/*! \brief C/A code frequency */
#define CODE_FREQ (1.023e6)
#define CARR_TO_CODE (1.0/1540.0)

// Sampling data format
#define SC01 (1)
#define SC08 (8)
#define SC16 (16)

// Navigation message frame constants
#define N_DWRD_SBF 10 // Number of GPS L1 data words per subframe
#define N_SBF 5       // Number of subframes in a GPS L1 navigation message

#define EPHEM_ARRAY_SIZE (15) // for daily GPS broadcast ephemers file (brdc)

/*! \brief Structure representing GPS time */
typedef struct
{
	int week;	/*!< GPS week number (since January 1980) */
	double sec; 	/*!< second inside the GPS \a week */
} gpstime_t;

/*! \brief Structure repreenting UTC time */
typedef struct
{
	int y; 		/*!< Calendar year */
	int m;		/*!< Calendar month */
	int d;		/*!< Calendar day */
	int hh;		/*!< Calendar hour */
	int mm;		/*!< Calendar minutes */
	double sec;	/*!< Calendar seconds */
} datetime_t;

/*! \brief Structure representing ephemeris of a single satellite */
typedef struct
{
	int vflg;	/*!< Valid Flag */
	datetime_t t;
	gpstime_t toc;	/*!< Time of Clock */
	gpstime_t toe;	/*!< Time of Ephemeris */
	int iodc;	/*!< Issue of Data, Clock */
	int iode;	/*!< Isuse of Data, Ephemeris */
	double deltan;	/*!< Delta-N (radians/sec) */
	double cuc;	/*!< Cuc (radians) */
	double cus;	/*!< Cus (radians) */
	double cic;	/*!< Correction to inclination cos (radians) */
	double cis;	/*!< Correction to inclination sin (radians) */
	double crc;	/*!< Correction to radius cos (meters) */
	double crs;	/*!< Correction to radius sin (meters) */
	double ecc;	/*!< e Eccentricity */
	double sqrta;	/*!< sqrt(A) (sqrt(m)) */
	double m0;	/*!< Mean anamoly (radians) */
	double omg0;	/*!< Longitude of the ascending node (radians) */
	double inc0;	/*!< Inclination (radians) */
	double aop;
	double omgdot;	/*!< Omega dot (radians/s) */
	double idot;	/*!< IDOT (radians/s) */
	double af0;	/*!< Clock offset (seconds) */
	double af1;	/*!< rate (sec/sec) */
	double af2;	/*!< acceleration (sec/sec^2) */
	double tgd;	/*!< Group delay L2 bias */
	int svhlth;
	int codeL2;
	// Working variables follow
	double n; 	/*!< Mean motion (Average angular velocity) */
	double sq1e2;	/*!< sqrt(1-e^2) */
	double A;	/*!< Semi-major axis */
	double omgkdot; /*!< OmegaDot-OmegaEdot */
} ephem_t;

typedef struct
{
	int enable;
	int vflg;
	double alpha0,alpha1,alpha2,alpha3;
	double beta0,beta1,beta2,beta3;
	double A0,A1;
	int dtls,tot,wnt;
	int dtlsf,dn,wnlsf;
	// enable custom leap event
	int leapen;
} ionoutc_t;

typedef struct
{
	gpstime_t g;
	double range; // pseudorange
	double rate;
	double d; // geometric distance
	double azel[2];
	double iono_delay;
} range_t;

/*! \brief Structure representing an L1C Channel */
typedef struct {
    int prn;                    /*< PRN Number */
    int *l1c_code;             /*< L1C spreading code */
    int *l1c_data;             /*< L1C data component */
    int *l1c_pilot;            /*< L1C pilot component */
    double code_phase;          /*< Code phase */
    double carr_phase;          /*< Carrier phase */
    double f_carr;              /*< Carrier frequency */
    double f_code;              /*< Code frequency */
    int *symbols;               /*< CNAV-2 symbols */
    int sym_idx;                /*< Current symbol index */
    gpstime_t g0;              /*< GPS time at start */
    double azel[2];            /*< Azimuth and elevation */
    range_t rho0;              /*< Pseudorange data */
    int32_t carr_phasestep;    /*< Carrier phase step */
    unsigned long dwrd[N_DWRD_SBF * N_SBF + 1]; /*< Data words in a subframe */
    unsigned long sbf[5][N_DWRD_SBF];           /*< Current subframe */
    // L1C specific fields
    int *l1c_data_code;        /*< L1C data component spreading code */
    int *l1c_pilot_code;       /*< L1C pilot component spreading code */
    double l1c_data_amp;       /*< L1C data component amplitude */  
    double l1c_pilot_amp;      /*< L1C pilot component amplitude */
    int *l1c_symbols;          /*< L1C navigation data symbols */
    int l1c_sym_idx;           /*< Current L1C symbol index */
} channel_t;

// L1C function declarations 
void l1c_gen_code(int *code, int prn);
void l1c_gen_symbols(channel_t *chan);
void l1c_gen_signal(channel_t *chan, double *i_buff, double *q_buff, int length);

#endif
