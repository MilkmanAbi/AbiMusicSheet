// ams to JSON

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <cctype>

// ============================================
// Error Tracking
// ============================================
struct ParseError {
    int line_number;
    std::string line_content;
    std::string error_message;
    std::string error_type;  // "SYNTAX", "SEMANTIC", "LOGIC", etc.
};

// ============================================
// JSON Builder (Simple)
// ============================================
class JSON {
private:
    std::ostringstream ss;
    bool first = true;
    int indent_level = 0;

    std::string indent() {
        return std::string(indent_level * 2, ' ');
    }

public:
    void startObject() {
        ss << "{\n";
        indent_level++;
        first = true;
    }

    void endObject() {
        ss << "\n";
        indent_level--;
        ss << indent() << "}";
        first = false;
    }

    void startArray(const std::string& key) {
        if (!first) ss << ",\n";
        ss << indent() << "\"" << key << "\": [\n";
        indent_level++;
        first = true;
    }

    void endArray() {
        ss << "\n";
        indent_level--;
        ss << indent() << "]";
        first = false;
    }

    void addString(const std::string& key, const std::string& value) {
        if (!first) ss << ",\n";
        ss << indent() << "\"" << key << "\": \"" << value << "\"";
        first = false;
    }

    void addNumber(const std::string& key, int value) {
        if (!first) ss << ",\n";
        ss << indent() << "\"" << key << "\": " << value;
        first = false;
    }

    void addNumber(const std::string& key, double value) {
        if (!first) ss << ",\n";
        ss << indent() << "\"" << key << "\": " << value;
        first = false;
    }

    void addBool(const std::string& key, bool value) {
        if (!first) ss << ",\n";
        ss << indent() << "\"" << key << "\": " << (value ? "true" : "false");
        first = false;
    }

    void addRaw(const std::string& value) {
        if (!first) ss << ",\n";
        ss << indent() << value;
        first = false;
    }

    std::string toString() {
        return ss.str();
    }
};

// ============================================
// Data Structures
// ============================================
struct Note {
    int degree;           // 1-7
    std::string accidental; // "#", "b", or ""
    int octave_shift;     // ^1, ^-1, etc.
    double duration;      // in beats
    bool is_dotted;
    std::string articulation; // "!", "~", ">", "(h)"
    std::string dynamic;  // "p", "f", "mf", etc.
    bool is_rest;

    Note() : degree(0), octave_shift(0), duration(1.0), is_dotted(false), is_rest(false) {}
};

struct Chord {
    std::vector<Note> notes;
    double duration;
    bool is_dotted;

    Chord() : duration(1.0), is_dotted(false) {}
};

struct Hand {
    std::vector<std::vector<Chord>> chunks; // chunks separated by ||
};

struct Segment {
    int id;
    std::string name;
    int tempo;
    Hand left;
    Hand right;
    int definition_line;  // Track where it was defined
};

struct Metadata {
    std::string title;
    std::string composer;
    std::string key;
    int tempo;
    std::string time_signature;
    int difficulty;
};

struct MapBlock {
    std::string key;
    std::string scale;
    std::map<int, std::string> note_mapping;
    bool defined;
    int line_number;
    
    MapBlock() : defined(false), line_number(0) {}
};

// ============================================
// Utility Functions
// ============================================
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(trim(item));
    }
    return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

std::string removeComments(const std::string& line) {
    size_t pos = line.find("//");
    return (pos != std::string::npos) ? line.substr(0, pos) : line;
}

std::string replaceExtension(const std::string& filename, const std::string& new_ext) {
    size_t last_dot = filename.find_last_of('.');
    size_t last_slash = filename.find_last_of("/\\");
    
    if (last_dot == std::string::npos || (last_slash != std::string::npos && last_dot < last_slash)) {
        return filename + new_ext;
    }
    
    return filename.substr(0, last_dot) + new_ext;
}

