using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  cout << "Data directory: " << directory << endl;
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// Comparison function for input actor and each actor in the actorFile
// cmp1 is the address of the input actor string, while cmp2 is the pointer
// to the next integer offset of an actor in the actorFile
int myStrCmp(const void *cmp1, const void *cmp2){

    char * str1 = * (char **) cmp1;
    char * str2 = (char *)(*(char **)((char *)cmp1 + sizeof(char *)) + *(int *)cmp2);
//				      ^__________^
//				       &actor
//				      ^___________________________^
//					  position of actor.base
//	                  ^________________________________________^
//			address of actorFile (i.e. dereferencing actor.base)
//		         ^________________________________________________________^
//			    address of actor.base + actor offset
//		  ^_______________________________________________________________^
//			    cast that address to be (char *)
//				            

    return strcmp(str1, str2);	
    //return 0;	
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 

    int nActors =  *(int *)actorFile;
    int *offarray = (int *)(actorFile);

    struct actor key = { player.c_str(), (void *)actorFile };
    int *res;

    // Cast (int *) just to avoid warning
    res = (int *)bsearch((void **)&key, &offarray[1], nActors, sizeof(int), myStrCmp);
    if (res == NULL) {
	printf("Not found\n");
	return false; 
    } else {
	printf("found %d at offset\n", *res);
    }

    // Update our actor struct with address of actor record
    key.base = (char *)actorFile + *res;

    int nameLen = strlen(key.name) + 1; // Get basic name length (including '\0')
    nameLen = nameLen + nameLen%2;	// Jump past padding
    short int actorCredits = *(short *)((char *)key.base + nameLen);

    // Get pointer to the offsets to the first movie record for the actor
    int *movOffsets = (int *)((char *)key.base + nameLen + sizeof(short)); 
   // movOffsets += (nameLen + sizeof(short))%4; // Jump past padding
    movOffsets = (int *)((char *)movOffsets + (nameLen + sizeof(short))%4); // Jump past padding
    
    struct film thisfilm;
    char yearbyte;
    int  titlelen;
    char *movie;

    for(int i = 0; i < actorCredits; i++) {
	movie = (char *)((char *)movieFile + *(movOffsets + i));
	thisfilm.title = movie;
	titlelen = strlen(movie) + 1;
	yearbyte = *(movie + titlelen);
	thisfilm.year = 1900 + yearbyte ;
//	printf("Movie: %s\tYear: %d\tyearbyte: %d\n", movie, thisfilm.year, yearbyte);
//	printf("Movie: %s\tYear: %d\n", movie, thisfilm.year);
	films.push_back(thisfilm);
//	movie += sizeof(int);
    } 
    return true;
}

// Comparison function for input actor and each actor in the actorFile
// cmp1 is the address of the input actor string, while cmp2 is the pointer
// to the next integer offset of an actor in the actorFile
int myStrCmp2(const void *cmp1, const void *cmp2){

    struct film f1 = *(struct film *)cmp1;
    struct filmwrap fw1 = *(struct filmwrap *)cmp1;
    int * base = fw1.base;
    char * str = (char *)base + *(int *)cmp2;
    int titlelen = strlen(str) + 1;
    char yearbyte = *(str + titlelen);

    struct film f2;
    f2.title = str;
    f2.year = 1900 + yearbyte;
    
    if (f2 == f1) {
	return 0;
    } else if (f2<f1) { // Key is greater than test element
	return 1;
    } else {
	return -1;  // Key is less than test element
    }

}
bool imdb::getCast(const film& movie, vector<string>& players) const { 

    int *offarray = (int *)(movieFile);
    int nMovies = *(int *)movieFile;

    struct film key = movie;
    struct filmwrap wkey = {key, offarray};
    int *res;
    cout << endl;

    res = (int *)bsearch((void **)&wkey, &offarray[1], nMovies, sizeof(int), myStrCmp2);
    if (res == NULL) {
	printf("Not found\n");
	return false; 
    } else {
	printf("found %d at offset\n", *res);
    }
    wkey.base = (int *)((char *)movieFile + *res);
//    printf("Found Movie %s\n",(char *)wkey.base);    

    int titleLen = strlen((char*)wkey.base) + 2; // Add 1 for '\0' and 1 for year byte
    titleLen = titleLen + titleLen%2;
    short int nActors = *(short *)((char *)wkey.base + titleLen);
    
    int * actorOffsets = (int *)((char *)wkey.base + titleLen + sizeof(short));
    actorOffsets = (int *)((char *)actorOffsets + (titleLen + sizeof(short))%4);

    char *actorStr;
    string actor;

    printf("Found Movie %s, %d actors\n",(char *)wkey.base, nActors);    
    for (int i = 0; i < nActors; i++) {
	actorStr = (char *)((char *)actorFile + *(actorOffsets + i));
//	printf("Actor: %s\n", actorStr);
	actor = actorStr;
	players.push_back(actor);
    }
    return true; 
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
