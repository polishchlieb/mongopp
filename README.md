### Warning: the lib is incomplete and unstable. Do not use it in production unless you know what you are doing.

# mongo++
Unofficial mongodb C++ library.

## Installation
Mongo++ is a header-only library. However, it's based on [nlohmann/json](https://github.com/nlohmann/json) and [mongo-c-driver](https://github.com/mongodb/mongo-c-driver).\
Installing the first one is just about downloading the `json.hpp` file from [here](https://github.com/nlohmann/json/releases) and including it in your project.\
With libmongoc, it's a little bit more complicated. You need to follow the [instructions](http://mongoc.org/libmongoc/current/installing.html) and then link everything (another tutorial for Visual Studio users: [clicc](http://mongoc.org/libmongoc/current/visual-studio-guide.html)).\
Then, linking mongo++ lib is the same as linking json.

## Usage
```cpp
#include <mongo.hpp>

int main() {
	mongo::Client client("url");
	mongo::Database db = client.getDatabase("db_name");
	mongo::Collection coll = db.getCollection("coll_name");
	coll.insertOne({ { "hello", "world" } });
}
```