#include "/public/read.h"
#include "Bridges.h"
#include "GraphAdjList.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <locale>
using namespace std;
using namespace bridges;

const int valid_char_arr_size = 24;
const int punctuation_arr_size = valid_char_arr_size - 3;
const char valid_char_arr[] = {
	// Punctuation
	'.',
	',',
	'!',
	'?',
	'-',
	'+',
	'&',
	'*',
	';',
	':',
	'<',
	'>',
	'{',
	'}',
	'(',
	')',
	'/',
	'\\',
	'\'',
	'\"',
	// Other
	' ',
	'[',
	']',
};

uint32_t TOTAL_START_COUNT = 0;

namespace NOT_BRIDGES {
struct Edge {
	uint32_t index;
	uint32_t weight;

	Edge(const uint32_t index, const uint32_t weight) : index(index), weight(weight) {}
};
struct Vertex {
	uint32_t word_count;
	uint32_t start_count;
	uint32_t end_count;
	uint32_t comma_count;
	uint32_t total_weight;
	string word;
	vector<NOT_BRIDGES::Edge> edge_vector;

	Vertex(const string word) : word_count(0), start_count(0), end_count(0), comma_count(0), total_weight(0), word(word) {}
};
};

void print_vertex_vector(vector<NOT_BRIDGES::Vertex> &vertex_vector) {
	for (const NOT_BRIDGES::Vertex &v : vertex_vector) {
		// Print
		cout << 
		"Vertex " << v.word << 
		" - Count: " << v.word_count <<
		" TEW: " << v.total_weight <<
		" Start Count: " << v.start_count <<
		" End Count: " << v.end_count <<
		" Comma Count: " << v.comma_count << 
		" Edges:" << endl;
		for (const NOT_BRIDGES::Edge &e : v.edge_vector) cout << '\t' << vertex_vector[e.index].word << ": " << e.weight << endl;
	}
}
bool is_valid_char(char &c) {
	if (isalpha(c) || isdigit(c)) return true;
	for (int i = 0; i < valid_char_arr_size; i++) {
		if (c == valid_char_arr[i]) return true;
	}
	return false;
}
void remove_brackets(string &s) {
	string retval;
	bool deleting = false;
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == '[') deleting = true;
		else if (s[i] == ']') deleting = false;
		else if (!deleting) retval.push_back(s[i]);
	}
	s = retval;
}
bool is_char_punctuation(char &c) {
	for (int i = 0; i < punctuation_arr_size; i++) {
		if (c == valid_char_arr[i]) return true;
	}
	return false;
}
void remove_punctuation(string &word) {
	if (word.empty()) return;
	// Bracket
	remove_brackets(word);
	// Punctuation
	while (is_char_punctuation(word[word.length() - 1])) {
		word = word.substr(0, word.length() - 1);	
	}
}
void add_word_to_sentence(string& sentence, string word, const bool is_starting) {
	for (char &c : word) c = tolower(c); 
	if (is_starting) word[0] = toupper(word[0]);
	else sentence += " ";
	sentence += word;
}

