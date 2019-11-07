# mongo++ documentation

**Note**: library uses const char* instead of std::string. To convert from std::string, use `str.c_str()`\
**yetanothernote:** `mongo::Document` is equivalent to `nlohmann::json`

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
mongo::Database db = client.getDatabase("name");
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
mongo::Collection coll = db.getCollection("name");
```

## mongo::Collection
### mongo::Collection::Collection
**Note**: you should use `mongo::Database::getCollection` instead probably
```cpp
mongo::Collection::Collection(mongoc_client_t* client, const char* db, const char* name);
```
### mongo::Collection::insertOne
```cpp
void mongo::Collection::insertOne(mongo::Document document);
```
Example:
```cpp
collection.insertOne({ { "hello", "world" } });
```

### mongo::Collection::find
**TODO**: Create `mongo::Cursor` instead of using `std::vector`. Performance reasons
```cpp
std::vector<mongo::Document> mongo::Collection::find(mongo::Document filter, nlohmann::json opts = {});
```
Example:
```cpp
auto docs = collection.find({ { "hello", "world" } });
for (auto document : docs) {
    std::cout << document.dump() << std::endl;
}
```

### mongo::Collection::findOne
Gets first document from the collection that satisfies given filter
```cpp
mongo::Document mongo::Collection::findOne(mongo::Document filter);
```
Example:
```cpp
// Finds a document and displays it
auto document = collection.findOne({ { "hello", "world" } });
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
```

### mongo::Collection::count
Gets accurate count of documents in the collection that satisfy given filter
```cpp
int mongo::Collection::count(mongo::Document filter = {});
```
Example:
```cpp
int count = collection.count();
```

### mongo::Collection::deleteMany
Deletes all the documents from the collection that satisfy given filter
```cpp
void mongo::Collection::deleteMany(mongo::Document filter);
```
Example:
```cpp
collection.deleteMany({ { "hello", "world" } });
```

### mongo::Collection::deleteOne
Deletes the first documents that satisfies given filter
```cpp
void mongo::Collection::deleteOne(mongo::Document filter);
```
Example:
```cpp
collection.deleteOne({ { "hello", "world" } });
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
collection.rename("database", "collection", false);
```

### mongo::Collection::replaceOne
Replaces a document in the collection
```cpp
void mongo::Collection::replaceOne(mongo::Document old, mongo::Document _new);
```
Example:
```cpp
collection.replaceOne(
    { { "hello", "world" } },
    { { "hell", "world" } }
);
```

### mongo::Collection::updateMany
Updates all the documents in the collection that satisfy given filter
```cpp
void mongo::Collection::updateMany(mongo::Document filter, mongo::Document update);
```

Example:
```cpp
collection.updateMany(
    { { "blyat", true } },
    { { "blyat", false } }
);
```

### mongo::Collection::updateOne
Updates the first document in the collection that satisfies given filter
```cpp
void mongo::Collection::updateOne(mongo::Document filter, mongo::Document update);
```

Example:
```cpp
collection.updateOne(
    { { "hello", "world" } },
    { { "pimposlaw", "doggo" } }
);
```

## mongo::bson_from_json
```cpp
static bson_t* bson_from_json(nlohmann::json data);
```