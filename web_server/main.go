package main

import (
	"context"
	"errors"
	"fmt"
	"html/template"
	"log"
	"net/http"
	"strconv"

	"github.com/golang/groupcache"

	"github.com/lemito/namingify/naming_client"
)

var formTemplate = `
<!DOCTYPE html>
<html>
<head>
	<title>UwU</title>
</head>
<body>
	<h1>Meow</h1>
	<form method="POST" action="/convert">
		<label for="option">Method:</label>
		<select name="option" id="option">
			<option value="0">0</option>
			<option value="1">1</option>
			<option value="2">2</option>
			<option value="3">3</option>
		</select>
		<br><br>
		<label for="message">Lex:</label>
		<input type="text" id="message" name="message" required>
		<br><br>
		<input type="submit" value="Send">
	</form>
</body>
</html>
`

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

func main() {

	http.HandleFunc("/convert", func(w http.ResponseWriter, r *http.Request) {
		switch r.Method {
		case http.MethodGet:
			tmpl, err := template.New("form").Parse(formTemplate)
			if err != nil {
				http.Error(w, "error", http.StatusInternalServerError)
				return
			}

			tmpl.Execute(w, nil)
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

	})

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
