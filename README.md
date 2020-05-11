# mongo++
A tiny header-only wrapper around [mongo-c-driver](http://mongoc.org/) written in modern C++.

## Installation
- Download `json.hpp` file from [here](https://github.com/nlohmann/json/releases) and add it to the include path
- Install mongo-c-driver: follow the [instructions](http://mongoc.org/libmongoc/current/installing.html) and then link everything (another tutorial for Visual Studio users: [clicc](http://mongoc.org/libmongoc/current/visual-studio-guide.html))
- Download `mongo.hpp` and add it to the include path

## Usage
```cpp
#include <mongo.hpp>

int main() {
	mongo::Client client("mongo://stuff");
	auto users = client.getDatabase("app").getCollection("users");

	// Insert a document
	coll.insertOne({
		{"username", "towarzyszchlebek"},
		{"id", 1945},
		{"points", 0}
	});

	// Update the document
	coll.updateOne(
		{ {"id", 1945} },
		{ {"$inc", {
			{"points", 1}
		}} }
	);
}
```