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
    printf("test string: %s\n",str2);

    return strcmp(str1, str2);	
    return 0;	
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 

    int nActors =  *(int *)actorFile;
    int nMovies = *(int *)movieFile;
    int *offarray = (int *)(actorFile);

    struct actor key = { player.c_str(), (void *)actorFile };
    int *res;

    // Cast (int *) just to avoid warning
    res = (int *)bsearch((void **)&key, &offarray[1], nActors, sizeof(int), myStrCmp);
    if (res == NULL) {
	printf("Not found\n");
    } else {
	printf("found %d at offset\n", *res);
    }

    // Update our actor struct with address of actor record
    key.base = (char *)actorFile + *res;

//    printf("base: %lx\tname: %s\n", key.base, (char *)key.base);

    short actorCredits = *(short *)(key.base + strlen(key.name) + strlen(key.name + 1)%2)
        
    return false; 
}




bool imdb::getCast(const film& movie, vector<string>& players) const { return false; }

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
