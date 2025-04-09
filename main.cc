#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <stdexcept>
#include <locale>
#include <string>
#include <fstream>
using namespace std;

const int valid_wchar_arr_size = 25;
const int punctuation_arr_size = valid_wchar_arr_size - 3;
const wchar_t valid_wchar_arr[] = {
	// Punctuation
	L'.',
	L',',
	L'!',
	L'?',
	L'-',
	L'+',
	L'—',
	L'&',
	L'*',
	L';',
	L':',
	L'<',
	L'>',
	L'{',
	L'}',
	L'(',
	L')',
	L'/',
	L'\\',
	L'\'',
	L'\"',
	// Other
	L' ',
	L'[',
	L']',
};

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
	wstring word;
	vector<Edge> edge_vector;

	Vertex(const wstring word) : word_count(0), start_count(0), end_count(0), comma_count(0), total_weight(0), word(word) {}
};

void print_vertex_vector(vector<Vertex> &vertex_vector) {
	for (const Vertex &v : vertex_vector) {
		// Print
		wcout << 
		"Vertex " << v.word << 
		" - Count: " << v.word_count <<
		" TEW: " << v.total_weight <<
		" Start Count: " << v.start_count <<
		" End Count: " << v.end_count <<
		" Comma Count: " << v.comma_count << 
		" Edges:" << endl;
		for (const Edge &e : v.edge_vector) wcout << '\t' << vertex_vector[e.index].word << ": " << e.weight << endl;
	}
}
bool is_valid_wchar(wchar_t &c) {
	if (isalpha(c) || isdigit(c)) return true;
	for (int i = 0; i < valid_wchar_arr_size; i++) {
		if (c == valid_wchar_arr[i]) return true;
	}
	return false;
}
void remove_brackets(wstring &s) {
	wstring retval;
	bool deleting = false;
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == '[') deleting = true;
		else if (s[i] == ']') deleting = false;
		else if (!deleting) retval.push_back(s[i]);
	}
	s = retval;
}
bool is_wchar_punctuation(wchar_t &c) {
	for (int i = 0; i < punctuation_arr_size; i++) {
		if (c == valid_wchar_arr[i]) return true;
	}
	return false;
}
bool remove_punctuation(wstring &word) {
	if (word.empty()) return false;
	// Is initial last wchar in word a comma
	bool is_comma = false;
	if (word[word.length() - 1] == ',') is_comma = true;
	// Bracket
	remove_brackets(word);
	// Punctuation
	while (is_wchar_punctuation(word[word.length() - 1])) {
		word = word.substr(0, word.length() - 1);	
	}
	return is_comma;
}

int main() {
    locale::global(locale(""));
	// Data
	vector<Vertex> vertex_vector;
	unordered_map<wstring, uint32_t> word_map;
	// File
	wcout << "Please enter a text file to open:\n";
	wstring wfilename = L"";
	wcin >> wfilename;
	string filename(wfilename.begin(), wfilename.end());
	wifstream file(filename);
	if (!file) { wcout << "Bad filename yo\n"; exit(EXIT_FAILURE); }
	// Read
	wstring line;
	while (getline(file, line)) {
		remove_brackets(line);
		if (line.empty()) continue;
		line += L' ';
		int line_word_count = 0;
		bool is_start = true;
		wstring word = L"";
		uint32_t curr_index = 0;
		uint32_t prev_index = 0;
		// For each wchar in the line
		for (int i = 0; i < static_cast<int>(line.length()); i++) {
			if (!is_valid_wchar(line[i])) continue;
			if (line[i] != L' ') { word += line[i]; continue; }
			if (word.empty()) continue;
			for (wchar_t &c : word) c = toupper(c);
			// Handle punctuation
			bool is_comma = remove_punctuation(word);
			if (word.empty()) continue;
			// If word does NOT exist in data
			if (word_map.find(word) == word_map.end()) {
				word_map[word] = vertex_vector.size();
				vertex_vector.push_back(Vertex(word));
			}
			// Process word
			curr_index = word_map[word];
			vertex_vector[curr_index].word_count++;
			if (is_comma) vertex_vector[curr_index].comma_count++;
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
				if (!is_edge_exist) vertex_vector[prev_index].edge_vector.push_back(Edge(curr_index, 1));
				vertex_vector[prev_index].total_weight++;
			}
			prev_index = curr_index;
			is_start = false;
			word = L"";
			line_word_count++;
		}
		if (line_word_count) vertex_vector[prev_index].end_count++;
	}
	// Prompt
	wcout << "1. Print Graph and Quit\n";
	wcout << "2. Generate Random Lyrics\n";
	wcout << "3. View on BRIDGES\n";
	int choice = 0;
	wcin >> choice;
	// Print
	if (choice == 1) {
		print_vertex_vector(vertex_vector);
		wcout << "Total Start Count: " << TOTAL_START_COUNT << endl;
		exit(0);
	}
	// Generate
	else if (choice == 2) {
		// Prompt generation
		if (!TOTAL_START_COUNT) {
			wcout << "Read no sentences, bailing out now...\n";
			exit(1);
		}
		wcout << "How many sentences do you wish to make?\n";
		int sentence_count = 0;
		wcin >> sentence_count;
		wcout << "Please enter the random seed:\n";
		int seed = 0;
		wcin >> seed;
		srand(seed);
		// For each sentence
		for (int sentence_index = 0; sentence_index < sentence_count; sentence_index++) {
			wstring sentence = L"";
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
						sentence += L" " + curr_vertex->word;
						break;
					}
				}
				// Handle comma
				float percentage = (static_cast<float>(curr_vertex->comma_count) / static_cast<float>(curr_vertex->word_count)) * 100.0f;
				int comma_threshold = static_cast<int>(percentage);
				int comma_roll = (rand() % 100) + 1;
				if (comma_roll <= comma_threshold) sentence += L",";
			}
			wcout << sentence << endl;
		}
	}

	return 0;
}
