#ifndef EXPORT_H
#define EXPORT_H
#pragma once

//This is main C export header export_h.h for libAttract dynamic library 

// Define EXPORTED for any platform
#if defined _WIN32 && !defined TESTAPP
# ifdef LIBATTRACT_EXPORTS
#   define EXPORTED  __declspec( dllexport )
# else
#   define EXPORTED  __declspec( dllimport )
# endif
#else
#	define EXPORTED
#endif

#include <inttypes.h> 
#include <stdio.h>
#include <stdlib.h>


//  Common structures
//	namespaces stripped for C compatibility

//  Geolocation structures
typedef enum CARDINAL_DIRECTION
{
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW
} CARDINAL_DIRECTION;

typedef struct LatLng {
	double latitude;
	double longitude;
} LatLng;

typedef struct LatLng_go { // for go, by soliax
	double Latitude;
	double Longitude;
} LatLng_go;

typedef struct DistanceBearring {
	double distance;
	double initialBearing;
	double finalBearing;
} DistanceBearring;

typedef struct DistanceBearing_go { // for go, by soliax
	double Distance;
	double InitialBearing;
	double FinalBearing;
} DistanceBearing_go;

typedef struct Coordinate {
	LatLng point;
	DistanceBearring bearing; //how it was calculated
} Coordinate;

typedef struct Coordinate_go { // for go, by soliax
	LatLng_go Point;
	DistanceBearing_go Bearing; //how it was calculated
} Coordinate_go;

const LatLng Moscow = { 55.751244 , 37.618423 };
const LatLng Paris = { 48.864716 , 2.349014 };
const LatLng London = { 51.509865 , -0.118092 };
const LatLng Fukuoka = { 33.5977505, 130.409509 };

//in c const is not what it seems, so:
#define COORD_ENTROPY_BITS_MAX 52
#define COORD_ENTROPY_BITS_ECO 32

//why 13 if 2xDouble is 16 bytes?
//https://unix4lyfe.org/random-double/

// double random number in the range[1, 2) needs
// 0x3FFn nnnn nnnn nnnn enthropy
// Where the ns are random hex digits.
// 13 bytes of entropy or 26 hex digits are required for a random point: doubl X + double Y
	
static const unsigned int POINT_ENTROPY_BYTES_MAX = (unsigned int)(COORD_ENTROPY_BITS_MAX*2/8);
static const unsigned int POINT_ENTROPY_HEX_MAX = (unsigned int)(COORD_ENTROPY_BITS_MAX*4/8); //string hex representation

// new defaults (economy!):
static const unsigned int POINT_ENTROPY_BYTES_ECO = (unsigned int)(COORD_ENTROPY_BITS_ECO*2/8);
static const unsigned int POINT_ENTROPY_HEX_ECO = (unsigned int)(COORD_ENTROPY_BITS_ECO*4/8); //string hex representation


static const unsigned int RANDOM_SEED = 23;

//https://en.wikipedia.org/wiki/Standard_score
//Z-score or Standart Score is an adaptation of standart deviations used to assess normally distributed data to a different probability distribution, in our case Poisson.

//From the rules for normally distributed data for a daily event:
//μ ± 0.5σ 	0.382924922548026 	2 in 3 	Four or five times a week
//μ ± σ 	0.682689492137086 	1 in 3 	Twice a week
//μ ± 1.5σ 	0.866385597462284 	1 in 7 	Weekly
//μ ± 2σ 	0.954499736103642 	1 in 22 	Every three weeks
//μ ± 2.5σ 	0.987580669348448 	1 in 81 	Quarterly
//μ ± 3σ 	0.997300203936740 	1 in 370 	Yearly
//μ ± 3.5σ 	0.999534741841929 	1 in 2149 	Every six years
//μ ± 4σ 	0.999936657516334 	1 in 15787 	Every 43 years (twice in a lifetime)
//μ ± 4.5σ 	0.999993204653751 	1 in 147160 	Every 403 years (once in the modern era)
//μ ± 5σ 	0.999999426696856 	1 in 1744278 	Every 4776 years (once in recorded history)
//μ ± 5.5σ 	0.999999962020875 	1 in 26330254 	Every 72090 years (thrice in history of modern humankind)
//μ ± 6σ 	0.999999998026825 	1 in 506797346 	Every 1.38 million years (twice in history of humankind)
//μ ± 6.5σ 	0.999999999919680 	1 in 12450197393 	Every 34 million years (twice since the extinction of dinosaurs)
//μ ± 7σ 	0.999999999997440 	1 in 390682215445 	Every 1.07 billion years (four times in history of Earth)

