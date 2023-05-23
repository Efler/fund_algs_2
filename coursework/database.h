#ifndef COURSEWORK_DATABASE_H
#define COURSEWORK_DATABASE_H
#include "../red_black_tree/red_black_tree.h"
#include "../allocator_1/allocator_1.h"
#include "../allocator_2/allocator_2.h"
#include "../allocator_3/allocator_3.h"
#include "../allocator_buddies/allocator_buddies.h"
#include "../avl_tree/avl_tree.h"


////TODO: 1. READ RANGE
////      2. DIALOG
////      3. MAKE README
////      4. ENABLE/DISABLE TREE LOGGING


class database final{

    ////* ------------------------------ ENUM FIELD ------------------------------ *////

public:

    enum class names_of_containers{
        avl_tree,
        red_black_tree
    };

    enum class names_of_allocators{
        global_heap,
        sorted_list,
        border_descriptor,
        buddies
    };

    enum class allocation_mode{
        first,
        best,
        worst,
        none
    };

    ////* ------------------------------ STRUCTS FIELD ------------------------------ *////

private:

    struct pipeline_key final
    {
        int id;
        string build_version;
    };

    struct pipeline_commit_info final
    {
        string hash;
        string developer_login;
        string developer_email;

        bool operator==(const pipeline_commit_info& other) const {
            if(this->hash == other.hash && this->developer_login == other.developer_login && this->developer_email == other.developer_email) return true;
            else return false;
        }

        bool operator!=(const pipeline_commit_info& other) const {
            return !(*this == other);
        }
    };

    struct pipeline_value final
    {
        pipeline_commit_info commit_info;
        string assembly_script_path;
        string assembly_name;
        string build_errors;
        string static_code_analysis_errors;
        string autotest_run_errors;
        string assembly_artifacts_dir;

        bool operator==(const pipeline_value& other) const {
            if(this->commit_info == other.commit_info &&
               this->assembly_script_path == other.assembly_script_path &&
               this->assembly_name == other.assembly_name &&
               this->build_errors == other.build_errors &&
               this->static_code_analysis_errors == other.static_code_analysis_errors &&
               this->autotest_run_errors == other.autotest_run_errors &&
               this->assembly_artifacts_dir == other.assembly_artifacts_dir) return true;
            else return false;
        }

        bool operator!=(const pipeline_value& other) const {
            return !(*this == other);
        }
    };

    ////* ------------------------------ COMPARATORS FIELD ------------------------------ *////

private:

    class string_comparer final
    {
    public:
        string_comparer() = default;
        int operator()(const string& s1, const string& s2) const {
            return s1.compare(s2);
        }
    };

    class pipeline_key_comparer final
    {
    public:
        pipeline_key_comparer() = default;
        int operator()(const pipeline_key& k1, const pipeline_key& k2) const {
            int res = k1.id - k2.id;
            if(res != 0) return res;
            else return k1.build_version.compare(k2.build_version);
        }
    };

    ////* ------------------------------ DATABASE POINTERS FIELD ------------------------------ *////

private:

    red_black_tree <string,   //database
                    red_black_tree <string,   //pool
                                    red_black_tree <string,   //scheme
                                                    associative_container <pipeline_key, pipeline_value>*,   //collection
                                                   string_comparer>*,
                                   string_comparer>*,
                   string_comparer>*
    _database;

    logger* _logger;
    allocator_1* _database_allocator;
    red_black_tree<string, abstract_allocator*, string_comparer>* _pool_allocators;


    ////* ------------------------------ CONSTRUCTORS FIELD ------------------------------ *////

public:

    explicit database(logger* l):
        _logger(l)
    {
        _database_allocator = new allocator_1();
        _database = new red_black_tree
                                      <string,                                                 //database
                                      red_black_tree <string,                                  //pool
                                      red_black_tree <string,                                  //scheme
                                      associative_container <pipeline_key, pipeline_value>*,   //collection
                                      string_comparer>*,
                                      string_comparer>*,
                                      string_comparer>(_database_allocator, _logger);

        _pool_allocators = new red_black_tree<string, abstract_allocator*, string_comparer>(_database_allocator, _logger);
        _logger->log("--- DATABASE CREATED! ---", logger::severity::information);
    }

