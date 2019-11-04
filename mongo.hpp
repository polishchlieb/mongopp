#pragma once
#include <mongoc/mongoc.h>
#include <json.hpp>

namespace mongo {
	using json = nlohmann::json;
	using Document = nlohmann::json;

	bson_t* bson_from_json(json data) {
		bson_error_t error;
		return bson_new_from_json((const uint8_t*) data.dump().c_str(), -1, &error);
	}

	class Collection {
	public:
		Collection(mongoc_client_t* client, const char* db, const char* name) {
			this->collection = mongoc_client_get_collection(client, db, name);
			this->client = client;
		}

		void insertOne(Document document) {
			bson_error_t error;
			bson_t* doc = bson_new_from_json((const uint8_t*) document.dump().c_str(), -1, &error);
			mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
			bson_destroy(doc);
		}

		std::vector<Document> find(Document filter, json opts) {
			bson_error_t error;
			bson_t* f = bson_from_json(filter);
			bson_t* o = bson_from_json(opts);

			auto cursor = mongoc_collection_find_with_opts(collection, f, o, NULL);

			std::vector<Document> documents; const bson_t* doc;
			while (mongoc_cursor_next(cursor, &doc)) {
				documents.push_back(
					Document::parse(bson_as_canonical_extended_json(doc, NULL))
				);
			}

			bson_destroy(f);
			bson_destroy(o);
			return documents;
		}

		std::vector<Document> find(Document filter) {
			bson_error_t error;
			bson_t* f = bson_from_json(filter);

			auto cursor = mongoc_collection_find_with_opts(collection, f, NULL, NULL);

			std::vector<Document> documents; const bson_t* doc;
			while (mongoc_cursor_next(cursor, &doc)) {
				documents.push_back(
					Document::parse(bson_as_canonical_extended_json(doc, NULL))
				);
			}

			return documents;
		}

		Document findOne(Document filter) {
			bson_error_t error;
			bson_t* f = bson_from_json(filter);

			auto cursor = mongoc_collection_find_with_opts(collection, f, NULL, NULL);

			std::vector<Document> documents; const bson_t* doc;
			if (!mongoc_cursor_next(cursor, &doc)) return NULL;
			return Document::parse(bson_as_canonical_extended_json(doc, NULL));
		}

		~Collection() {
			mongoc_collection_destroy(collection);
		}

	private:
		mongoc_client_t* client;
		mongoc_collection_t* collection;
	};

	class Database {
	public:
		Database(mongoc_client_t* client, const char* name) {
			this->client = client;
			this->name = name;
		}

		Collection getCollection(const char* name) {
			Collection c(this->client, this->name, name);
			return c;
		}

	private:
		const char* name;
		mongoc_client_t* client;
	};

	class Client {
	public:
		Client(const char* uri) {
			mongoc_init();
			bson_error_t error;

			mongoc_uri_t* mUri = mongoc_uri_new_with_error(uri, &error);
			if (!mUri) {
				std::cout << "blyat: uri" << std::endl;
			}
			mongoc_client_t* client = mongoc_client_new_from_uri(mUri);
			if (!client) {
				std::cout << "blyat: client" << std::endl;
			}
			mongoc_client_set_appname(client, "mongo");

			this->client = client;
		}

		Database getDatabase(const char* name) {
			Database d(this->client, name);
			return d;
		}

		~Client() {
			mongoc_client_destroy(client);
			mongoc_cleanup();
		}

	private:
		mongoc_client_t* client;
	};
}