const unsigned long MIN_DOTS_IN_RADIUS = 10; //used for oldstyle power calculation

const double SIGNIFICANCE = 2.5; //significance absolute threshold for calculation, recommended value is in [2.5, 3.0]. Higher value speeds up the calculation resulting in less findings.
const double FILTERING_SIGNIFICANCE = 4.0; //significance absolute threshold for filtering of results, recommended value is 4.0 or higher, this usualy produces 0..10 results 

typedef enum SpotType {
	UNDEF,
	ATTRACTOR,
	REPELLER,
	FAKE
} SpotType;

typedef enum Rarity {
	JUNK, //black			//0.0
	POOR, //gray			//3.0
	COMMON, //white			//3.5
	UNCOMMON, //green		//4.0
	RARE, //blue			//4.5
	EPIC, //purple			//5.0
	LEGENDARY, //orange		//5.5 
	RELIC, // light gold	//6.0
	JOKER, //rainbow		//6.5
	SINGULARITY //octarine	//7.0
} Rarity;

typedef struct FinalAttractor { //struct, export variant
	// for speed sake all values are calculated for flat model, neglecting curvature
	// WGS coordinates are then applied to attractor coordinates, pertaining distances and bearing to central coordinate. 

	unsigned long long GID; //global id, defined by user (for databases)
	unsigned long long TID; //timestamp id
	unsigned long long LID; //local id, i.e number in array

	SpotType type;

	double x;
	double y;

	Coordinate center; //center of attractor peak
	CARDINAL_DIRECTION side;

	double distanceErr; //calculation error due to neglected curvature etc
	double radiusM; //radius of attractor peak

	unsigned long long n; //number of points
	double mean; //mean average

	Rarity rarity; //TODO: significance simplified ;)

	double power_old; //oldstyle power - area taken by 10 central points of attractor/mean value for that area		
	double power; //area taken by all points of attractor/mean value for the area

	double z_score; // poisson z-score of single random event, if we chose this area on random and got this distribution
	long double probability_single; //exact probability of the above event being random.

	double integral_score; //abstract value of integral significance - how z-score varies with growth of radius, along with power characterises the density of condentsation/rarefaction

	double significance; // poisson z-score of entire random event, i.e how possible is, that the event of finding this one attractor after the whole calculation was random.
	long double probability; //exact probability of the above event being random.
} FinalAttractor;

typedef struct FinalAttractorNLD_go { //struct, export variant - FinalAttractor with No Long Double for use with go wrapper & capitalize member names so we can marshal to Json (soliax)
	// for speed sake all values are calculated for flat model, neglecting curvature
	// WGS coordinates are then applied to attractor coordinates, pertaining distances and bearing to central coordinate. 

	unsigned long long GID; //global id, defined by user (for databases)
	unsigned long long TID; //timestamp id
	unsigned long long LID; //local id, i.e number in array

	SpotType Type;

	double X;
	double Y;

	Coordinate_go Center; //center of attractor peak
	CARDINAL_DIRECTION Side;

	double DistanceErr; //calculation error due to neglected curvature etc
	double RadiusM; //radius of attractor peak

	unsigned long long N; //number of points
	double Mean; //mean average

	Rarity Rarity; //TODO: significance simplified ;)

	double Power_old; //oldstyle power - area taken by 10 central points of attractor/mean value for that area		
	double Power; //area taken by all points of attractor/mean value for the area

	double Z_score; // poisson z-score of single random event, if we chose this area on random and got this distribution
	double Probability_single; //exact probability of the above event being random.

	double Integral_score; //abstract value of integral significance - how z-score varies with growth of radius, along with power characterises the density of condentsation/rarefaction

	double Significance; // poisson z-score of entire random event, i.e how possible is, that the event of finding this one attractor after the whole calculation was random.
	double Probability; //exact probability of the above event being random.
} FinalAttractorNLD_go;