    database(const database& other) = delete;
    database(database&& other) = delete;
    database& operator=(const database& other) = delete;
    database& operator=(database&& other) = delete;

    ////* ------------------------------ DESTRUCTOR FIELD ------------------------------ *////

public:

    ~database(){

        auto iter = _database->begin_postf();
        auto end_iter = _database->end_postf();
        for(; iter != end_iter; ++iter){
            auto pool_name = get<0>(*iter);
            delete_pool(pool_name);
        }

        delete _pool_allocators;
        delete _database;
        delete _database_allocator;

        _logger->log("--- DATABASE DELETED! ---", logger::severity::information);
    }

    ////* ------------------------------ RUN_FILE_COMMANDS FIELD ------------------------------ *////

private:

    pipeline_key parsing_key(const string& key){
        string key_str;
        size_t tmp1;
        size_t tmp2;

        if(key.find("key: ") != 0) throw logic_error("Error: wrong format of a key string");
        key_str = key.substr(5);

        tmp1 = key_str.find('[');
        if(tmp1 != 0) throw logic_error("Error: wrong format of a key string");
        tmp2 = key_str.find("] ");
        if(tmp2 == string::npos) throw logic_error("Error: wrong format of a key string");
        string id_str = key_str.substr(1, tmp2 - 1);
        if(id_str.empty()) throw logic_error("Error: invalid id");
        for(char c : id_str) if(!isdigit(c)) throw logic_error("Error: invalid id");
        int id = stoi(id_str);
        key_str = key_str.substr(tmp2 + 2);

        tmp1 = key_str.find('[');
        if(tmp1 != 0) throw logic_error("Error: wrong format of a key string");
        tmp2 = key_str.find(']');
        if(tmp2 == string::npos) throw logic_error("Error: wrong format of a key string");
        string build_version_str = key_str.substr(1, tmp2 - 1);
        if(build_version_str.empty()) throw logic_error("Error: invalid build version");

        pipeline_key k = {id, build_version_str};
        return k;
    }

    string parse_field(string& value_str, const string& name){
        size_t tmp1;
        size_t tmp2;

        tmp1 = value_str.find('[');
        if(tmp1 != 0) throw logic_error("Error: wrong format of a field");
        tmp2 = value_str.find("] ");
        if(tmp2 == string::npos) throw logic_error("Error: wrong format of a field");
        string result = value_str.substr(1, tmp2 - 1);
        if(result.empty()){
            string msg = "Error: invalid ";
            msg += name;
            throw logic_error(msg);
        }
        value_str = value_str.substr(tmp2 + 2);
        return result;
    }

