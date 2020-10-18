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

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

class dboperation
{

  mongocxx::instance instance{};
  mongocxx::client client{mongocxx::uri{}};

public:
  void register_Person(std::string userName, std::string password);
  bool view_record(std::string userName);
  void delete_user(std::string name);
  bool login_user(std::string userName, std::string password);
};

void dboperation::register_Person(std::string userName, std::string password)
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

bool dboperation::login_user(std::string userName, std::string password)
{
  std::cout << "check1";
  mongocxx::database db = client["chat_db"];

  mongocxx::collection crud_operation = db["user_data"];
  std::cout << "check2";
  bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      crud_operation.find_one(document{} << "UserName"

                                         << userName
                                         << "Password"
                                         << password << finalize);
  std:: cout<< "check3";
  if (maybe_result)
  {
    return true;
  }
  std::cout << "authentication failed";
  return false;
}

bool dboperation::view_record(std::string userName)
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

void dboperation::delete_user(std::string userName)
{
  mongocxx::database db = client["chat_db"];
  mongocxx::collection collection = db["user_data"];

  bsoncxx::stdx::optional<mongocxx::result::delete_result> result =
      collection.delete_many(
          document{} << "UserName" << open_document << "$eq" << userName << close_document << finalize);
}

// int main()
// {
//   dboperation dataObj;
//   dataObj.register_Person("kajal", "kajal12456");
//   dataObj.register_Person("kajal", "kajal12456");
//   dataObj.register_Person("sandeep", "sandeep12456");
//   dataObj.view_record("kajal");
//   std::string user_name;
//   std::string password;
//   std::cout << "Enter the user Name";
//   std::cin >> user_name;
//   std::cout << "Enter the Password";
//   std::cin >> password;
//   dataObj.login_user(user_name, password);
//   //dataObj.delete_user("kajal");
//   //dataObj.view_record("kajal");
//   return 0;
// }