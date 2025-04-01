#include "/public/read.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept> // TODO
using namespace std;

uint32_t TOTAL_START_COUNT = 0;

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
	vector<Edge> edge_vector;

	Vertex(const string word) : word_count(0), start_count(0), end_count(0), comma_count(0), total_weight(0), word(word) {}
};

void print_vertex_vector(vector<Vertex> &vertex_vector) {
	for (const Vertex &v : vertex_vector) {
		// Print
		cout << 
		"Vertex " << v.word << 
		" - Count: " << v.word_count <<
		" TEW: " << v.total_weight <<
		" Start Count: " << v.start_count <<
		" End Count: " << v.end_count <<
		" Comma Count: " << v.comma_count << 
		" Edges:" << endl;
		for (const Edge &e : v.edge_vector) cout << '\t' << vertex_vector[e.index].word << ": " << e.weight << endl;
	}
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

int main() {
	// Data
	vector<Vertex> vertex_vector;
	unordered_map<string, uint32_t> word_map;
	// File
	const string filename = read("Please enter a text file to open:\n");
	ifstream file(filename);
	if (!file) { cout << "Bad filename yo\n"; exit(EXIT_FAILURE); }
	// Read
	string line;
	while (getline(file, line)) {
		if (line.empty()) continue;
		line += ' ';
		TOTAL_START_COUNT++;
		bool is_start = true;
		string word = "";
		uint32_t curr_index = 0;
		uint32_t prev_index = 0;
		// For each word in the line
		for (int i = 0; i < static_cast<int>(line.length()); i++) {
			if (line[i] == '=') continue;
			if (line[i] != ' ') { word += line[i]; continue; }
			if (word.empty()) continue;
			for (char &c : word) c = toupper(c);
			remove_brackets(word);
			// Handle punctuation
			const char last_char = word[word.size() - 1];
			bool is_comma = false;
			if (last_char == ',') {
				word = word.substr(0, word.size() - 1);
				is_comma = true;
			}
			if (last_char == '.' || last_char == '?' || last_char == '!') word = word.substr(0, word.size() - 1);
			// If word does NOT exist in data
			if (word_map.find(word) == word_map.end()) {
				word_map[word] = vertex_vector.size();
				vertex_vector.push_back(Vertex(word));
			}
			// Process word
			curr_index = word_map[word];
			vertex_vector[curr_index].word_count++;
			if (is_comma) vertex_vector[curr_index].comma_count++;
			if (is_start) vertex_vector[curr_index].start_count++;
			else {
				bool is_edge_exist = false;
				for (int j = 0; j < static_cast<int>(vertex_vector[prev_index].edge_vector.size()); j++) {
					if (curr_index == vertex_vector[prev_index].edge_vector.at(j).index) {
						vertex_vector[prev_index].edge_vector.at(j).weight++;
						is_edge_exist = true;
						break;
					}
				}
				if (!is_edge_exist) vertex_vector[prev_index].edge_vector.push_back(Edge(curr_index, 1));
				vertex_vector[prev_index].total_weight++;
			}
			prev_index = curr_index;
			is_start = false;
			word = "";
		}
		vertex_vector[prev_index].end_count++;
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
			Vertex* curr_vertex = nullptr;
			// Pick starting word
			int start_roll = rand() % TOTAL_START_COUNT;
			for (int i = 0; i < static_cast<int>(vertex_vector.size()); i++) {
				start_roll -= vertex_vector.at(i).start_count;
				if (start_roll < 0) {
					curr_vertex = &vertex_vector.at(i);
					sentence += curr_vertex->word;
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
						sentence += " " + curr_vertex->word;
						break;
					}
				}
				// Handle comma
				float percentage = (static_cast<float>(curr_vertex->comma_count) / static_cast<float>(curr_vertex->word_count)) * 100.0f;
				int comma_threshold = static_cast<int>(percentage);
				int comma_roll = (rand() % 100) + 1;
				if (comma_roll <= comma_threshold) sentence += ",";
			}
			cout << sentence << endl;
		}
	}

	return 0;
}