    pipeline_value parsing_value(const string& value){
        string value_str;
        size_t tmp1;
        size_t tmp2;

        if(value.find("value: ") != 0) throw logic_error("Error: wrong format of a value string");
        value_str = value.substr(7);

        string hash_str;
        string developer_login_str;
        string developer_email_str;
        string assembly_script_path_str;
        string assembly_name_str;
        string build_errors_str;
        string static_code_analysis_errors_str;
        string autotest_run_errors_str;
        string assembly_artifacts_dir_str;

        try{ hash_str = parse_field(value_str, "hash"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ developer_login_str = parse_field(value_str, "developer login"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ developer_email_str = parse_field(value_str, "developer email"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ assembly_script_path_str = parse_field(value_str, "assembly script path"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ assembly_name_str = parse_field(value_str, "assembly name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ build_errors_str = parse_field(value_str, "build errors"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ static_code_analysis_errors_str = parse_field(value_str, "static code analysis errors"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ autotest_run_errors_str = parse_field(value_str, "autotest run errors"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        tmp1 = value_str.find('[');
        if(tmp1 != 0) throw logic_error("Error: wrong format of a value string");
        tmp2 = value_str.find(']');
        assembly_artifacts_dir_str = value_str.substr(1, tmp2 - 1);
        if(assembly_artifacts_dir_str.empty()) throw logic_error("Error: invalid assembly artifacts directory");

        pipeline_commit_info c = {hash_str, developer_login_str, developer_email_str};
        pipeline_value v = {c, assembly_script_path_str, assembly_name_str, build_errors_str, static_code_analysis_errors_str, autotest_run_errors_str, assembly_artifacts_dir_str};

        return v;
    }

    string from_value_to_output_string(const pipeline_value& v){
        string result = "\n[Pipeline commit information]:\n   [hash]: ";
        result += v.commit_info.hash;
        result += "\n   [developer login]: ";
        result += v.commit_info.developer_login;
        result += "\n   [developer e-mail]: ";
        result += v.commit_info.developer_email;
        result += "\n[Assembly script path]: ";
        result += v.assembly_script_path;
        result += "\n[Assembly name]: ";
        result += v.assembly_name;
        result += "\n[Build errors]: ";
        result += v.build_errors;
        result += "\n[Static code analysis errors]: ";
        result += v.static_code_analysis_errors;
        result += "\n[Autotest run errors]: ";
        result += v.autotest_run_errors;
        result += "\n[Assembly artifacts directory]: ";
        result += v.assembly_artifacts_dir;
        result += "\n";
        return result;
    }

    void parsing_pool_scheme_collection(string& pool_name, string& scheme_name, string& collection_name, string& command_str){
        size_t tmp1;
        size_t tmp2;

        try{ pool_name = parse_field(command_str, "pool name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme_name = parse_field(command_str, "scheme name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        tmp1 = command_str.find('[');
        if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
        tmp2 = command_str.find(']');
        if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
        collection_name = command_str.substr(1, tmp2 - 1);
        if(collection_name.empty()) throw logic_error("Error: invalid collection name");
    }

    void parsing_command(const string& command, const string& key, const string& value){
        string command_str;
        size_t tmp1;
        size_t tmp2;

        if(command.find("insert: ") == 0){
            command_str = command.substr(8);

            string pool_name;
            string scheme_name;
            string collection_name;

            try{ parsing_pool_scheme_collection(pool_name, scheme_name, collection_name, command_str); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            pipeline_key k;
            pipeline_value v;

            try{k = std::move(parsing_key(key));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{v = std::move(parsing_value(value));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{insert(k, v, pool_name, scheme_name, collection_name);} catch(const logic_error& ex) {throw logic_error(ex.what());}

            _logger->log(">> Insert DONE!", logger::severity::information);

        }
        else if(command.find("read key: ") == 0){
            command_str = command.substr(10);

            string pool_name;
            string scheme_name;
            string collection_name;

            try{ parsing_pool_scheme_collection(pool_name, scheme_name, collection_name, command_str); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            pipeline_key k;
            pipeline_value v;

            try{k = std::move(parsing_key(key));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{read_key(k, v, pool_name, scheme_name, collection_name);} catch(const logic_error& ex) {throw logic_error(ex.what());}

            string msg = "\n\n>> Reading key [id: ";
            msg += to_string(k.id);
            msg += ", build version: ";
            msg += k.build_version;
            msg += "]";
            msg += from_value_to_output_string(v);
            _logger->log(msg, logger::severity::information);

        }
        else if(command.find("update key: ") == 0){
            command_str = command.substr(12);

            string pool_name;
            string scheme_name;
            string collection_name;

            try{ parsing_pool_scheme_collection(pool_name, scheme_name, collection_name, command_str); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            pipeline_key k;
            pipeline_value v;

            try{k = std::move(parsing_key(key));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{v = std::move(parsing_value(value));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{update_key(k, v, pool_name, scheme_name, collection_name);} catch(const logic_error& ex) {throw logic_error(ex.what());}

            _logger->log(">> Update key DONE!", logger::severity::information);

        }
        else if(command.find("remove: ") == 0){
            command_str = command.substr(8);

            string pool_name;
            string scheme_name;
            string collection_name;

            try{ parsing_pool_scheme_collection(pool_name, scheme_name, collection_name, command_str); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            pipeline_key k;

            try{k = std::move(parsing_key(key));} catch(const logic_error& ex) {throw logic_error(ex.what());}

            try{remove(k, pool_name, scheme_name, collection_name);} catch(const logic_error& ex) {throw logic_error(ex.what());}

            _logger->log(">> Remove DONE!", logger::severity::information);
        }
        else if(command.find("add pool: ") == 0){
            command_str = command.substr(10);

            string pool_name;
            names_of_allocators alloc_name;
            size_t size;
            allocation_mode alloc_mode;

            try{ pool_name = parse_field(command_str, "pool name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            tmp1 = command_str.find("{ ");
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            command_str = command_str.substr(2);

            tmp1 = command_str.find('[');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string1");
            tmp2 = command_str.find("] ");
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            string alloc_name_str = command_str.substr(1, tmp2 - 1);
            if(alloc_name_str.empty()) throw logic_error("Error: invalid allocator name");
            if(alloc_name_str == "global heap") alloc_name = names_of_allocators::global_heap;
            else if(alloc_name_str == "sorted list") alloc_name = names_of_allocators::sorted_list;
            else if(alloc_name_str == "border descriptor") alloc_name = names_of_allocators::border_descriptor;
            else if(alloc_name_str == "buddies") alloc_name = names_of_allocators::buddies;
            else throw logic_error("Error: invalid allocator name");
            command_str = command_str.substr(tmp2 + 2);

            tmp1 = command_str.find('[');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            tmp2 = command_str.find("] ");
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            string size_str = command_str.substr(1, tmp2 - 1);
            if(size_str.empty()) throw logic_error("Error: invalid size");
            for(char c : size_str) if(!isdigit(c)) throw logic_error("Error: invalid size");
            size = (size_t)(stoi(size_str));
            command_str = command_str.substr(tmp2 + 2);

            if(alloc_name == names_of_allocators::sorted_list || alloc_name == names_of_allocators::border_descriptor){
                tmp1 = command_str.find('[');
                if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
                tmp2 = command_str.find("] ");
                if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
                string mode_str = command_str.substr(1, tmp2 - 1);
                if(mode_str.empty()) throw logic_error("Error: invalid mode");
                if(mode_str == "first") alloc_mode = allocation_mode::first;
                else if(mode_str == "best") alloc_mode = allocation_mode::best;
                else if(mode_str == "worst") alloc_mode = allocation_mode::worst;
                else throw logic_error("Error: invalid mode");
                command_str = command_str.substr(tmp2 + 2);

                tmp1 = command_str.find('}');
                if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            }else{
                alloc_mode = allocation_mode::none;
                tmp1 = command_str.find('}');
                if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            }

            try{ add_pool(pool_name, alloc_name, size, alloc_mode); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Add pool '";
            msg += pool_name;
            msg += "' DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("delete pool: ") == 0){
            command_str = command.substr(13);

            string pool_name;

            tmp1 = command_str.find('[');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            tmp2 = command_str.find(']');
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            pool_name = command_str.substr(1, tmp2 - 1);
            if(pool_name.empty()) throw logic_error("Error: invalid pool name");

            try{ delete_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Delete pool '";
            msg += pool_name;
            msg += "' DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("add scheme: ") == 0){
            command_str = command.substr(12);

            string pool_name;
            string scheme_name;

            try{ pool_name = parse_field(command_str, "pool name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            tmp1 = command_str.find('[');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            tmp2 = command_str.find(']');
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            scheme_name = command_str.substr(1, tmp2 - 1);
            if(scheme_name.empty()) throw logic_error("Error: invalid scheme name");

            try{ add_scheme(scheme_name, pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Add scheme '";
            msg += scheme_name;
            msg += "' (pool: '";
            msg += pool_name;
            msg += "') DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("delete scheme: ") == 0){
            command_str = command.substr(15);

            string pool_name;
            string scheme_name;

            try{ pool_name = parse_field(command_str, "pool name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            tmp1 = command_str.find('[');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            tmp2 = command_str.find(']');
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            scheme_name = command_str.substr(1, tmp2 - 1);
            if(scheme_name.empty()) throw logic_error("Error: invalid scheme name");

            try{ delete_scheme(scheme_name, pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Delete scheme '";
            msg += scheme_name;
            msg += "' (pool: '";
            msg += pool_name;
            msg += "') DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("add collection: ") == 0){
            command_str = command.substr(16);

            string pool_name;
            string scheme_name;
            string collection_name;
            names_of_containers tree_name;

            try{ pool_name = parse_field(command_str, "pool name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
            try{ scheme_name = parse_field(command_str, "scheme name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
            try{ collection_name = parse_field(command_str, "collection name"); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            tmp1 = command_str.find('{');
            if(tmp1 != 0) throw logic_error("Error: wrong format of a command string");
            tmp2 = command_str.find('}');
            if(tmp2 == string::npos) throw logic_error("Error: wrong format of a command string");
            string tree_name_str = command_str.substr(1, tmp2 - 1);
            if(tree_name_str.empty()) throw logic_error("Error: invalid tree name");
            if(tree_name_str == "avl tree") tree_name = names_of_containers::avl_tree;
            else if(tree_name_str == "red black tree") tree_name = names_of_containers::red_black_tree;
            else throw logic_error("Error: invalid tree name");

            try{ add_collection(collection_name, pool_name, scheme_name, tree_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Add collection '";
            msg += collection_name;
            msg += "' (pool: '";
            msg += pool_name;
            msg += "', scheme: '";
            msg += scheme_name;
            msg += "') DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("delete collection: ") == 0){
            command_str = command.substr(19);

            string pool_name;
            string scheme_name;
            string collection_name;

            try{ parsing_pool_scheme_collection(pool_name, scheme_name, collection_name, command_str); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            try{ delete_collection(collection_name, pool_name, scheme_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

            string msg = ">> Delete collection '";
            msg += collection_name;
            msg += "' (pool: '";
            msg += pool_name;
            msg += "', scheme: '";
            msg += scheme_name;
            msg += "') DONE!";
            _logger->log(msg, logger::severity::information);
        }
        else if(command.find("read range: ") == 0){
            ////TODO: READ RANGE!!
        }
        else throw logic_error("Error: invalid command/format!");
    }

public:

    void run_file_commands(const string& path){
        ifstream fin;
        fin.open(path);
        if(!fin.is_open()){
            throw logic_error("Could not open a file!");
        }

        string line;
        string line2;
        string line3;
        while(getline(fin, line)){

                        ////TODO: READ RANGE !!!!

            if(line.find("insert: ") == 0 || line.find("update key: ") == 0){
                if(!getline(fin, line2)){
                    fin.close();
                    throw logic_error("Error: wrong format of a command string (no key)");
                }
                if(!getline(fin, line3)){
                    fin.close();
                    throw logic_error("Error: wrong format of a command string (no value)");
                }
            }else if(line.find("read key: ") == 0 || line.find("remove: ") == 0){
                if(!getline(fin, line2)){
                    fin.close();
                    throw logic_error("Error: wrong format of a command string (no key)");
                }
            }else if(line.find("add pool: ") == 0 || line.find("delete pool: ") == 0 || line.find("add scheme: ") == 0 || line.find("delete scheme: ") == 0 || line.find("add collection: ") == 0 || line.find("delete collection: ") == 0){
                try{ parsing_command(line, line2, line3); } catch(const logic_error& ex){
                    fin.close();
                    throw logic_error(ex.what());
                }
                continue;
            }
            else{
                fin.close();
                throw logic_error("Error: invalid command/format!");
            }

            try{ parsing_command(line, line2, line3); } catch(const logic_error& ex){
                fin.close();
                throw logic_error(ex.what());
            }
        }
        fin.close();
        _logger->log("==>> PARSING DONE!", logger::severity::information);
    }

    ////* ------------------------------ DIALOG FIELD ------------------------------ *////

public:

    void start_dialog(){
        ////TODO: !!
    }

    ////* ------------------------------ COMMANDS FIELD ------------------------------ *////

private:

    red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* get_pool(const string& pool_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        try{
            pool = _database->get(pool_name);
        }catch(const logic_error& ex){
            string error_msg = "Error: pool '";
            error_msg += pool_name;
            error_msg += "' is not found!";
            throw logic_error(error_msg);
        }
        return pool;
    }

    red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* get_scheme(const string& scheme_name, const red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool){
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;
        try{
            scheme = pool->get(scheme_name);
        }catch(const logic_error& ex){
            string error_msg = "Error: scheme '";
            error_msg += scheme_name;
            error_msg += "' is not found!";
            throw logic_error(error_msg);
        }
        return scheme;
    }

    associative_container<pipeline_key, pipeline_value>* get_collection(const string& collection_name, const red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme){
        associative_container<pipeline_key, pipeline_value>* collection;
        try{
            collection = scheme->get(collection_name);
        }catch(const logic_error& ex){
            string error_msg = "Error: collection '";
            error_msg += collection_name;
            error_msg += "' is not found!";
            throw logic_error(error_msg);
        }
        return collection;
    }

private:

    void insert(const pipeline_key& key, const pipeline_value& value, const string& pool_name, const string& scheme_name, const string& collection_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;
        associative_container<pipeline_key, pipeline_value>* collection;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ collection = get_collection(collection_name, scheme); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        try{
            collection->insert(key, value);
        }catch(const logic_error& ex){
            string error_msg = "Error: in collection '";
            error_msg += collection_name;
            error_msg += "': key is already exists!";
            throw logic_error(error_msg);
        }

        _logger->log("(insert complete)", logger::severity::information);
    }

    void read_key(const pipeline_key& key, pipeline_value& value, const string& pool_name, const string& scheme_name, const string& collection_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;
        associative_container<pipeline_key, pipeline_value>* collection;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ collection = get_collection(collection_name, scheme); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        try{
            value = collection->get(key);
        }catch(const logic_error& ex){
            string error_msg = "Error: in collection '";
            error_msg += collection_name;
            error_msg += "': key is not found!";
            throw logic_error(error_msg);
        }

        _logger->log("(read key complete)", logger::severity::information);
    }

    void read_range(const pipeline_key& a, const pipeline_key& b, vector<pipeline_value>& values){
        ////TODO: !!
    }

    void update_key(const pipeline_key& key, const pipeline_value& value, const string& pool_name, const string& scheme_name, const string& collection_name){
        try{
            remove(key, pool_name, scheme_name, collection_name);
            insert(key, value, pool_name, scheme_name, collection_name);
        }catch(const logic_error& ex){
            throw logic_error(ex.what());
        }

        _logger->log("(update key complete)", logger::severity::information);
    }

    void remove(const pipeline_key& key, const string& pool_name, const string& scheme_name, const string& collection_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;
        associative_container<pipeline_key, pipeline_value>* collection;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ collection = get_collection(collection_name, scheme); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        try{
            collection->remove(key);
        }catch(const logic_error& ex){
            string error_msg = "Error: in collection '";
            error_msg += collection_name;
            error_msg += "': key is not found!";
            throw logic_error(error_msg);
        }

        _logger->log("(remove complete)", logger::severity::information);
    }

    void add_pool(const string& pool_name, names_of_allocators allocator_name, size_t size, allocation_mode mode = allocation_mode::none){
        bool flag = false;

        try{_database->get(pool_name);}catch(const logic_error& ex){ flag = true;}
        if(!flag){
            string error_msg = "Error: pool '";
            error_msg += pool_name;
            error_msg += "' is already exists!";
            throw logic_error(error_msg);
        }

        abstract_allocator* alloc;
        if(allocator_name == names_of_allocators::sorted_list){
            if(mode == allocation_mode::first) alloc = new allocator_2(size, allocator_2::mode::first);
            else if(mode == allocation_mode::best) alloc = new allocator_2(size, allocator_2::mode::best);
            else if(mode == allocation_mode::worst) alloc = new allocator_2(size, allocator_2::mode::worst);
            else throw logic_error("Error: in add_pool: wrong allocation mode!");
        }else if(allocator_name == names_of_allocators::border_descriptor){
            if (mode == allocation_mode::first) alloc = new allocator_3(size, allocator_3::mode::first);
            else if (mode == allocation_mode::best) alloc = new allocator_3(size, allocator_3::mode::best);
            else if (mode == allocation_mode::worst) alloc = new allocator_3(size, allocator_3::mode::worst);
            else throw logic_error("Error: in add_pool: wrong allocation mode!");
        }else if(allocator_name == names_of_allocators::buddies){
            alloc = new allocator_buddies(size);
        }else{
            alloc = new allocator_1();
        }

        auto* tree = reinterpret_cast<red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>))));
        new (tree) red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>(alloc, _logger);

        _database->insert(pool_name, tree);

        _pool_allocators->insert(pool_name, alloc);

        _logger->log("(add pool complete)", logger::severity::information);
    }

    void delete_pool(const string& pool_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        auto iter = pool->begin_postf();
        auto end_iter = pool->end_postf();
        for(; iter != end_iter; ++iter){
            auto scheme_name = get<0>(*iter);
            delete_scheme(scheme_name, pool_name);
        }

        auto* alloc = _pool_allocators->get(pool_name);
        pool->~red_black_tree();
        alloc->deallocate(reinterpret_cast<void*>(pool));

        _pool_allocators->remove(pool_name);
        delete alloc;

        _database->remove(pool_name);

        _logger->log("(delete pool complete)", logger::severity::information);
    }

    void add_scheme(const string& scheme_name, const string& pool_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        bool flag = false;
        try{pool->get(scheme_name);}catch(const logic_error& ex){ flag = true;}
        if(!flag){
            string error_msg = "Error: scheme '";
            error_msg += scheme_name;
            error_msg += "' is already exists!";
            throw logic_error(error_msg);
        }

        auto* alloc = _pool_allocators->get(pool_name);

        auto* tree = reinterpret_cast<red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>))));
        new (tree) red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>(alloc, _logger);

        pool->insert(scheme_name, tree);

        _logger->log("(add scheme complete)", logger::severity::information);
    }

    void delete_scheme(const string& scheme_name, const string& pool_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        auto iter = scheme->begin_postf();
        auto end_iter = scheme->end_postf();
        for(; iter != end_iter; ++iter){
            auto collection_name = get<0>(*iter);
            delete_collection(collection_name, pool_name, scheme_name);
        }

        auto* alloc = _pool_allocators->get(pool_name);
        scheme->~red_black_tree();
        alloc->deallocate(reinterpret_cast<void*>(scheme));

        pool->remove(scheme_name);

        _logger->log("(delete scheme complete)", logger::severity::information);
    }

    void add_collection(const string& collection_name, const string& pool_name, const string& scheme_name, names_of_containers type){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        bool flag = false;
        try{scheme->get(collection_name);}catch(const logic_error& ex){flag = true;}
        if(!flag){
            string error_msg = "Error: collection '";
            error_msg += collection_name;
            error_msg += "' is already exists!";
            throw logic_error(error_msg);
        }

        auto* alloc = _pool_allocators->get(pool_name);

        if(type == names_of_containers::avl_tree){
            auto* tree = reinterpret_cast<avl_tree<pipeline_key, pipeline_value, pipeline_key_comparer>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(avl_tree<pipeline_key, pipeline_value, pipeline_key_comparer>))));
            new (tree) avl_tree<pipeline_key, pipeline_value, pipeline_key_comparer>(alloc, _logger);

            scheme->insert(collection_name, tree);
        }else{
            auto* tree = reinterpret_cast<red_black_tree<pipeline_key, pipeline_value, pipeline_key_comparer>*>(alloc->allocate(reinterpret_cast<size_t>(sizeof(red_black_tree<pipeline_key, pipeline_value, pipeline_key_comparer>))));
            new (tree) red_black_tree<pipeline_key, pipeline_value, pipeline_key_comparer>(alloc, _logger);

            scheme->insert(collection_name, tree);
        }

        _logger->log("(add collection complete)", logger::severity::information);
    }

    void delete_collection(const string& collection_name, const string& pool_name, const string& scheme_name){
        red_black_tree<string, red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>*, string_comparer>* pool;
        red_black_tree<string, associative_container<pipeline_key, pipeline_value>*, string_comparer>* scheme;
        associative_container<pipeline_key, pipeline_value>* collection;

        try{ pool = get_pool(pool_name); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ scheme = get_scheme(scheme_name, pool); } catch(const logic_error& ex){ throw logic_error(ex.what()); }
        try{ collection = get_collection(collection_name, scheme); } catch(const logic_error& ex){ throw logic_error(ex.what()); }

        auto* alloc = _pool_allocators->get(pool_name);
        collection->~associative_container();
        alloc->deallocate(reinterpret_cast<void*>(collection));

        scheme->remove(collection_name);

        _logger->log("(delete collection complete)", logger::severity::information);
    }

};


#endif //COURSEWORK_DATABASE_H
