#pragma once
#include <mongoc/mongoc.h>
#include <json.hpp>
#include <iostream>

namespace mongo {
	using json = nlohmann::json;
	typedef json Document;

	class Cursor {
	public:
		Cursor(mongoc_collection_t* collection, const Document& _filter, const json& _opts) {
			bson_error_t error;
			filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			opts = bson_new_from_json((const uint8_t*)_opts.dump().c_str(), -1, &error);
			cursor = mongoc_collection_find_with_opts(collection, filter, opts, NULL);
		}
		Document next() {
			const bson_t* doc;
			if (!mongoc_cursor_next(cursor, &doc)) return NULL;
			return Document::parse(bson_as_json(doc, NULL));
		}
		std::vector<Document> all() {
			std::vector<Document> result;
			const bson_t* doc;
			while (mongoc_cursor_next(cursor, &doc)) {
				result.push_back(Document::parse(bson_as_json(doc, NULL)));
			}
			return result;
		}
		~Cursor() {
			mongoc_cursor_destroy(cursor);
			bson_free(filter);
			bson_free(opts);
		}
	private:
		mongoc_cursor_t* cursor;
		bson_t* filter, * opts;
	};
	
	class Collection {
	public:
		Collection(mongoc_client_t* client, const char* dbname, const char* name) {
			this->client = client;
			this->collection = mongoc_client_get_collection(client, dbname, name);
			this->name = name;
		}
		void insertOne(const Document& document) {
			bson_error_t error;
			bson_t* doc = bson_new_from_json((const uint8_t*)document.dump().c_str(), -1, &error);
			mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
			bson_destroy(doc);
		}
		void insertMany(const std::vector<Document>& docs) {
			bson_error_t error;
			const bson_t** documents{};
			const unsigned int size = docs.size();
			for (unsigned int i = 0; i < size; i++) {
				documents[i] = bson_new_from_json((const uint8_t*)docs[i].dump().c_str(), -1, &error);
			}
			mongoc_collection_insert_many(collection, documents, size, NULL, NULL, &error);
			for (unsigned int i = 0; i < size; i++) {
				bson_free((bson_t*)documents[i]);
			}
			bson_free(documents);
		}
		Cursor find(const Document& filter, const json& opts = {}) {
			return Cursor(this->collection, filter, opts);
		}
		Document findOne(const Document& filter) {
			bson_error_t error;
			bson_t* f = bson_new_from_json((const uint8_t*)filter.dump().c_str(), -1, &error);

			auto cursor = mongoc_collection_find_with_opts(collection, f, NULL, NULL);

			std::vector<Document> documents;
			const bson_t* doc;
			if (!mongoc_cursor_next(cursor, &doc)) return NULL;
			const auto result = Document::parse(bson_as_json(doc, NULL));

			bson_free(f);

			return result;
		}
		int estimate_count() {
			bson_error_t error;
			return static_cast<int>(mongoc_collection_estimated_document_count(collection, NULL, NULL, NULL, &error));
		}
		int count(const Document& _filter = {}) {
			bson_error_t error;
			bson_t* filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			const int result = static_cast<int>(
				mongoc_collection_count_documents(
					collection, filter, NULL, NULL, NULL, &error
				)
				);
			bson_free(filter);
			return result;
		}
		void deleteMany(const Document& _filter) {
			bson_error_t error;
			bson_t* filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			mongoc_collection_delete_many(collection, filter, NULL, NULL, &error);
			bson_free(filter);
		}
		void deleteOne(const Document& _filter) {
			bson_error_t error;
			bson_t* filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			mongoc_collection_delete_one(collection, filter, NULL, NULL, &error);
			bson_free(filter);
		}
		void drop() {
			bson_error_t error;
			mongoc_collection_drop(collection, &error);
		}
		void rename(const char* newdb, const char* newcoll, bool dropBeforeRename) {
			bson_error_t error;
			this->name = newcoll;
			mongoc_collection_rename(collection, newdb, newcoll, dropBeforeRename, &error);
		}
		void replaceOne(const Document& _old, const Document& __new) {
			bson_error_t error;
			bson_t* old = bson_new_from_json((const uint8_t*)_old.dump().c_str(), -1, &error);
			bson_t* _new = bson_new_from_json((const uint8_t*)__new.dump().c_str(), -1, &error);
			mongoc_collection_replace_one(collection, old, _new, NULL, NULL, &error);
			bson_free(old);
			bson_free(_new);
		}
		void updateMany(const Document& _filter, const Document& _update) {
			bson_error_t error;
			bson_t* filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			bson_t* update = bson_new_from_json((const uint8_t*)_update.dump().c_str(), -1, &error);
			mongoc_collection_update_many(collection, filter, update, NULL, NULL, &error);
			bson_free(filter);
			bson_free(update);
		}
		void updateOne(const Document& _filter, const Document& _update) {
			bson_error_t error;
			bson_t* filter = bson_new_from_json((const uint8_t*)_filter.dump().c_str(), -1, &error);
			bson_t* update = bson_new_from_json((const uint8_t*)_update.dump().c_str(), -1, &error);
			mongoc_collection_update_one(collection, filter, update, NULL, NULL, &error);
			bson_free(filter);
			bson_free(update);
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