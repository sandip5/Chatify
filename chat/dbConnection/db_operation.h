#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <string>
#include <chrono>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

class db_operation
{

  mongocxx::instance instance{};
  mongocxx::client client{mongocxx::uri{}};

public:
  void register_user(std::string userName, std::string password);
  bool view_record(std::string userName);
  void delete_user(std::string name);
  bool login_user(std::string userName, std::string password);
  void save_chats_of_user(std::string sender, std::string receiver, std::string message);
  std::vector<std::string> get_saved_chats(std::string sender, std::string receiver);
  std::string make_common_collection(std::string sender, std::string receiver);
};

void db_operation::register_user(std::string userName, std::string password)
{
  mongocxx::database db = client["chat_db"];

  mongocxx::collection crud_operation = db["user_data"];

  bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      crud_operation.find_one(document{} << "UserName"

                                         << userName << finalize);
  if (maybe_result)
  {
    std::cout << "\n\n"
              << userName << "\tALREADY PRESENT!!!\n";
    return;
  }
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = builder
                                       << "UserName"
                                       << userName

                                       << "Password"
                                       << password

                                       << bsoncxx::builder::stream::finalize;

  crud_operation.insert_one(doc_value.view());

  view_record(userName);
}

bool db_operation::login_user(std::string userName, std::string password)
{
  mongocxx::database db = client["chat_db"];

  mongocxx::collection crud_operation = db["user_data"];
  bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      crud_operation.find_one(document{} << "UserName"

                                         << userName
                                         << "Password"
                                         << password << finalize);
  if (maybe_result)
  {
    return true;
  }
  return false;
}

bool db_operation::view_record(std::string userName)
{
  mongocxx::database db = client["chat_db"];
  mongocxx::collection collection = db["user_data"];
  bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      collection.find_one(document{} << "UserName"

                                     << userName << finalize);
  if (maybe_result)
  {
    std::cout << bsoncxx::to_json(*maybe_result) << "\n";
    return true;
  }
  std::cout << "\n\n " << userName << " NOT FOUND\n";
  return false;
}

void db_operation::delete_user(std::string userName)
{
  mongocxx::database db = client["chat_db"];
  mongocxx::collection collection = db["user_data"];

  bsoncxx::stdx::optional<mongocxx::result::delete_result> result =
      collection.delete_many(
          document{} << "UserName" << open_document << "$eq" << userName << close_document << finalize);
}

void db_operation::save_chats_of_user(std::string sender, std::string receiver, std::string message)
{
  std::string collectionName = make_common_collection(sender, receiver);
  mongocxx::collection collection = client["chat_db"][collectionName];

  auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto builder = bsoncxx::builder::stream::document{};

  bsoncxx::document::value data = builder
                                  << "sender" << sender
                                  << "receiver" << receiver
                                  << "message" << message
                                  << "time" << time << finalize;

  collection.insert_one(data.view());
}

std::vector<std::string> db_operation::get_saved_chats(std::string sender, std::string receiver)
{
  std::vector<std::string> messages;

  std::string collectionName = make_common_collection(sender, receiver);
  mongocxx::collection collection = client["chat_db"][collectionName];
  mongocxx::cursor result = collection.find({});

  for (auto &&data : result)
  {
    std::string message;

    message = data["message"].get_utf8().value.to_string();
    if (data["message"].get_utf8().value.size() > 0)
    {
      messages.push_back(message);
    }
  }
  return messages;
}

std::string db_operation::make_common_collection(std::string sender, std::string receiver)
{
  std::string str = sender + receiver;

  sort(str.begin(), str.end());

  return str;
}