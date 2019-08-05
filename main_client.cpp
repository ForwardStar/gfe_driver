/**
 * Copyright (C) 2019 Dean De Leo, email: dleo[at]cwi.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

#include "common/error.hpp"
#include "common/timer.hpp"
#include "experiment/aging.hpp"
#include "experiment/insert_only.hpp"
#include "experiment/graphalytics.hpp"
#include "graph/edge_stream.hpp"
#include "library/interface.hpp"
#include "network/client.hpp"
#include "network/error.hpp"
#include "third-party/cxxopts/cxxopts.hpp"

#include "configuration.hpp"

using namespace common;
using namespace std;

/*****************************************************************************
 *                                                                           *
 *  DEBUG                                                                    *
 *                                                                           *
 *****************************************************************************/
#define DEBUG
#define COUT_DEBUG_FORCE(msg) { LOG("[main_client::" << __FUNCTION__ << "] " << msg) }
#if defined(DEBUG)
    #define COUT_DEBUG(msg) COUT_DEBUG_FORCE(msg)
#else
    #define COUT_DEBUG(msg)
#endif


/*****************************************************************************
 *                                                                           *
 *  Interactive mode                                                         *
 *                                                                           *
 *****************************************************************************/
namespace {
// forward declaration
class Argument;
class Command;
static Command parse_command();

DEFINE_EXCEPTION(ParseCommandError);
#define PARSE_COMMAND_ERROR(message) RAISE_EXCEPTION(ParseCommandError, message)

class Argument {
    friend Command parse_command();
    const string m_argument;

    Argument(const char* start, int length) : m_argument(start, length) {}

public:
    string to_string() const{ return m_argument; }
    string to_s() const { return to_string(); } // alias

    uint64_t to_unsigned() const {
        try {
            return stoull(m_argument);
        } catch(std::invalid_argument& e){
            PARSE_COMMAND_ERROR("Cannot convert the value `" << m_argument << "' to an unsigned interger");
        }
    }
    uint64_t to_u() { return to_unsigned(); } // alias

    operator string() const { return to_string(); }
};

ostream& operator<<(ostream& out, const Argument& argument){
    out << argument.to_string();
    return out;
}

class Command {
    friend Command parse_command();

    string m_command;
    vector<Argument> m_arguments;

    Command(){};
public:
    const std::string& get_command() const{ return m_command; }

    std::string to_string() const {
        stringstream ss;
        ss << m_command << "(";
        bool first = true;
        for(auto& a: m_arguments){
            if(first) { first = false; } else { ss << ", "; }
            ss << a.to_string();
        }
        ss << ")";
        return ss.str();
    }

