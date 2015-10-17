#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}


/**
 * Implementation of generateShortestPath
 */
//path & generateShortestPath(const imdb & db, const string &source, const string &target) {
void generateShortestPath(const imdb & db, const string &source, const string &target) {

    list<path> partialPaths;
    set<string> previouslySeenActors;
    set<string> previouslySeenMovies;

    previouslySeenActors.insert(source);    // Exclude our source actor
    path sourcePath(source);
    partialPaths.push_front(sourcePath);
    while ((!partialPaths.empty()) && (partialPaths.front().getLength() < 5)){
	path frontpath = partialPaths.front();
	partialPaths.pop_front();
	vector<film> films;
	string cplayer = frontpath.getLastPlayer();
	db.getCredits(cplayer, films);
	for(vector<film>::iterator it=films.begin(); it != films.end(); ++it){
	    // Might need extra logic to check movie years match..
	    if(previouslySeenMovies.find(it->title) != previouslySeenMovies.end()){
		continue;	    // Skip ahead if we've done this movie
	    } else {		    // otherwise add it to the set
		previouslySeenMovies.insert(it->title);
	    }
	    vector<string> players;
	    db.getCast(*it, players);
	    for(vector<string>::iterator itp=players.begin(); itp != players.end(); ++itp){
		if(previouslySeenActors.find(*itp) != previouslySeenActors.end()){
		   continue;
		} else {
		   previouslySeenActors.insert(*itp);
		} 
		path clone = frontpath;
		clone.addConnection(*it, *itp);
		if(*itp == target){
		    cout << clone << endl;
		    return;
		} else { 
		    partialPaths.push_back(clone);
		}  
	    }
	}
    }
    cout << "Didn't find a path" << endl;

}


/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
 // imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      // replace the following line by a call to your generateShortestPath routine... 
      //
	generateShortestPath(db, source, target);     
    }
  }
  
  cout << endl << "No path between those two people could be found." << endl << endl;
  cout << "Thanks for playing!" << endl;
  return 0;
}