// ============================================
// Parser Class
// ============================================
class AMSParser {
private:
    std::vector<std::string> lines;
    std::vector<std::string> original_lines;  // Keep original for error reporting
    size_t current_line;
    Metadata metadata;
    MapBlock map_block;
    std::map<std::string, std::string> macros;
    std::map<std::string, int> macro_definitions;  // Track where macros were defined
    std::vector<Segment> segments;
    std::map<int, int> segment_definitions;  // id -> line number
    std::map<std::string, int> segment_name_definitions;  // name -> line number
    std::vector<ParseError> errors;
    bool has_main_block;
    std::set<std::string> valid_keys = {"C", "D", "E", "F", "G", "A", "B"};
    std::set<std::string> valid_scales = {"Major", "Minor", "HarmonicMinor"};

public:
    AMSParser(const std::string& filename) : current_line(0), has_main_block(false) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            ParseError err;
            err.line_number = 0;
            err.error_type = "FILE";
            err.error_message = "Cannot open file: " + filename;
            errors.push_back(err);
            return;
        }

        std::string line;
        int line_num = 1;
        while (std::getline(file, line)) {
            original_lines.push_back(line);
            line = removeComments(line);
            line = trim(line);
            if (!line.empty()) {
                lines.push_back(line);
            } else {
                lines.push_back("");  // Keep empty lines for line number tracking
            }
            line_num++;
        }
    }

    bool hasErrors() const {
        return !errors.empty();
    }

    void printErrors() const {
        std::cerr << "\n╔════════════════════════════════════════════════════════════════╗\n";
        std::cerr << "║                    COMPILATION FAILED                          ║\n";
        std::cerr << "╚════════════════════════════════════════════════════════════════╝\n\n";
        
        for (const auto& err : errors) {
            std::cerr << "┌─ [" << err.error_type << " ERROR] ";
            std::cerr << "at line " << err.line_number << "\n";
            std::cerr << "│\n";
            
            if (!err.line_content.empty()) {
                std::cerr << "│  " << err.line_number << " │ " << err.line_content << "\n";
                std::cerr << "│    │ ";
                for (size_t i = 0; i < err.line_content.length(); i++) {
                    std::cerr << "^";
                }
                std::cerr << "\n";
            }
            
            std::cerr << "│\n";
            std::cerr << "└─ " << err.error_message << "\n\n";
        }
        
        std::cerr << "Total errors: " << errors.size() << "\n";
    }

    void addError(const std::string& type, const std::string& message, int line_num = -1) {
        ParseError err;
        err.error_type = type;
        err.error_message = message;
        
        if (line_num == -1) {
            line_num = current_line;
        }
        
        err.line_number = line_num + 1;  // Convert to 1-indexed
        if (line_num >= 0 && line_num < original_lines.size()) {
            err.line_content = trim(original_lines[line_num]);
        }
        
        errors.push_back(err);
    }

    bool parse() {
        // Parse metadata
        parseMetadata();

        // Parse Map block (required)
        if (!parseMap()) {
            addError("SEMANTIC", "Missing required Map block - every AMS file must define a Map");
            return false;
        }

        // Validate Map block
        if (!validateMap()) {
            return false;
        }

        // Generate note mapping
        generateNoteMapping();

        // Parse macros and segments
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = lines[current_line];

            if (startsWith(line, "Define ")) {
                parseMacro();
            } else if (startsWith(line, "Segment(")) {
                parseSegment();
            } else if (startsWith(line, "Main()")) {
                has_main_block = true;
                parseMain();
                break;
            } else {
                current_line++;
            }
        }

        // Validate
        if (!has_main_block) {
            addError("SEMANTIC", "Missing required Main() block - every AMS file must define playback order");
            return false;
        }

        // If we have segments, validate them
        if (!segments.empty()) {
            validateSegments();
        }

        return errors.empty();
    }

    std::string toJSON() {
        JSON json;
        json.startObject();

        // Metadata
        json.addString("version", "3.0-Beta");
        json.addString("title", metadata.title);
        json.addString("composer", metadata.composer);
        json.addString("key", metadata.key);
        json.addNumber("tempo", metadata.tempo);
        json.addString("timeSignature", metadata.time_signature);
        json.addNumber("difficulty", metadata.difficulty);

        // Map
        json.startArray("map");
        json.startObject();
        json.addString("key", map_block.key);
        json.addString("scale", map_block.scale);
        json.startArray("noteMapping");
        for (int i = 1; i <= 7; i++) {
            json.addRaw("\"" + map_block.note_mapping[i] + "\"");
        }
        json.endArray();
        json.endObject();
        json.endArray();

        // Segments
        json.startArray("segments");
        for (const auto& seg : segments) {
            json.addRaw(segmentToJSON(seg));
        }
        json.endArray();

        json.endObject();
        return json.toString();
    }

