package main

// #cgo CPPFLAGS: -g -Wall
// #cgo LDFLAGS: -lc++ -L. -lAttract
// #include "export_h.h"
import "C"
import (
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strconv"
	"unsafe"

	"github.com/gorilla/mux"
)

func attractors(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")

	var err error
	query := r.URL.Query()

	radius := -1.0
	val := query.Get("radius")
	radius, err = strconv.ParseFloat(val, 64)
	if radius <= 0 || err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "radius must me a positive number(Float64)"}`))
		return
	}

	latitude := -1.0
	val = query.Get("latitude")
	latitude, err = strconv.ParseFloat(val, 64)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "latitude must me a number(Float64)"}`))
		return
	}

	longitude := -1.0
	val = query.Get("longitude")
	longitude, err = strconv.ParseFloat(val, 64)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "longitude must me a number(Float64)"}`))
		return
	}

	gid, err := strconv.Atoi(query.Get("gid"))
	if gid < 1 || err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "gid must be a positive number"}`))
		return
	}

	entropy, err := ioutil.ReadAll(r.Body)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(fmt.Sprintf(`{"error": "Cannot read entropy payload - %s"}`, err.Error())))
		return
	}

	res, err := findAttractors(radius, latitude, longitude, gid, entropy)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		w.Write([]byte(err.Error()))
		return
	}

	w.WriteHeader(http.StatusOK)
	w.Write([]byte(*res))
}

func findAttractors(radius float64, latitude float64, longitude float64, gid int, entropy []byte) (*string, error) {
	var dots C.ulong = C.getOptimizedDots(C.double(radius))
	var neededEntropySize C.ulong = C.requiredEntropyBytes(dots)
	if len(entropy) < int(neededEntropySize) {
		return nil, fmt.Errorf(`{"error": "Entropy payload is too small; have %d bytes, need %d bytes"}`, len(entropy), neededEntropySize)
	}

	var location C.LatLng
	location.latitude = C.double(latitude)
	location.longitude = C.double(longitude)

	var al C.ulong = 0
	var idaPtr *C.FinalAttractorNLD_go
	newtonEngine := C.initWithBytes(C.getHandle(), (*C.uchar)(unsafe.Pointer(&entropy[0])), C.ulong(len(entropy)))
	defer C.releaseEngine_go(newtonEngine, idaPtr)

	C.findAttractors(newtonEngine, 2.5 /*==significance*/, 4.0 /*==filtering_significance*/)
	al = C.getAttractorsLength(newtonEngine)
	idaPtr = C.getAttractorsNLD_go(newtonEngine, C.double(radius), location, C.ulonglong(gid))

	if al > 0 {
		ida := (*[1 << 30]C.struct_FinalAttractorNLD_go)(unsafe.Pointer(idaPtr))[:al:al]

		var jsonData []byte
		jsonData, err := json.Marshal(ida)
		if err != nil {
			return nil, fmt.Errorf(`{"error": "Error marshalling the response to JSON - %s"}`, err.Error())
		}

		ret := fmt.Sprintf(`{"points": %s}`, string(jsonData))
		return &ret, nil
	}

	ret := fmt.Sprintf(`{"points": []}`)
	return &ret, nil
}

func main() {
	// define command line arguments
	var gid = flag.Int("gid", 23, "GID of entropy passed in via stdin")
	var longitude = flag.Float64("longitude", 0.0, "longitude where to base the search center (e.g. 130.0)")
	var latitude = flag.Float64("latitude", 0.0, "latitude where to base the search center (e.g. 33.0)")
	var radius = flag.Float64("radius", 3000.0, "radius in m within which to search")
	var port = flag.Int("port", 3333, "port for REST API HTTP server")
	var apiMode = flag.Bool("server", true, "true to run in REST API HTTP server mode, false to run from CLI")
	flag.Parse()

	if *apiMode {
		// run an HTTP server that responds to the /attractors REST API
		r := mux.NewRouter()
		api := r.PathPrefix("").Subrouter()
		api.HandleFunc("/attractors", attractors).Methods(http.MethodPost)

		portStr := fmt.Sprintf(":%d", *port)
		fmt.Printf("newtonlib wrapper will listen on %s\n", portStr)
		err := http.ListenAndServe(portStr, r)
		if err != nil {
			log.Fatal(err)
		}
	} else {
		// do the same as the /attractors API but as a command line program with command line arguments and entropy as stdin
		// TODO: no args checking

		entropy, err := ioutil.ReadAll(os.Stdin)
		if err != nil {
			fmt.Println(fmt.Sprintf(`{"error": "Cannot read entropy from stdin pipe - %s"}`, err.Error()))
			return
		}

		res, err := findAttractors(*radius, *latitude, *longitude, *gid, entropy)
		if err != nil {
			fmt.Println(err.Error())
			return
		}

		fmt.Printf("%s", *res)
	}
}
