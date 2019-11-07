#pragma once
#include <mongoc/mongoc.h>
#include <json.hpp>
#include <iostream>

namespace mongo {
	using json = nlohmann::json;
	using Document = nlohmann::json;

	static bson_t* bson_from_json(json data) {
		bson_error_t error;
		return bson_new_from_json((const uint8_t*) data.dump().c_str(), -1, &error);
	}

	class Collection {
	public:
		Collection(mongoc_client_t* client, const char* db, const char* name) {
			this->client = client;
			this->collection = mongoc_client_get_collection(client, db, name);
			this->name = name;
		}

		void insertOne(Document document) {
			bson_error_t error;
			bson_t* doc = bson_new_from_json((const uint8_t*) document.dump().c_str(), -1, &error);
			mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
			bson_destroy(doc);
		}

		void insertMany(std::vector<Document> docs) {
			for (auto document : docs) {
				insertOne(document);
			}
		}

		std::vector<Document> find(Document filter, json opts = {}) {
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

		Document findOne(Document filter) {
			bson_t* f = bson_from_json(filter);

			auto cursor = mongoc_collection_find_with_opts(collection, f, NULL, NULL);

			std::vector<Document> documents; const bson_t* doc;
			if (!mongoc_cursor_next(cursor, &doc)) return NULL;
			return Document::parse(bson_as_canonical_extended_json(doc, NULL));
		}

		int estimate_count() {
			bson_error_t error;
			return static_cast<int>(mongoc_collection_estimated_document_count(collection, NULL, NULL, NULL, &error));
		}

		int count(Document filter = {}) {
			bson_error_t error;
			return static_cast<int>(
				mongoc_collection_count_documents(
					collection, bson_from_json(filter), NULL, NULL, NULL, &error
				)
			);
		}

		void deleteMany(Document filter) {
			bson_error_t error;
			mongoc_collection_delete_many(collection, bson_from_json(filter), NULL, NULL, &error);
		}

		void deleteOne(Document filter) {
			bson_error_t error;
			mongoc_collection_delete_one(collection, bson_from_json(filter), NULL, NULL, &error);
		}

		void drop() {
			bson_error_t error;
			mongoc_collection_drop(collection, &error);
		}

		void rename(const char* newdb, const char* newcoll, bool dropBeforeRename) {
			bson_error_t error;
			mongoc_collection_rename(collection, newdb, newcoll, dropBeforeRename, &error);
		}

		void replaceOne(Document old, Document _new) {
			bson_error_t error;
			mongoc_collection_replace_one(collection, bson_from_json(old), bson_from_json(_new), NULL, NULL, &error);
		}

		void updateMany(Document filter, Document update) {
			bson_error_t error;
			mongoc_collection_update_many(collection, bson_from_json(filter), bson_from_json(update), NULL, NULL, &error);
		}

		void updateOne(Document filter, Document update) {
			bson_error_t error;
			mongoc_collection_update_one(collection, bson_from_json(filter), bson_from_json(update), NULL, NULL, &error);
		}

		~Collection() {
			mongoc_collection_destroy(collection);
		}

		const char* name;
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
			return Collection{ this->client, this->name, name };
		}

		const char* name;
	private:
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
			return Database{ this->client, name };
		}

		~Client() {
			mongoc_client_destroy(client);
			mongoc_cleanup();
		}

	private:
		mongoc_client_t* client;
	};
}