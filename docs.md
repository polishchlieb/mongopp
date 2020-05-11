# mongo++ documentation

**Note**: library uses const char* instead of std::string. To convert from std::string, use `str.c_str()`\
**yetanothernote:** `mongo::Document` and `mongo::json` equivalent to `nlohmann::json`\
**crazy note vol. 3** this is not the MongoDB documentation, you can find it [here](https://docs.mongodb.com/manual/reference/) instead

## mongo::Client
### mongo::Client::Client
```cpp
mongo::Client::Client(const char* uri);
```
Example:
```cpp
mongo::Client client("uri");
```

### mongo::Client::getDatabase
```cpp
mongo::Database client::getDatabase(const char* name);
```
Example:
```cpp
auto db = client.getDatabase("app");
```

## mongo::Database
### mongo::Database::Database
**Note**: you should use `mongo::Client::getDatabase` instead probably
```cpp
mongo::Database db(mongoc_client_t* client, const char* name);
```

### mongo::Database::getCollection
```cpp
mongo::Collection db::getCollection(const char* name);
```
Example:
```cpp
auto users = db.getCollection("users");
```

## mongo::Collection
### mongo::Collection::Collection
**Note**: perhaps you should use `mongo::Database::getCollection` instead
```cpp
mongo::Collection::Collection(mongoc_client_t* client, const char* db, const char* name);
```
### mongo::Collection::insertOne
```cpp
void mongo::Collection::insertOne(const mongo::Document& document);
```
Example:
```cpp
collection.insertOne({
    {"username", "towarzyszchlebek"},
    {"github", "https://github.com/polishchlieb"}
});
```

### mongo::Collection::find
Returns a cursor to the documents in a collection that satisfy given filter
```cpp
mongo::Cursor mongo::Collection::find(const mongo::Document& filter, const mongo::json& opts = {});
```
Example:
```cpp
const auto ids = mongo::json::array({ 1, 2, 3 });
auto cursor = collection.find({ {"id", { {"$in", ids} }} });
```

### mongo::Collection::findOne
Gets first document from the collection that satisfies given filter
```cpp
mongo::Document mongo::Collection::findOne(const mongo::Document& filter);
```
Example:
```cpp
// Finds a document and displays it
auto document = collection.findOne({ {"username", "towarzyszchlebek"} });
std::cout << document.dump() << std::endl;
```

### mongo::Collection::estimateCount
Gets estimate count of documents in the collection based on the collection's metadata 
```cpp
int mongo::Collection::estimateCount();
```
Example:
```cpp
int count = collection.estimateCount();
std::cout << count << std::endl;
```

### mongo::Collection::count
Gets accurate count of documents that satisfy given filter
```cpp
int mongo::Collection::count(const mongo::Document& filter = {});
```
Example:
```cpp
int count = collection.count();
std::cout << count << std::endl;
```

### mongo::Collection::deleteMany
Deletes all documents that satisfy given filter from the collection
```cpp
void mongo::Collection::deleteMany(const mongo::Document& filter);
```
Example:
```cpp
const auto ids = mongo::json::array({ 1, 2, 3 });
collection.deleteMany({ {"id", { {"$in", ids} }} });
```

### mongo::Collection::deleteOne
Deletes the first document that satisfies given filter
```cpp
void mongo::Collection::deleteOne(const mongo::Document& filter);
```
Example:
```cpp
collection.deleteOne({ {"username", "towarzyszchlebek"} });
```

### mongo::Collection::drop
Drops the collection
```cpp
void mongo::Collection::drop();
```
Example:
```cpp
collection.drop();
```

### mongo::Collection::rename
Renames the collection
```cpp
void mongo::Collection::rename(const char* newdb, const char* newcoll, bool dropBeforeRename);
```
where: `newdb` - new collection's database name, `newcoll` - new name of the collection, `dropBeforeRename` drops the collection and creates its copy instead of just renaming it if set to true.\
Example:
```cpp
collection.rename("newapp", "users", false);
```

### mongo::Collection::replaceOne
Replaces a document in the collection
```cpp
void mongo::Collection::replaceOne(const mongo::Document& filter, const mongo::Document& newDocument);
```
Example:
```cpp
collection.replaceOne(
    { {"username", "towarzyszchlebek"} },
    {
        {"username", "polishchlieb"},
        {"id", -1}
    }
);
```

### mongo::Collection::updateMany
Updates all documents that satisfy given filter.
```cpp
void mongo::Collection::updateMany(const mongo::Document& filter, const mongo::Document& update);
```

Example:
```cpp
const auto ids = mongo::json::array({ 1, 2, 3 });
collection.updateMany(
    { {"id", { {"$in", ids} }} },
    { {"$inc", { {"points", 1} }} }
);
```

### mongo::Collection::updateOne
Updates the first document in the collection that satisfies given filter
```cpp
void mongo::Collection::updateOne(const mongo::Document& filter, const mongo::Document& update);
```

Example:
```cpp
collection.updateOne(
    { {"username", "towarzyszchlebek"} },
    { {"$inc", { {"points", 1} }} }
);
```

## mongo::Cursor
### mongo::Cursor::Cursor
**Note**: you should not initiate this class manually, use collection methods, e.g. `mongo::Collection::find` instead.
```cpp
mongo::Cursor::Cursor(mongoc_collection_t* collection, const mongo::Document& filter, const mongo::json& opts);
```

### mongo::Cursor::next
Retrieves next document from the cursor
```cpp
mongo::Document mongo::Cursor::next();
```

Example:
```cpp
auto cursor = collection.find({ {"year", "1991"} });
// Print the first element from the cursor
std::cout << cursor.next().dump() << std::endl;

// Print next elements using the while loop
mongo::Document current;
while ((current = cursor.next()) != NULL) {
    std::cout << cursor.next().dump() << std::endl;
}
```

### mongo::Cursor::all
Retrieves all documents from the cursor
```cpp
std::vector<mongo::Document> all();
```

Example:
```cpp
auto cursor = collection.find({ {"year", "1991"} });
auto elements = cursor.all();

for (const auto& element : elements) {
    std::cout << element.dump() << std::endl;
}
```