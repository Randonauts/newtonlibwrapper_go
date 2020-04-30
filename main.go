package main

// #cgo CPPFLAGS: -g -Wall
// #cgo LDFLAGS: -lc++ -L. -lAttract
// #include "export_h.h"
import "C"
import (
	"encoding/json"
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
	pathParams := mux.Vars(r)
	w.Header().Set("Content-Type", "application/json")

	// TODO: code cleanup with a generic input/args/params checking function

	apiVersion := -1
	var err error
	if val, ok := pathParams["apiVersion"]; ok {
		apiVersion, err = strconv.Atoi(val)
		if apiVersion < 0 || apiVersion > 0 {
			w.WriteHeader(http.StatusBadRequest)
			w.Write([]byte(`{"error": "Unsupported API version"}`))
			return
		}
	}

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
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "gid must be a positive number"}`))
		return
	}

	entropy, err := ioutil.ReadAll(r.Body)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(`{"error": "Cannot read entropy payload"}`))
		return
	}

	var dots C.ulong = C.getOptimizedDots(C.double(radius))
	var neededEntropySize C.ulong = C.requiredEntropyBytes(dots)
	if len(entropy) < int(neededEntropySize) {
		w.WriteHeader(http.StatusBadRequest)
		w.Write([]byte(fmt.Sprintf(`{"error": "Entropy payload is too small; %d bytes needed"}`, neededEntropySize)))
		return
	}

	var location C.LatLng
	location.latitude = C.double(latitude)
	location.longitude = C.double(longitude)

	var al C.ulong = 0
	var idaPtr *C.FinalAttractorNLD_go
	newtonEngine := C.initWithBytes(C.getHandle(), (*C.uchar)(unsafe.Pointer(&entropy[0])), neededEntropySize)
	defer C.releaseEngine(newtonEngine)

	C.findAttractors(newtonEngine, 2.5 /*==significance*/, 4.0 /*==filtering_significance*/)
	al = C.getAttractorsLength(newtonEngine)
	idaPtr = C.getAttractorsNLD_go(newtonEngine, C.double(radius), location, C.ulonglong(gid))

	if al > 0 {
		ida := (*[1 << 2]C.struct_FinalAttractorNLD_go)(unsafe.Pointer(idaPtr))[:al:al]

		var jsonData []byte
		jsonData, err := json.Marshal(ida)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			w.Write([]byte(fmt.Sprintf(`{"error": "Error marshalling the response - %s"}`, err.Error())))
			return
		}

		w.WriteHeader(http.StatusOK)
		w.Write([]byte(fmt.Sprintf(`{"points": %s}`, string(jsonData))))
	} else {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte(fmt.Sprintf(`{"points": []}`)))
	}
}

func main() {
	port := "3333"
	if len(os.Args) == 2 {
		port = os.Args[1]
	}

	r := mux.NewRouter()
	apiv0 := r.PathPrefix("/lib/newton/v{apiVersion}").Subrouter()
	apiv0.HandleFunc("/attractors", attractors).Methods(http.MethodPost)

	bindHostPort := fmt.Sprintf(":%s", port)
	fmt.Printf("newtonlib wrapper listening on %s\n", bindHostPort)
	log.Fatal(http.ListenAndServe(bindHostPort, r))
}
