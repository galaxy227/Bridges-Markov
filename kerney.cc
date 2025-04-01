#include "/public/read.h"   //Kerney's input thing
#include "/public/colors.h" //Kerney's color thing
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <cctype>
using namespace std;

//Each word will have a list of other words they're connected to by this weighted edge.
//For example if APPLE has the following Vertices associated with it: {PEAR,3},{GRAPE,2},{NANA,10} this means that three possible words follow APPLE (PEAR, GRAPE, NANA) and the probability we pick PEAR is 3/15, GRAPE 2/15, NANA 10/15.
//So we'd roll a number from 0 to 14 and if we got a 0-2 we'd pick PEAR, 3-4 we'd pick GRAPE, and the rest NANA
struct Edge {
	string word; //The word we're connected to
	int weight = 0; //How often they follow us
};

//Adjacency List style graph
struct Vertex {
	string word; //What word this vertex corresponds to, like APPLE
	int count = 0; //How often this word came up
	int total_edge_weight = 0; //Total weight of the edges coming out of this vertex
	int start_count = 0; //How often this word starts a line
	//End_count can just be computed by count - total_edge_weight
	//int end_count = 0; //How often this word ends a line
	int comma_count = 0; //How often the word has a comma following it
	vector<Edge> edges; //Holds the words we're connected to, and how often they follow us, like {PEAR,3}
};
ostream &operator<<(ostream &outs,const Vertex &v) {
	outs << "Vertex " << v.word << " - Count: " << v.count <<
		" TEW: " << v.total_edge_weight <<
		" Start Count: " << v.start_count <<
		" End Count: " << (v.count - v.total_edge_weight) <<
		" Comma Count: " << v.comma_count << 
		" Edges:" << endl;
	for (const Edge &e : v.edges) {
		outs << '\t' << e.word << ": " << e.weight << endl;
	}
	return outs;
}

//Utility functions for changing the case on words
//The graph will hold everything in ALL CAPS

//Turn food -> FOOD
inline void uppercaseify(string &s) {
	for (char &c : s) c = toupper(c);
}
//Turn FOOD -> food
inline void lowercaseify(string &s) {
	for (char &c : s) c = tolower(c);
}
//Turn FOOD -> Food, for starting a sentence
inline void first_cap(string &s) {
	if (!s.size()) return;
	for (char &c : s) c = tolower(c);
	s.at(0) = toupper(s.at(0));
}

//Eliminates all words between square brackets to remove informative lines in lyrics
//Example: "Chorus[GZA]:" becomes "Chorus:"
void strip_brackets(string &s) {
	string retval;
	bool deleting = false;
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == '[') deleting = true;
		else if (s[i] == ']') deleting = false;
		else if (!deleting) retval.push_back(s[i]);
	}
	s = retval;
}

int main() {
	const string filename = read("Please enter a text file to open:\n");
	ifstream ins(filename);
	if (!ins) {
		cout << "Bad filename yo\n";
		exit(EXIT_FAILURE);
	}
	
	unordered_map<string,size_t> hash; //Holds the index (in vec) of each word we've read
	vector<Vertex> graph; //Holds each word and who it's connected to via adjacency list
	long long total_start_count = 0; //Holds how many lines of the file we've read

	//PART 1 - BUILD THE GRAPH FROM THE FILE
	//YOU:
	//  getline one line at a time from the file, and turn it into a stringstream for parsing
	//  Note which word starts a line. The first and last word on a line matters, not periods.
	//  Create one vertex for each word, with a directed edge between them each time word B follows word A
	//  (All words read in should be uppercaseify-ed so that Home and HOME and home are the same word.)
	//  Preserve the order that words are read in, and the order than one word follows another
	//  Use the strip_brackets function to remove all characters between []'s
	//  Drop all trailing punctuation, but keep track of how often a comma follows each word
	//YOU: Set total_start_count to how many lines were read, based on getline, not periods.

	//After the graph is loaded, choose what to do
	cout << "1. Print Graph and Quit\n";
	cout << "2. Generate Random Lyrics\n";
	cout << "3. View on BRIDGES\n";
	int choice = read();
	if (choice == 1) {
		for (const Vertex &v : graph) {
			cout << v;
		}
		cout << "Total Start Count: " << total_start_count << endl;
		exit(0);
	}
	//PART 2 - GENERATE SOME SENTENCES
	else if (choice == 2) {
		//Sanity check that we read in at least one sentence
		if (!total_start_count) {
			cout << "Read no sentences, bailing out now...\n";
			exit(1);
		}

		const int sentences = read("How many sentences do you wish to make?\n");
		const int seed = read("Please enter the random seed:\n");
		srand(seed);

		//YOU: Generate the sentences
		int roll = rand() % total_start_count;
		//Which word should you pick to start the sentence? Go down the graph, and any time you find a word that started a sentence, subtract how many times it started a sentence from roll. If roll is then < 0, that's your starting word
		//Then go into a loop, and do the same thing to find the next word. Roll a die, modulus by the total_edge_weight of the starting node. This die will choose to either A) End the sentence, or B) To move to another vertex and keep going. 
		//  First, subtract out the number of times this vertex ended a sentence, which you can compute by the number of times this word appeared (i.e. count) and subtracting from it the number of times we left this word (i.e. total_edge_weight). If this subtraction drives the roll negative, end the sentence. Print a . and endl, and move on to the next sentence
		//  Otherwise, go through each edge in order, subtracting off their edge weight from the roll. When roll goes negative, that's the edge to follow. Print that word, and then repeat on that vertex until you reach the end of a sentence.
		//You need to handle commas with a separate die roll. Each word had a percentage chance to have a comma following it if it does not end a sentence.
	}
	//PART 3 - SEND THE GRAPH TO BRIDGES
	else {
		//Render the graph on BRIDGES
		//YOU: Figure this out by reading the BRIDGES API here: https://bridgesuncc.github.io/doc/cxx-api/current/html/index.html
	}
}