// FinalAttractor -> FinalAttractorNLD_go (No Long Double) - for go, by soliax
FinalAttractorNLD_go f2nld(FinalAttractor *f) {
	FinalAttractorNLD_go ret;

	// printf("GID: %llu\n", f->GID);
	ret.GID = f->GID;

	// printf("TID: %llu\n", f->TID);
	ret.TID = f->TID;

	// printf("LID: %llu\n", f->LID);
	ret.LID = f->LID;

	// printf("Type: %u\n", f->type);
	ret.Type = f->type;

	// printf("X: %f\n", f->x);
	ret.X = f->x;

	// printf("Y: %f\n", f->y);
	ret.Y = f->y;

	// printf("Latitude: %f\n", f->center.point.latitude);
	ret.Center.Point.Latitude = f->center.point.latitude;

	// printf("Longitude: %f\n", f->center.point.longitude);
	ret.Center.Point.Longitude = f->center.point.longitude;

	// printf("Distance: %f\n", f->center.bearing.distance);
	ret.Center.Bearing.Distance = f->center.bearing.distance;

	// printf("InitialBearing: %f\n", f->center.bearing.initialBearing);
	ret.Center.Bearing.InitialBearing = f->center.bearing.initialBearing;
	
	// printf("FinalBearing: %f\n", f->center.bearing.finalBearing);
	ret.Center.Bearing.FinalBearing = f->center.bearing.finalBearing;
	
	// printf("Side: %u\n", f->side);
	ret.Side = f->side;
	
	// printf("DistanceErr: %f\n", f->distanceErr);
	ret.DistanceErr = f->distanceErr;
	
	// printf("RadiusM: %f\n", f->radiusM);
	ret.RadiusM = f->radiusM;
	
	// printf("N: %llu\n", f->n);
	ret.N = f->n;
	
	// printf("Mean: %f\n", f->mean);
	ret.Mean = f->mean;
	
	// printf("Rarity: %d\n", f->rarity);
	ret.Rarity = f->rarity;
	
	// printf("Power_old: %f\n", f->power_old);
	ret.Power_old = f->power_old;
	
	// printf("Power: %f\n", f->power);
	ret.Power = f->power;
	
	// printf("Z_score: %f\n", f->z_score);
	ret.Z_score = f->z_score;
	
	// printf("Probability_single: %Lf\n", f->probability_single);
	ret.Probability_single = f->probability_single;
	
	// printf("Integral_score: %f\n", f->integral_score);
	ret.Integral_score = f->integral_score;
	
	// printf("Significance: %f\n", f->significance);
	ret.Significance = f->significance;

	// printf("Probability: %Lf\n", f->probability);
	ret.Probability = f->probability;

	return ret;
}

	//c++20 has compatibility tho
#ifndef __cplusplus
	FinalAttractor FinalAttractor_default = {
		0,
		0,
		0,
		0,
		.0,
		.0,
		{ {.0,.0},
		  {.0,.0,.0}
		},
		0,
		.0,
		.0,
		0,
		.0,
		0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0,
		.0
	};
#endif 