private:
    void parseMetadata() {
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = lines[current_line];

            if (startsWith(line, "Title:")) {
                metadata.title = extractValue(line);
            } else if (startsWith(line, "Composer:")) {
                metadata.composer = extractValue(line);
            } else if (startsWith(line, "Key:")) {
                metadata.key = extractValue(line);
            } else if (startsWith(line, "Tempo:")) {
                std::string tempo_str = extractValue(line);
                try {
                    metadata.tempo = std::stoi(tempo_str);
                    if (metadata.tempo <= 0 || metadata.tempo > 300) {
                        addError("LOGIC", "Tempo must be between 1 and 300 BPM, got: " + tempo_str);
                    }
                } catch (...) {
                    addError("SYNTAX", "Invalid tempo value: " + tempo_str);
                }
            } else if (startsWith(line, "TimeSignature:")) {
                metadata.time_signature = extractValue(line);
            } else if (startsWith(line, "Difficulty:")) {
                std::string diff_str = extractValue(line);
                try {
                    metadata.difficulty = std::stoi(diff_str);
                    if (metadata.difficulty < 0 || metadata.difficulty > 10) {
                        addError("LOGIC", "Difficulty must be between 0 and 10, got: " + diff_str);
                    }
                } catch (...) {
                    addError("SYNTAX", "Invalid difficulty value: " + diff_str);
                }
            } else if (startsWith(line, "Map {")) {
                break;
            }

            current_line++;
        }
    }

    bool parseMap() {
        if (current_line >= lines.size()) return false;

        std::string line = lines[current_line];
        if (!startsWith(line, "Map {")) return false;

        map_block.defined = true;
        map_block.line_number = current_line;
        current_line++;

        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            line = trim(lines[current_line]);

            if (line == "}") {
                current_line++;
                return true;
            }

            if (startsWith(line, "Key:")) {
                map_block.key = extractValue(line);
            } else if (startsWith(line, "Scale:")) {
                map_block.scale = extractValue(line);
            } else {
                addError("SYNTAX", "Unexpected content in Map block: " + line);
            }

            current_line++;
        }

        addError("SYNTAX", "Unclosed Map block - missing '}'", map_block.line_number);
        return false;
    }

    bool validateMap() {
        if (map_block.key.empty()) {
            addError("SEMANTIC", "Map block must specify a Key", map_block.line_number);
            return false;
        }
        
        if (map_block.scale.empty()) {
            addError("SEMANTIC", "Map block must specify a Scale", map_block.line_number);
            return false;
        }
        
        if (valid_keys.find(map_block.key) == valid_keys.end()) {
            addError("LOGIC", "Invalid key '" + map_block.key + "'. Valid keys: C, D, E, F, G, A, B", map_block.line_number);
            return false;
        }
        
        if (valid_scales.find(map_block.scale) == valid_scales.end()) {
            addError("LOGIC", "Invalid scale '" + map_block.scale + "'. Valid scales: Major, Minor, HarmonicMinor", map_block.line_number);
            return false;
        }
        
        return true;
    }

    void generateNoteMapping() {
        std::map<std::string, std::vector<std::string>> scale_mappings = {
            {"C_Major", {"C", "D", "E", "F", "G", "A", "B"}},
            {"A_Minor", {"A", "B", "C", "D", "E", "F", "G"}},
            {"G_Major", {"G", "A", "B", "C", "D", "E", "F#"}},
            {"D_Major", {"D", "E", "F#", "G", "A", "B", "C#"}},
            {"E_Major", {"E", "F#", "G#", "A", "B", "C#", "D#"}},
            {"F_Major", {"F", "G", "A", "Bb", "C", "D", "E"}},
            {"B_Major", {"B", "C#", "D#", "E", "F#", "G#", "A#"}},
            {"E_Minor", {"E", "F#", "G", "A", "B", "C", "D"}},
            {"D_Minor", {"D", "E", "F", "G", "A", "Bb", "C"}},
        };

        std::string key = map_block.key + "_" + map_block.scale;
        std::vector<std::string> scale;

        if (scale_mappings.find(key) != scale_mappings.end()) {
            scale = scale_mappings[key];
        } else {
            // Default to C Major
            scale = scale_mappings["C_Major"];
        }

        for (int i = 0; i < 7; i++) {
            map_block.note_mapping[i + 1] = scale[i];
        }
    }

    void parseMacro() {
        std::string line = lines[current_line];
        std::regex define_regex(R"(Define\s+([A-Z_]+)\s*\{)");
        std::smatch match;

        if (std::regex_search(line, match, define_regex)) {
            std::string macro_name = match[1];
            int definition_line = current_line;
            
            // Check for redefinition
            if (macro_definitions.find(macro_name) != macro_definitions.end()) {
                addError("REDEFINITION", "Macro '" + macro_name + "' already defined at line " + 
                        std::to_string(macro_definitions[macro_name] + 1));
                current_line++;
                return;
            }
            
            macro_definitions[macro_name] = definition_line;
            std::string macro_body;

            current_line++;
            bool found_close = false;
            
            while (current_line < lines.size()) {
                line = trim(lines[current_line]);
                if (line == "}") {
                    macros[macro_name] = macro_body;
                    current_line++;
                    found_close = true;
                    break;
                }
                macro_body += line + " ";
                current_line++;
            }
            
            if (!found_close) {
                addError("SYNTAX", "Unclosed Define block for macro '" + macro_name + "' - missing '}'", definition_line);
            }
        } else {
            addError("SYNTAX", "Invalid Define syntax - expected: Define MACRO_NAME {");
        }
    }

    void parseSegment() {
        std::string line = lines[current_line];
        std::regex seg_regex(R"(Segment\((\d+),\s*([A-Z_]+)\))");
        std::smatch match;

        if (std::regex_search(line, match, seg_regex)) {
            Segment seg;
            seg.id = std::stoi(match[1]);
            seg.name = match[2];
            seg.tempo = metadata.tempo;
            seg.definition_line = current_line;
            
            // Check for duplicate segment ID
            if (segment_definitions.find(seg.id) != segment_definitions.end()) {
                addError("REDEFINITION", "Segment with ID " + std::to_string(seg.id) + 
                        " already defined at line " + std::to_string(segment_definitions[seg.id] + 1));
                current_line++;
                return;
            }
            
            // Check for duplicate segment name
            if (segment_name_definitions.find(seg.name) != segment_name_definitions.end()) {
                addError("REDEFINITION", "Segment with name '" + seg.name + 
                        "' already defined at line " + std::to_string(segment_name_definitions[seg.name] + 1));
                current_line++;
                return;
            }
            
            segment_definitions[seg.id] = current_line;
            segment_name_definitions[seg.name] = current_line;

            current_line++;
            bool found_end = false;
            bool has_left = false;
            bool has_right = false;

            while (current_line < lines.size()) {
                if (lines[current_line].empty()) {
                    current_line++;
                    continue;
                }
                
                line = trim(lines[current_line]);

                if (line == "END;") {
                    found_end = true;
                    
                    // Warn if missing hands
                    if (!has_left && !has_right) {
                        addError("LOGIC", "Segment '" + seg.name + "' has no hand blocks defined", seg.definition_line);
                    }
                    
                    segments.push_back(seg);
                    current_line++;
                    break;
                }

                if (startsWith(line, "Tempo(")) {
                    seg.tempo = extractNumber(line);
                    if (seg.tempo <= 0 || seg.tempo > 300) {
                        addError("LOGIC", "Invalid tempo in segment: " + std::to_string(seg.tempo));
                    }
                } else if (startsWith(line, "Begin.LEFT {")) {
                    if (has_left) {
                        addError("REDEFINITION", "Multiple Begin.LEFT blocks in segment '" + seg.name + "'");
                    }
                    has_left = true;
                    seg.left = parseHand();
                } else if (startsWith(line, "Begin.RIGHT {")) {
                    if (has_right) {
                        addError("REDEFINITION", "Multiple Begin.RIGHT blocks in segment '" + seg.name + "'");
                    }
                    has_right = true;
                    seg.right = parseHand();
                } else {
                    addError("SYNTAX", "Unexpected content in segment: " + line);
                }

                current_line++;
            }
            
            if (!found_end) {
                addError("SYNTAX", "Segment '" + seg.name + "' missing END; terminator", seg.definition_line);
            }
        } else {
            addError("SYNTAX", "Invalid Segment syntax - expected: Segment(id, NAME)");
        }
    }

    Hand parseHand() {
        Hand hand;
        int hand_start_line = current_line;
        current_line++;

        std::string chunk_data;
        bool found_close = false;
        
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = trim(lines[current_line]);

            if (line == "}") {
                found_close = true;
                if (!chunk_data.empty()) {
                    hand.chunks = parseChunks(chunk_data);
                }
                break;
            }

            if (startsWith(line, "SYNC()") || startsWith(line, "Position(")) {
                current_line++;
                continue;
            }

            chunk_data += line + " ";
            current_line++;
        }
        
        if (!found_close) {
            addError("SYNTAX", "Unclosed hand block - missing '}'", hand_start_line);
        }

        return hand;
    }

    std::vector<std::vector<Chord>> parseChunks(const std::string& data) {
        std::vector<std::vector<Chord>> result;
        std::vector<std::string> chunks = split(data, '|');

        for (const auto& chunk_str : chunks) {
            if (chunk_str.empty()) continue;

            std::vector<Chord> chords = parseChordSequence(chunk_str);
            if (!chords.empty()) {
                result.push_back(chords);
            }
        }

        return result;
    }

    std::vector<Chord> parseChordSequence(const std::string& str) {
        std::vector<Chord> chords;
        std::vector<std::string> parts = split(str, ',');

        for (const auto& part : parts) {
            std::string trimmed = trim(part);
            if (!trimmed.empty()) {
                Chord chord = parseChord(trimmed);
                chords.push_back(chord);
            }
        }

        return chords;
    }

    Chord parseChord(const std::string& str) {
        Chord chord;

        // Check if this is a chord (contains dots between digits)
        // Pattern: digit.digit or digit.digit.duration
        bool is_chord = false;
        for (size_t i = 0; i < str.size() - 1; i++) {
            if (str[i] == '.' && i > 0 && i + 1 < str.size() && 
                std::isdigit(str[i-1]) && std::isdigit(str[i+1])) {
                is_chord = true;
                break;
            }
        }
        
        if (is_chord) {
            // Parse as chord: 1.3.5 or 1.3.5.h
            std::vector<std::string> note_parts;
            std::string current;
            
            for (size_t i = 0; i < str.size(); i++) {
                char c = str[i];
                
                // Check if this dot separates note degrees
                if (c == '.' && i + 1 < str.size() && std::isdigit(str[i + 1])) {
                    if (!current.empty()) {
                        note_parts.push_back(current);
                        current.clear();
                    }
                } else {
                    current += c;
                }
            }
            
            if (!current.empty()) {
                note_parts.push_back(current);
            }

            // The last part contains the duration suffix
            if (!note_parts.empty()) {
                std::string last = note_parts.back();
                note_parts.pop_back();
                
                // Parse the last part to get duration
                Note duration_note = parseNote(last);
                chord.duration = duration_note.duration;
                chord.is_dotted = duration_note.is_dotted;
                
                // If last part also has a note degree, add it back
                if (duration_note.degree > 0) {
                    note_parts.push_back(std::to_string(duration_note.degree));
                }

                // Parse all chord notes (just the degree numbers)
                for (const auto& np : note_parts) {
                    if (!np.empty() && std::isdigit(np[0])) {
                        Note n;
                        n.degree = std::stoi(np);
                        if (n.degree < 1 || n.degree > 7) {
                            addError("LOGIC", "Invalid note degree in chord: " + np + " (must be 1-7)");
                        }
                        n.duration = chord.duration;
                        n.is_dotted = chord.is_dotted;
                        n.is_rest = false;
                        chord.notes.push_back(n);
                    }
                }
            }
        } else {
            // Single note
            Note n = parseNote(str);
            if (!n.is_rest && n.degree != 0 && (n.degree < 1 || n.degree > 7)) {
                addError("LOGIC", "Invalid note degree: " + str + " (must be 1-7)");
            }
            chord.notes.push_back(n);
            chord.duration = n.duration;
            chord.is_dotted = n.is_dotted;
        }

        return chord;
    }

    Note parseNote(const std::string& str) {
        Note note;

        if (str.empty()) return note;

        if (str[0] == 'R') {
            note.is_rest = true;
            parseDuration(str.substr(1), note);
            return note;
        }

        size_t i = 0;
        std::string degree_str;
        while (i < str.size() && std::isdigit(str[i])) {
            degree_str += str[i++];
        }

        if (!degree_str.empty()) {
            note.degree = std::stoi(degree_str);
        }

        while (i < str.size()) {
            char c = str[i];

            if (c == '#') {
                note.accidental = "#";
                i++;
            } else if (c == 'b') {
                note.accidental = "b";
                i++;
            } else if (c == '^') {
                i++;
                std::string shift_str;
                if (i < str.size() && str[i] == '-') {
                    shift_str += '-';
                    i++;
                }
                while (i < str.size() && std::isdigit(str[i])) {
                    shift_str += str[i++];
                }
                if (!shift_str.empty()) {
                    note.octave_shift = std::stoi(shift_str);
                }
            } else if (c == '!' || c == '~' || c == '>') {
                note.articulation = std::string(1, c);
                i++;
            } else if (c == '(' && i + 1 < str.size() && str[i + 1] == 'h') {
                note.articulation = "(h)";
                i += 3;
            } else if (c == 'p' || c == 'f' || c == 'm') {
                std::string dyn;
                while (i < str.size() && (str[i] == 'p' || str[i] == 'f' || str[i] == 'm')) {
                    dyn += str[i++];
                }
                note.dynamic = dyn;
            } else if (c == '.') {
                parseDuration(str.substr(i), note);
                break;
            } else {
                i++;
            }
        }

        return note;
    }

    void parseDuration(const std::string& str, Note& note) {
        if (str.empty()) {
            note.duration = 1.0;
            return;
        }

        if (str == ".h") {
            note.duration = 2.0;
        } else if (str == ".w") {
            note.duration = 4.0;
        } else if (str == ".e") {
            note.duration = 0.5;
        } else if (str == ".s") {
            note.duration = 0.25;
        } else if (str == ".h.") {
            note.duration = 3.0;
            note.is_dotted = true;
        } else if (str == ".") {
            note.duration = 1.5;
            note.is_dotted = true;
        } else if (str == ".e.") {
            note.duration = 0.75;
            note.is_dotted = true;
        } else {
            note.duration = 1.0;
        }
    }

    void parseMain() {
        int main_line = current_line;
        std::string line = lines[current_line];
        
        bool found_open = false;
        bool found_close = false;
        std::set<int> used_segments;
        
        // Check if opening brace is on same line as Main()
        if (line.find('{') != std::string::npos) {
            found_open = true;
            current_line++;
        } else {
            current_line++;
            
            // Look for opening brace on next lines
            while (current_line < lines.size()) {
                if (lines[current_line].empty()) {
                    current_line++;
                    continue;
                }
                
                line = trim(lines[current_line]);
                
                if (line == "{") {
                    found_open = true;
                    current_line++;
                    break;
                }
                
                addError("SYNTAX", "Expected '{' after Main()");
                return;
            }
        }
        
        if (!found_open) {
            addError("SYNTAX", "Main() block missing opening '{'", main_line);
            return;
        }
        
        // Parse Main block content
        while (current_line < lines.size()) {
            if (lines[current_line].empty()) {
                current_line++;
                continue;
            }
            
            std::string line = trim(lines[current_line]);
            
            if (line == "}") {
                found_close = true;
                current_line++;
                break;
            }
            
            // Check for Segment calls
            if (startsWith(line, "Segment(")) {
                std::regex seg_call_regex(R"(Segment\((\d+),\s*([A-Z_]+)\);)");
                std::smatch match;
                
                if (std::regex_search(line, match, seg_call_regex)) {
                    int seg_id = std::stoi(match[1]);
                    std::string seg_name = match[2];
                    
                    // Check if segment exists
                    if (segment_definitions.find(seg_id) == segment_definitions.end()) {
                        addError("SEMANTIC", "Undefined segment ID: " + std::to_string(seg_id) + 
                                " (Segment not defined before Main block)");
                    } else {
                        used_segments.insert(seg_id);
                    }
                } else {
                    addError("SYNTAX", "Invalid Segment call syntax - expected: Segment(id, NAME);");
                }
            }
            // Check for Repeat blocks
            else if (startsWith(line, "Repeat(")) {
                std::regex repeat_regex(R"(Repeat\((\d+)\)\s*\{)");
                std::smatch match;
                
                if (std::regex_search(line, match, repeat_regex)) {
                    int repeat_count = std::stoi(match[1]);
                    if (repeat_count <= 0) {
                        addError("LOGIC", "Repeat count must be positive, got: " + std::to_string(repeat_count));
                    }
                    // TODO: Parse repeat block content
                } else {
                    addError("SYNTAX", "Invalid Repeat syntax - expected: Repeat(count) {");
                }
            }
            // Check for inline hand commands
            else if (startsWith(line, "LEFT:") || startsWith(line, "RIGHT:")) {
                // Inline commands are allowed
            }
            else if (line != "{" && line != "}") {
                addError("SYNTAX", "Unexpected content in Main block: " + line);
            }
            
            current_line++;
        }
        
        if (!found_close) {
            addError("SYNTAX", "Main() block missing closing '}'", main_line);
        }
        
        // Warn about unused segments
        for (const auto& seg : segments) {
            if (used_segments.find(seg.id) == used_segments.end()) {
                // This is just a warning, not an error
                // Could add warning system if desired
            }
        }
    }

    void validateSegments() {
        for (const auto& seg : segments) {
            // Check if both hands have data
            bool left_empty = seg.left.chunks.empty();
            bool right_empty = seg.right.chunks.empty();
            
            if (left_empty && right_empty) {
                addError("LOGIC", "Segment '" + seg.name + "' has no musical content", seg.definition_line);
                continue;
            }
            
            // Validate chunk alignment between hands
            if (!left_empty && !right_empty) {
                validateChunkAlignment(seg);
            }
        }
    }

    void validateChunkAlignment(const Segment& seg) {
        // Check if chunks have matching durations
        size_t max_chunks = std::max(seg.left.chunks.size(), seg.right.chunks.size());
        
        for (size_t i = 0; i < max_chunks; i++) {
            double left_duration = 0.0;
            double right_duration = 0.0;
            
            if (i < seg.left.chunks.size()) {
                for (const auto& chord : seg.left.chunks[i]) {
                    left_duration += chord.duration;
                }
            }
            
            if (i < seg.right.chunks.size()) {
                for (const auto& chord : seg.right.chunks[i]) {
                    right_duration += chord.duration;
                }
            }
            
            // Allow small floating point differences
            if (std::abs(left_duration - right_duration) > 0.01) {
                addError("LOGIC", "Duration mismatch in segment '" + seg.name + "' chunk " + 
                        std::to_string(i + 1) + ": LEFT=" + std::to_string(left_duration) + 
                        " beats, RIGHT=" + std::to_string(right_duration) + " beats", 
                        seg.definition_line);
            }
        }
    }

    std::string extractValue(const std::string& line) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string value = trim(line.substr(pos + 1));
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                return value.substr(1, value.size() - 2);
            }
            return value;
        }
        return "";
    }

    int extractNumber(const std::string& line) {
        std::regex num_regex(R"(\d+)");
        std::smatch match;
        if (std::regex_search(line, match, num_regex)) {
            return std::stoi(match[0]);
        }
        return 0;
    }

    std::string segmentToJSON(const Segment& seg) {
        JSON json;
        json.startObject();
        json.addNumber("id", seg.id);
        json.addString("name", seg.name);
        json.addNumber("tempo", seg.tempo);

        json.addRaw("\"left\": " + handToJSON(seg.left));
        json.addRaw("\"right\": " + handToJSON(seg.right));

        json.endObject();
        return json.toString();
    }

    std::string handToJSON(const Hand& hand) {
        JSON json;
        json.startArray("chunks");
        
        for (const auto& chunk : hand.chunks) {
            JSON chunk_json;
            chunk_json.startArray("chords");
            for (const auto& chord : chunk) {
                chunk_json.addRaw(chordToJSON(chord));
            }
            chunk_json.endArray();
            json.addRaw(chunk_json.toString());
        }
        
        json.endArray();
        return "{ " + json.toString() + " }";
    }

    std::string chordToJSON(const Chord& chord) {
        JSON json;
        json.startObject();
        json.addNumber("duration", chord.duration);
        json.addBool("isDotted", chord.is_dotted);
        
        json.startArray("notes");
        for (const auto& note : chord.notes) {
            json.addRaw(noteToJSON(note));
        }
        json.endArray();
        
        json.endObject();
        return json.toString();
    }

    std::string noteToJSON(const Note& note) {
        JSON json;
        json.startObject();
        json.addBool("isRest", note.is_rest);
        
        if (!note.is_rest && note.degree > 0) {
            json.addNumber("degree", note.degree);
            json.addString("accidental", note.accidental);
            json.addNumber("octaveShift", note.octave_shift);
            if (map_block.note_mapping.find(note.degree) != map_block.note_mapping.end()) {
                json.addString("pitch", map_block.note_mapping[note.degree]);
            } else {
                json.addString("pitch", "");
            }
        }
        
        json.addNumber("duration", note.duration);
        json.addBool("isDotted", note.is_dotted);
        json.addString("articulation", note.articulation);
        json.addString("dynamic", note.dynamic);
        
        json.endObject();
        return json.toString();
    }
};