int main() {
	// Data
	vector<NOT_BRIDGES::Vertex> vertex_vector;
	unordered_map<string, uint32_t> word_map;
	// File
	const string filename = read("Please enter a text file to open:\n");
	ifstream file(filename);
	if (!file) { cout << "Bad filename yo\n"; exit(EXIT_FAILURE); }
	// Read
	string line;
	while (getline(file, line)) {
		remove_brackets(line);
		if (line.empty()) continue;
		line += ' ';
		bool is_start = true;
		string word = "";
		char last_char = ' ';
		int line_word_count = 0;
		uint32_t curr_index = 0;
		uint32_t prev_index = 0;
		// For each char in the line
		for (int i = 0; i < static_cast<int>(line.length()); i++) {
			if (!is_valid_char(line[i])) continue;
			if (line[i] != ' ') { word += line[i]; continue; }
			if (word.empty()) continue;
			for (char &c : word) c = toupper(c);
			// Handle punctuation
			last_char = word[word.length() - 1];
			remove_punctuation(word);
			if (word.empty()) continue;
			// If word does NOT exist in data
			if (word_map.find(word) == word_map.end()) {
				word_map[word] = vertex_vector.size();
				vertex_vector.push_back(NOT_BRIDGES::Vertex(word));
			}
			// Process word
			curr_index = word_map[word];
			vertex_vector[curr_index].word_count++;
			if (last_char == ',') vertex_vector[curr_index].comma_count++;
			// else if (last_char == '.') vertex_vector[curr_index].end_count++;
			if (is_start) {
				vertex_vector[curr_index].start_count++;
				TOTAL_START_COUNT++;
			}
			else {
				bool is_edge_exist = false;
				for (int j = 0; j < static_cast<int>(vertex_vector[prev_index].edge_vector.size()); j++) {
					if (curr_index == vertex_vector[prev_index].edge_vector.at(j).index) {
						vertex_vector[prev_index].edge_vector.at(j).weight++;
						is_edge_exist = true;
						break;
					}
				}
				if (!is_edge_exist) vertex_vector[prev_index].edge_vector.push_back(NOT_BRIDGES::Edge(curr_index, 1));
				vertex_vector[prev_index].total_weight++;
			}
			prev_index = curr_index;
			is_start = false;
			word = "";
			line_word_count++;
		}
		// if (line_word_count && last_char != '.') vertex_vector[prev_index].end_count++;
		if (line_word_count) vertex_vector[prev_index].end_count++;
	}
	// Prompt
	cout << "1. Print Graph and Quit\n";
	cout << "2. Generate Random Lyrics\n";
	cout << "3. View on BRIDGES\n";
	int choice = read();
	// Print
	if (choice == 1) {
		print_vertex_vector(vertex_vector);
		cout << "Total Start Count: " << TOTAL_START_COUNT << endl;
		exit(0);
	}
	// Generate
	else if (choice == 2) {
		// Prompt generation
		if (!TOTAL_START_COUNT) {
			cout << "Read no sentences, bailing out now...\n";
			exit(1);
		}
		const int sentence_count = read("How many sentences do you wish to make?\n");
		const int seed = read("Please enter the random seed:\n");
		srand(seed);
		// For each sentence
		for (int sentence_index = 0; sentence_index < sentence_count; sentence_index++) {
			string sentence = "";
			NOT_BRIDGES::Vertex* curr_vertex = nullptr;
			// Pick starting word
			int start_roll = rand() % TOTAL_START_COUNT;
			for (int i = 0; i < static_cast<int>(vertex_vector.size()); i++) {
				start_roll -= vertex_vector.at(i).start_count;
				if (start_roll < 0) {
					curr_vertex = &vertex_vector.at(i);
					add_word_to_sentence(sentence, curr_vertex->word, true);
					break;
				}
			}
			// Pick next word
			while (true) {
				if (curr_vertex->total_weight == 0) break;
				int word_roll = rand() % curr_vertex->total_weight;
				word_roll -= curr_vertex->end_count;
				for (int i = 0; i < static_cast<int>(curr_vertex->edge_vector.size()); i++) {
					word_roll -= curr_vertex->edge_vector.at(i).weight;
					if (word_roll < 0) {
						curr_vertex = &vertex_vector.at(curr_vertex->edge_vector.at(i).index);
						add_word_to_sentence(sentence, curr_vertex->word, false);
						break;
					}
				}
				// Handle comma
				float percentage = (static_cast<float>(curr_vertex->comma_count) / static_cast<float>(curr_vertex->word_count)) * 100.0f;
				int comma_roll = (rand() % 100) + 1;
				if (comma_roll <= percentage) sentence += ",";
			}
			sentence += ".";
			cout << sentence << endl;
		}
	}
	else if (choice == 3) {
		// Init bridges api
		Bridges bridges(2, "galaxy227", "818750556369");
		bridges.setTitle("Bridges Markov");
    	bridges.setDescription("GraphAdjList");
		// Init variables
		GraphAdjList<string, string, string> graph; // Key, Vertex data, Edge data
		const string start_str = "START NODE";
		const string end_str = "END NODE";
		graph.addVertex(start_str, start_str);
		graph.getVertex(start_str)->setColor("lime");
		graph.addVertex(end_str, end_str);
		graph.getVertex(end_str)->setColor("red");
		// Add graph vertex
		const int vertex_count = static_cast<int>(vertex_vector.size());
		for (int i = 0; i < vertex_count; i++) {
			graph.addVertex(vertex_vector.at(i).word, vertex_vector.at(i).word);
			graph.getVertex(vertex_vector.at(i).word)->setColor("blue");
		}
		// Add graph edge
		for (int i = 0; i < vertex_count; i++) {
			NOT_BRIDGES::Vertex* vertex = &vertex_vector.at(i);
			if (vertex->start_count > 0) graph.addEdge(start_str, vertex->word, to_string(vertex->start_count));
			if (vertex->end_count > 0) graph.addEdge(vertex->word, end_str, to_string(vertex->end_count));
			for (int j = 0; j < static_cast<int>(vertex->edge_vector.size()); j++) {
				graph.addEdge(vertex->word, vertex_vector.at(vertex->edge_vector.at(j).index).word, to_string(vertex->edge_vector.at(j).weight));
			}
		}
		// Visualize
		bridges.setDataStructure(&graph);
		bridges.visualize();
	}

	return 0;
}