    const vector<Argument>& arguments() const { return m_arguments; };
    const int64_t num_arguments() const { return m_arguments.size(); }
    const Argument& operator[](int index){ return m_arguments[index]; }
};

[[maybe_unused]] std::ostream& operator<<(std::ostream& out, const Command& c){
    out << c.to_string();
    return out;
}

static Command parse_command(){
    Command command;

    cout << "> ";
    std::string user_prompt;
    getline(cin, user_prompt);

    int pos = 0; // position in the user prompt
    while(pos < user_prompt.length() && isspace(user_prompt[pos])) pos++;

    // parse the statement, single word
    int start = pos;
    while(pos < user_prompt.length() && (isalnum(user_prompt[pos]) || user_prompt[pos] == '_')) pos++;
    int end = pos;

//    COUT_DEBUG("start: " << start << ", end: " << end);
    command.m_command = string(user_prompt.c_str() + start, end - start);
    std::transform(command.m_command.begin(), command.m_command.end(), command.m_command.begin(), ::tolower); // all lowercase
//    COUT_DEBUG( "Command: " << command.m_command  );

    // skip empty spaces
    while(pos < user_prompt.length() && isspace(user_prompt[pos])) pos++;

    bool match_opened_parenthesis = false;
    if(pos < user_prompt.length() && user_prompt[pos] == '('){
        match_opened_parenthesis = true;
        pos++;
    }

    bool stop = false;
    bool skip_next_comma = false;
    while(pos < user_prompt.length() && !stop){ // next argument
        if(isspace(user_prompt[pos])){ pos++; continue; } // skip empty spaces at the start
        if(skip_next_comma && user_prompt[pos] == ','){ skip_next_comma = false; pos++; continue; }
        skip_next_comma = false;
//        cout << "character [1]: " << user_prompt[pos] << "\n";

        int string_delimiter = 0;
        if(match_opened_parenthesis && user_prompt[pos] == ')'){
//            cout << "match closed parenthesis [1]" << endl;
            break; // done
        } else if(user_prompt[pos] == '"' || user_prompt[pos] == '\''){
//            cout << "match start of string [1]\n";
            string_delimiter = user_prompt[pos];
            start = pos +1;
        } else {
//            cout << "match start [1]\n";
            start = pos;
        }

        pos++;

        bool terminated = false;
        while(pos < user_prompt.length() && !terminated){
//            cout << "character [2]: " << user_prompt[pos] << "\n";
            if(user_prompt[pos] == '\\'){
                pos+=2;
            } else if (string_delimiter == 0 && match_opened_parenthesis && user_prompt[pos] == ')'){
//                cout << "match closed parenthesis [2]" << endl;
                end = pos;
                terminated = true;
                stop = true;
            } else if (string_delimiter == 0 && (isspace(user_prompt[pos]) || user_prompt[pos] == ',')){
//                cout << "match end parameter [2]" << endl;
                skip_next_comma = (user_prompt[pos] != ',');
                end = pos;
                terminated = true;
            } else if (string_delimiter != 0 && user_prompt[pos] == string_delimiter){
//                cout << "match closed string [2]" << endl;
                end = pos;
                skip_next_comma = true;
                terminated = true;
            } else {
                pos++;
            }
        }
        if(!terminated){ end = pos; }

        if(start < end){
            command.m_arguments.push_back(Argument{user_prompt.c_str() + start, end - start});
        }

        pos ++;
    }

    return command;
}

} // anon

static void run_client_interactive(){
    // Open a connection with the server
    LOG("[client] Connecting to the server at " << cfgclient().get_server_string());
    network::Client impl { cfgclient().get_server_host(), (int) cfgclient().get_server_port() };

    while(true){
        Command command = parse_command();
        const string& stmt = command.get_command();
        if(stmt == "" && cin.eof()) { cout << "\n";  break; } // CTRL-D, we're done

//        cout << "command parsed: " << command << endl;

        try {
            if(stmt == "bfs"){
                bool error = false;
                if(command.num_arguments() == 0 || command.num_arguments() > 2) {
                    cerr<< "[client] Invalid number of arguments: " << command.num_arguments() << ". Syntax: bfs( source_vertex [, path_dump] );" << endl;
                    error = true;
                } else { // either 1 or 2 arguments
                    uint64_t source_vertex;
                    string str_path_dump;
                    try {
                        source_vertex = command[0].to_unsigned();

                        if(command.num_arguments() > 1){ str_path_dump = command[1]; }

                    } catch(ParseCommandError& e){
                        error = true;
                        cerr<< "[client] Invalid argument source_vertex: `" << command[0] << "'. Syntax: bfs( source_vertex [, path_dump] );" << endl;
                    }

                    if(!error){
                        Timer timer; timer.start();
                        impl.bfs(source_vertex, str_path_dump.c_str());
                        timer.stop();
                        cout << "[client] BFS on source vertex `" << source_vertex << "' completed in " << timer << "\n";
                    }
                }
            } else if(stmt == "d" || stmt == "dump" || stmt == "\\d") {
                if(command.num_arguments() == 0){
                    impl.dump();
                } else {
                    cerr << "[client] ERROR: invalid number of arguments: " << command.num_arguments() << ". Expected 0\n";
                }
            } else if (stmt == "directed" || stmt == "is_directed" || stmt == "undirected" || stmt == "is_undirected") {
                if(command.num_arguments() == 0){
                    if(impl.is_directed()){
                        cout << "[client] The graph is directed\n";
                    } else {
                        cout << "[client] The graph is undirected\n";
                    }
                } else {
                    cerr << "[client] ERROR: invalid number of arguments: " << command.num_arguments() << ". Expected 0\n";
                }
            } else if(stmt == "load"){
                if(command.num_arguments() != 1){
                    cerr << "[client] [load] invalid number of arguments, expected 1: load (path);" << endl;
                } else {
                    impl.load(command[0]);
                    cout << "[client] Load done\n";
                }
            } else if(stmt == "q" || stmt == "quit" || stmt == "\\q"){
                break;
            } else if(stmt == "terminate"){
                impl.set_terminate_server_on_exit(true);
                break;
            } else {
                cerr << "[client] ERROR, invalid command: " << stmt << endl;
            }

        } catch(network::RPCError& e){
            cerr << e << endl;
        }

    }
}