#ifdef __cplusplus
	extern "C" {
#endif 

	//get library version, just returns values from the corresponding header (for C# compatibility etc)
	EXPORTED unsigned long getVersionMajor();
	EXPORTED unsigned long getVersionMinor();
	EXPORTED unsigned long getVersionPatch();

	// First: decide how many points are used for calculation
	// spot radius is expected minimum attractor radius, usualy the are bigger
	EXPORTED unsigned long getOptimizedDots(const double areaRadiusM); //how many coordinates is needed for requested radius, optimized for performance on larger areas
	EXPORTED unsigned long getDotsBySpotRadius(const double spotRadiusM, const double areaRadiusM); //how many dots are needed for the chosen spot size
	EXPORTED double getSpotSize(const unsigned long N, const double areaRadiusM); //reverse problem: what is the expected minimum attractor radius given number of dots and area

	//Second: obtain the necessary amounts of enthropy from RNG, either as raw bytes, or as hex string
	unsigned long requiredEnthropyBytes(const unsigned long N); // pas d'exposé of H
	unsigned long requiredEnthropyHex(const unsigned long N);	// ^ " ditto
	EXPORTED unsigned long requiredEntropyBytes(const unsigned long N) { return requiredEnthropyBytes(N); }; // N* POINT_ENTROPY_BYTES 
	EXPORTED unsigned long requiredEntropyHex(const unsigned long N) { return requiredEnthropyHex(N); }; // N* POINT_ENTROPY_HEX 

	//Third: init engine, the number returned is engine index;

	//for testing init with pseudo circular coords in [0..1):
	EXPORTED unsigned long getHandle();
	EXPORTED unsigned long initWithPseudo(unsigned long handle, const unsigned long N, unsigned int seed);

	//for production use 
	EXPORTED unsigned long initWithCoords(unsigned long handle, const double coords[], const unsigned long N); //N is number of 2D coordinates expects 2*N coords input : x,y,x1,y1...
	EXPORTED unsigned long initWithBytes(unsigned long handle, const unsigned char byteinput[], unsigned long bytelength);
	EXPORTED unsigned long initWithHex(unsigned long handle, const char hexstring[], unsigned long stringlength);

	//check number of coords obtained
	EXPORTED unsigned long getEngineCoordsLength(const unsigned long engineInstance);
	
	//Fourth: 

	//long long the object and do the calculation, returns engine instance
	EXPORTED unsigned long findAttractors(const unsigned long engineInstance, const double significance, const double filtering_significance);

	//apply geometry fixes and get result
	EXPORTED unsigned long getAttractorsLength(const unsigned long engineInstance);
	EXPORTED FinalAttractor* getAttractors(const unsigned long engineInstance, const double radiusM, const LatLng center, const unsigned long long gid);
	EXPORTED FinalAttractorNLD_go* getAttractorsNLD_go(const unsigned long engineInstance, const double radiusM, const LatLng center, const unsigned long long gid) {
		unsigned long al = getAttractorsLength(engineInstance);
		FinalAttractor* fa = getAttractors(engineInstance, radiusM, center, gid);

		FinalAttractorNLD_go *fanlds = (FinalAttractorNLD_go*) malloc(sizeof(FinalAttractorNLD_go) * al);
		for (int i = 0; i < al; i++) {
			fanlds[i] = f2nld(&fa[i]);
		}

		return fanlds;
	}
	// [Removed] free memory allocated for array:
	// EXPORTED void releaseAttractors(FinalAttractor* results);
	// replaced by releaseEngine

	//apply geometry fixes and get all coords (optional):
	EXPORTED unsigned long getFinalCoordsLength(const unsigned long engineInstance);
	EXPORTED Coordinate* getFinalCoords(const unsigned long engineInstance, const double radiusM, const LatLng center);
	
	// [Removed] free memory allocated for array:
	// EXPORTED void releaseCoords(Coordinate* results);
	// replaced by releaseEngine

	/* *********************** */

	//apply geometry fixes and get blindfold result
	// std::vector<FinalAttractor> getBlind(const double radiusM, const LatLng center, const unsigned long long gid); //TODO

	/* *********************** */

	//simple print:
	EXPORTED void printAttractor(const FinalAttractor result);
	EXPORTED void printAttractors(FinalAttractor results[], unsigned long length);
	EXPORTED void printCoords(Coordinate results[], unsigned long length);


	//free allocated resources
	EXPORTED void releaseEngine(const unsigned long engineInstance); // release single instance
	EXPORTED void releaseEngine_go(const unsigned long engineInstance, FinalAttractorNLD_go *fanlds) { // release single instance
		free(fanlds);
		releaseEngine(engineInstance);
	}
	EXPORTED void releaseHandle(const unsigned long handle); 	// this frees all engines tied to handle
	EXPORTED void finalize(); 	// !!!CAUTION!!! this frees all engines systemwide, call before unloading the dll
	//хардресет
	
#ifdef __cplusplus  
	} // extern "C"  
#endif

#endif
