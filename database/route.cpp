#include "route.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Route::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Route` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`name` VARCHAR(1024) NOT NULL,"
                        << "`creator_id` INT NOT NULL,"
                        << "PRIMARY KEY (`id`),"
                        << "CONSTRAINT fk_c_u foreign key (creator_id) references User (id))",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
        std::cout << "Entity Route initiated\n";
    }

    Poco::JSON::Object::Ptr Route::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("name", _name);
        root->set("creator_id", _creator_id);

        return root;
    }

    Route Route::fromJSON(const std::string &str)
    {
        Route route;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        route.id() = object->getValue<long>("id");
        route.name() = object->getValue<std::string>("name");

        return route;
    }

    std::optional<Route> Route::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Route a;
            select << "SELECT id, name, creator_id FROM Route where id=?",
                into(a._id),
                into(a._name),
                into(a._creator_id),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    void Route::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Route (name, creator_id) VALUES(?, ?)",
                use(_name),
                use(_creator_id);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long Route::get_id() const
    {
        return _id;
    }

    const std::string &Route::get_name() const
    {
        return _name;
    }

    long &Route::id()
    {
        return _id;
    }

    std::string &Route::name()
    {
        return _name;
    }
    
    long Route::get_creator_id() const {
        return _creator_id;
    }
    long& Route::creator_id() {
        return _creator_id;
    }
}