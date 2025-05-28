package main

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"strconv"

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
			<option value="0">Snail2Camel</option>
			<option value="1">Camel2Snail</option>
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

			option := r.FormValue("option")
			message := r.FormValue("message")

			opt, err := strconv.Atoi(option)
			if err != nil {
				log.Fatalf("Err %v", err)
				http.Error(w, "error", http.StatusInternalServerError)
				return
			}

			log.Printf("Received:\nOption: %s\nMessage: %s", option, message)

			msg, err := naming_client.Conver(opt, message)
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

	log.Fatal(http.ListenAndServe(":8080", nil))
}
