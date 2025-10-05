#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926535898
#define RADIUS 100.0  // Circle radius in meters
#define CENTER_LAT 35.681298  // Tokyo latitude
#define CENTER_LON 139.766247 // Tokyo longitude
#define HEIGHT 100.0 // Height in meters above WGS84
#define NUM_POINTS 3000 // 5 minutes at 10Hz
#define DEG2RAD (PI/180.0)

// WGS84 parameters
#define WGS84_A 6378137.0
#define WGS84_F (1.0/298.257223563)
#define WGS84_B (WGS84_A*(1.0-WGS84_F))
#define WGS84_E2 (2.0*WGS84_F - WGS84_F*WGS84_F)

void llh2xyz(double lat, double lon, double h, double *x, double *y, double *z) 
{
    double phi = lat * DEG2RAD;
    double lambda = lon * DEG2RAD;
    double sinphi = sin(phi);
    double cosphi = cos(phi);
    double sinlambda = sin(lambda);
    double coslambda = cos(lambda);
    
    double N = WGS84_A / sqrt(1.0 - WGS84_E2*sinphi*sinphi);
    
    *x = (N + h) * cosphi * coslambda;
    *y = (N + h) * cosphi * sinlambda;
    *z = (N*(1.0 - WGS84_E2) + h) * sinphi;
}

int main(void)
{
    FILE *fp;
    double t, angle;
    double x0, y0, z0; // Center point ECEF coordinates
    double x, y, z;    // Current point ECEF coordinates
    double lat, lon;
    
    // Open output file
    fp = fopen("circle_l1c.csv", "w");
    if (fp == NULL) {
        printf("Failed to open output file.\n");
        return 1;
    }
    
    // Convert center point to ECEF
    llh2xyz(CENTER_LAT, CENTER_LON, HEIGHT, &x0, &y0, &z0);
    
    // Generate circle points
    for (int i = 0; i < NUM_POINTS; i++) {
        t = 0.1 * i;  // Time at 10Hz
        angle = 2.0 * PI * t / 60.0; // Complete circle every 60 seconds
        
        // Calculate local ENU offsets
        double east = RADIUS * cos(angle);
        double north = RADIUS * sin(angle);
        
        // Convert to lat/lon changes
        double dlat = north / (WGS84_A * DEG2RAD);
        double dlon = east / (WGS84_A * cos(CENTER_LAT * DEG2RAD) * DEG2RAD);
        
        lat = CENTER_LAT + dlat;
        lon = CENTER_LON + dlon;
        
        // Convert to ECEF
        llh2xyz(lat, lon, HEIGHT, &x, &y, &z);
        
        // Write to file: time,x,y,z
        fprintf(fp, "%.1f,%.3f,%.3f,%.3f\n", t, x, y, z);
    }
    
    fclose(fp);
    printf("Generated circular motion data for L1C simulation: circle_l1c.csv\n");
    printf("Duration: %.1f seconds\n", (NUM_POINTS-1)*0.1);
    printf("Sample rate: 10 Hz\n");
    printf("Circle radius: %.1f meters\n", RADIUS);
    
    return 0;
}
