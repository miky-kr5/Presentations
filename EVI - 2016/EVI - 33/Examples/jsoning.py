import json

data = {"Name": "Miguel",
        "Age": 27,
        "ID": 18810993,
        "info" : {"Height": 1.72,
                  "Weight": 80
                  }
        }

with open("data.json", "w") as f:
    f.write(json.dumps(data))

with open("example.json") as f:
    j = json.loads(f.read())
    print(j["isAlive"])
