#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <stdexcept>


std::string get_word_from_cin() {
    std::string cin_input_string;
    std::cin >> cin_input_string;

    return cin_input_string;
}


void display_vector(std::vector<std::string>& v) {
    for (const std::string& word: v) {
        std::cout << word << "\n";
    }
}


void addCorrectWords(const std::vector<std::string>& possible_words, const std::set<std::string>& dictionary, std::vector<std::string>& correct) {
    for (const std::string& possible_word: possible_words) {
        if (dictionary.find(possible_word) != dictionary.end()) {
            correct.push_back(possible_word);
        }
    }
}


struct WordParts {
    std::string firstPart;
    char firstLetter;
    char secondLetter;
    std::string secondPart;
};


std::vector<std::string> firstCorrect(const std::string& word) {
    const int n = static_cast<int>(word.length());
    //const int n_parts = n - 2;
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    std::vector<WordParts> parts;
    std::vector<std::string> possible_words;

    for (int i = 1; i < n; i++) {  // appending separated parts to vector using structure
        std::string firstPart = word.substr(0, i-1);
        char firstLetter = word[i-1];
        char secondLetter = word[i];
        std::string secondPart = word.substr(i+1);

        //std::cout << firstPart << " " << word[i-1] << " " << word[i] << " " << secondPart << "\n";
        //std::cout << firstPart + word[i-1] + word[i] + secondPart << "\n";
        parts.emplace_back(WordParts{firstPart, firstLetter, secondLetter, secondPart});
    }

    // We will be working with 4 types of errors:
    // 1. Misspelled letter, for example - mqtro
    // 2. Missing letter, for example - mtro
    // 3. Two letters swapped, for example - emtro
    // 4. Extra letter in the word, for example - metror

    possible_words.push_back(word);

    for (size_t i = 0; i != parts.size(); ++i) { // when replacing a letter or having an extra, we alsways neglect the firstLetter
        possible_words.push_back(parts[i].firstPart + parts[i].secondLetter + parts[i].firstLetter + parts[i].secondPart);               // swapped
        possible_words.push_back(parts[i].firstPart + parts[i].secondLetter + parts[i].secondPart);                                      // extra
        for (char const letter: alphabet) {
            if (letter != parts[i].firstLetter) { // if added here to not duplicate current replaced letter
                possible_words.push_back(parts[i].firstPart + letter + parts[i].secondLetter + parts[i].secondPart);                     // wrong
            }
            possible_words.push_back(parts[i].firstPart + letter + parts[i].firstLetter +  parts[i].secondLetter + parts[i].secondPart); // missing
        }
    }

    for (char const letter: alphabet) { // add situations for wrong and missing with the last letter
        if (letter != parts[n-2].secondLetter) { // if added here to not duplicate current replaced letter
            possible_words.push_back(parts[n-2].firstPart + parts[n-2].firstLetter + letter + parts[n-2].secondPart);                         // wrong last
        }
        possible_words.push_back(parts[n-2].firstPart + parts[n-2].firstLetter + letter +  parts[n-2].secondLetter + parts[n-2].secondPart);  // missing pre-last
        possible_words.push_back(parts[n-2].firstPart + parts[n-2].firstLetter +  parts[n-2].secondLetter + letter +  parts[n-2].secondPart); // missing last
    }

    return possible_words;
}


std::vector<std::string> secondCorrect(std::vector<std::string>& first_corrected) {
    std::vector<std::string> possible_words_second;

    for (std::string& possible_first: first_corrected) {
        std::vector<std::string> possible_second = firstCorrect(possible_first);
        possible_words_second.insert(possible_words_second.end(), possible_second.begin(), possible_second.end());
    }

    return possible_words_second;
}



std::vector<std::string> autoCorrect(const std::string& word, const std::set<std::string>& dictionary) {
    std::vector<std::string> variants;
    std::vector<std::string> first_variants = firstCorrect(word);

    addCorrectWords(first_variants, dictionary, variants);
    addCorrectWords(secondCorrect(first_variants), dictionary, variants);

    return variants;
}



std::string getCorrectedWord(const std::string& word, const std::set<std::string>& dictionary){
    // Perhaps add a sorting algorithm according to word's frequency
    std::vector<std::string> correct = autoCorrect(word, dictionary);
    if (correct.empty()) {
        throw std::runtime_error("No possible variants");
    }
    return correct[0];
}



int main() {

    std::ifstream fin("C:/Users/Admin/CLionProjects/AutoCorrect/google-10000-english.txt");
    // https://github.com/first20hours/google-10000-english

    if (!fin.is_open()) {
        std::cout << "Failed to open file";
        return 0;
    }

    // fill the dictionary with words from the file
    std::set<std::string> dictionary;
    while (!fin.eof()) {
        std::string word;
        std::getline(fin, word);
        if (word.size() > 2) {
            dictionary.insert(word);
        }
    }
    fin.close();

    std::cout << getCorrectedWord(get_word_from_cin(), dictionary);
}