/*****************************************************************************
 *                                                                           *
 *  Non-interactive mode                                                     *
 *                                                                           *
 *****************************************************************************/

static void run_experiments(){
    using namespace experiment;

    const string& experiment = cfgclient().get_experiment_name();
    if(experiment.empty()) ERROR("Experiment not set (use the parameter --experiment)");
    LOG("[client] Experiment: " << experiment);

    if(experiment == "basic"){
        const std::string& path_graph = cfgclient().get_path_graph(); // graph to use?
        if(path_graph.empty()) ERROR("Path to the graph to load not set (use the parameter --graph)");

        // implementation to the evaluate
        LOG("[client] Connecting to the server at " << cfgclient().get_server_string());
        auto impl = make_shared<network::Client>( cfgclient().get_server_host(), (int) cfgclient().get_server_port() );

        LOG("[client] Loading the graph from " << path_graph);
        auto stream = make_shared<graph::WeightedEdgeStream> ( cfgclient().get_path_graph() );
        stream->permute();

        LOG("[client] Number of concurrent threads: " << cfgclient().num_threads(THREADS_WRITE) );
        if(cfgclient().num_updates() == 0){ // insert the elements in the graph one by one
            InsertOnly experiment { impl, move(stream), cfgclient().num_threads(THREADS_WRITE) };
            experiment.execute();
            if(configuration().has_database()) experiment.save();
        } else {
            LOG("[client] Number of updates to perform: " << cfgclient().num_updates());
            Aging experiment(impl, move(stream), cfgclient().num_updates(), cfgclient().num_threads(THREADS_WRITE));
            experiment.execute();
            if(configuration().has_database()) experiment.save();
        }

        // run the graphalytics suite
        GraphalyticsAlgorithms properties { path_graph };
        GraphalyticsSequential exp_seq { impl, cfgclient().num_repetitions(), properties };
        exp_seq.execute();
        exp_seq.report(configuration().has_database());

    } else {
        ERROR("Experiment not recognised: " << experiment);
    }
}

/*****************************************************************************
 *                                                                           *
 *  Main                                                                     *
 *                                                                           *
 *****************************************************************************/

static void run_client(int argc, char* argv[]){
    LOG("[client] Init configuration ... " );
    ClientConfiguration::initialise(argc, argv);

    if(configuration().has_database()){
        LOG( "[client] Save the current configuration properties in " << configuration().get_database_path() )
        configuration().save_parameters();
    }

    if(cfgclient().is_interactive()){
        LOG( "[client] Interactive mode" )
        run_client_interactive();
    } else {
        run_experiments();

//        // test
//        vector<uint64_t> values;
//        values.emplace_back(15);
//        values.emplace_back(75);
//        values.emplace_back(40);
//        values.emplace_back(60);
//        experiment::ExecStatistics stats{values};
//        cout << stats << endl;
//        stats.save("this_is_a_test");

    }

    LOG( "[client] Done" );
}

int main(int argc, char* argv[]){
    int rc = 0;
    try {
        run_client(argc, argv);
    } catch(common::Error& e){
        cerr << e << endl;
        cerr << "Client terminating due to exception..." << endl;
        rc = 1;
    } catch(cxxopts::option_not_exists_exception& e){
        cerr << "ERROR: " << e.what() << "\n";
        rc = 1;
    }

    cfgfree();
    return rc;
}