// ============================================
// Main
// ============================================
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.ams>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              AMS Parser v3.0-Beta Compiler                     ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    std::cout << "Compiling: " << filename << "\n\n";

    AMSParser parser(filename);

    if (!parser.parse()) {
        parser.printErrors();
        return 1;
    }

    std::cout << "✓ Compilation successful!\n\n";
    
    // Generate JSON
    std::string json_output = parser.toJSON();
    
    // Write to file
    std::string output_filename = replaceExtension(filename, ".json");
    std::ofstream output_file(output_filename);
    
    if (output_file.is_open()) {
        output_file << json_output;
        output_file.close();
        std::cout << "✓ JSON output written to: " << output_filename << "\n\n";
        std::cout << "Output preview:\n";
        std::cout << "────────────────────────────────────────────────────────────────\n";
        
        // Print first 20 lines of JSON
        std::istringstream iss(json_output);
        std::string line;
        int line_count = 0;
        while (std::getline(iss, line) && line_count < 20) {
            std::cout << line << "\n";
            line_count++;
        }
        
        if (line_count == 20) {
            std::cout << "... (output truncated, see " << output_filename << " for full content)\n";
        }
        
        std::cout << "────────────────────────────────────────────────────────────────\n";
    } else {
        std::cerr << "\n✗ ERROR: Could not write to file: " << output_filename << std::endl;
        return 1;
    }

    return 0;
}
