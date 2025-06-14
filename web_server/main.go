package main

import (
	"context"
	"embed"
	"errors"
	"fmt"
	"html/template"
	"log"
	"net/http"
	"strconv"

	"github.com/golang/groupcache"

	"github.com/lemito/namingify/naming_client"
)

//go:embed templates
var templateFS embed.FS

var formTemplate *template.Template

func init() {
	tpl, err := template.ParseFS(templateFS, "templates/*.html")
	if err != nil {
		log.Fatalf("Failed to parse templates: %v", err)
	}
	formTemplate = tpl.Lookup("index.html")
	if formTemplate == nil {
		log.Fatal("Form template not found")
	}
}

var fake_db = map[string][]byte{
	"cat":   []byte("meow"),
	"dog":   []byte("woof"),
	"panda": []byte("undefined"),
}

var Group = groupcache.NewGroup("sounds", 64<<20, groupcache.GetterFunc(func(ctx context.Context, key string, dest groupcache.Sink) error {
	log.Println("Cache not found. Work with DB started")
	val, err := fake_db[key]
	if !err {
		return errors.New("no such sound")

	}
	dest.SetBytes(val)
	return nil
}))

func convertHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case http.MethodGet:
		if err := formTemplate.Execute(w, nil); err != nil {
			http.Error(w, "Template error", http.StatusInternalServerError)
			log.Printf("Template error: %v", err)
		}
	case http.MethodPost:
		err := r.ParseForm()
		if err != nil {
			http.Error(w, "Failed to parse form", http.StatusBadRequest)
			return
		}

		message := r.FormValue("message")
		opt_string := r.FormValue("option")

		opt, err := strconv.Atoi(opt_string)
		if err != nil {
			log.Fatalf("Err %v", err)
			http.Error(w, "error", http.StatusInternalServerError)
			return
		}

		log.Printf("Received:\n Option %d \n Message: %s", opt, message)

		msg, err := naming_client.Convert(message, opt)
		if err != nil {
			log.Fatalf("Err %v", err)
			http.Error(w, "error", http.StatusInternalServerError)
			return
		}

		_, err = fmt.Fprint(w, msg)
		if err != nil {
			log.Fatalf("Err %v", err)
			http.Error(w, "error", http.StatusInternalServerError)
			return
		}
	default:
		http.Error(w, "No such method", http.StatusMethodNotAllowed)
	}

}

func main() {

	http.HandleFunc("/convert", convertHandler)

	http.HandleFunc("/sound", func(w http.ResponseWriter, r *http.Request) {
		animal := r.URL.Query().Get("animal")
		var res []byte

		err := Group.Get(context.Background(), animal, groupcache.AllocatingByteSliceSink(&res))
		if err != nil {
			http.Error(w, err.Error(), http.StatusNotFound)
			return
		}
		w.Write(res)
	})

	log.Fatal(http.ListenAndServe(":8080", nil))
}
