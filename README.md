# mooshak-cli

A command-line interface for [Mooshak](https://mooshak.dcc.fc.up.pt/)

## Build

Just
`make`

Only libcurl required

## Run

`./mooshak-cli/mooshak-cli`

### Options

 - -c: Configuration generation form (asks for host, user, password, etc)

## API-ish

Sort of adhoc-documenting the mooshak endpoints and parameters

```
get_endpoint: GET /~mooshak/cgi-bin/execute -> 302 for endpoint in Location header i.e. /~mooshak/cgi-bin/execute/1604585587794234
login:        POST ${endpoint}?command=login&arguments=&contest=AED1_25_Practica&user=B117&password=******
get_ranking:  GET ${endpoint}?all_problems=true&page=0&problem=P003&type=ranking&all_teams=true&lines=200&time=5&command=listing
get_listing:  GET ${endpoint}?all_problems=true&page=0&problem=P003&type=submissions&all_teams=true&lines=200&time=5&command=listing
submit:       POST ${endpoint}
              MIME multipart (Content-Disposition: form-data;) {
                # name: content
                "command": "analyze",
                "problem": "P003",
                {
                    name: "program",
                    filename: "main.cpp",
                    Content-Type: text/x-c
                },
                "analyze": "Submit"
              }
```
